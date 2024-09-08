#include <stdio.h>
#include <stdlib.h>


struct local_header_t
{
	char version[2];
	char flags[2];
	char compression[2];
	char modTime[2];
	char modDate[2];
	char crc32[4];
	char compressedSize[4];
	char uncompressedSize[4];
	char fileNameLen[2];
	char extraFieldName[2];
};

struct central_directory_t
{
	char version[2];
	char versNeeded[2];
	char flags[2];
	char compression[2];
	char modTime[2];
	char modDate[2];
	char crc32[4];
	char compressedSize[4];
	char uncompressedSize[4];
	char fileNameLen[2];
	char ExtraFieldLen[2];
	char FileCommLen[2];
	char diskStart[2];
	char internalAttr[2];
	char externalAttr[4];
	char offsetLocalHeader[4];

};

void checkInput(int argc, int arc_num);

int sigMatch(unsigned char *beginOfSig , unsigned char *endOfSig, unsigned char c);

unsigned short convert_FileNameLen(char fileNameLen[2]);

int main(int argc, char* argv[])
{
	checkInput(argc, 2);
	
	FILE *fp = fopen(argv[1], "rb");
	if (fp == NULL)
	{
		printf("Error. Not open file... \n");
		exit(1);
	}

	int findZip = 0, zipLocalHeader_find = 0; // sign of ZIP signature finded
	while(!feof(fp))
	{		
		findZip = 0; 		
		unsigned char c = getc(fp);
		unsigned char zipSig[4] = {0x50,0x4b}; //{0xff,0xd9};  {0x50,0x4b,0x03,0x04}
						       
		findZip = sigMatch(&zipSig[0], &zipSig[1], c);
		
		// when find zip signature 
		if (findZip == 1)
		{
			unsigned char secondPartOfSig[2];
			fread(&secondPartOfSig, sizeof(char), 2, fp);

			if ( ( (secondPartOfSig[0] << 8) | secondPartOfSig[1]) == 0x0304)
			{
				struct local_header_t local_header;
				if ( fread(&local_header, sizeof(char), sizeof(local_header), fp) == sizeof(local_header)  ) // 26
				{
					//printf("local file header: \n");
					unsigned short lenghtOfFile = convert_FileNameLen(local_header.fileNameLen);
					char filename[lenghtOfFile + 1];
						
					fread(&filename, sizeof(char), lenghtOfFile, fp);
					filename[lenghtOfFile] = '\0';
				       	printf("%s \n",filename);	
				
					zipLocalHeader_find = 1;
				}
				else
					printf("Error.Broken archive...\n");

			}

			else if( ( (secondPartOfSig[0] << 8) | secondPartOfSig[1])== 0x0506)
			{
				if (zipLocalHeader_find == 0)
					printf("End of central directory. Empty archive. \n");
			}

		
#if 0
		
			else if( ( (secondPartOfSig[0] << 8) | secondPartOfSig[1])== 0x0102)
			{	
				printf("central directory: \n");
				struct central_directory_t central_directory;
				size_t readed_byte = fread(&central_directory, sizeof(char), sizeof(central_directory), fp); 
				printf("readed count = %ld \n", readed_byte);
				if (readed_byte == sizeof(central_directory))
				{
					unsigned short lenghtOfFile = convert_FileNameLen(central_directory.fileNameLen);
					char filename[lenghtOfFile + 1];
						
					fread(&filename, sizeof(char), lenghtOfFile, fp);
					filename[lenghtOfFile] = '\0';
				       	printf(" ->%s \n",filename);							
				}
			else
				printf("Empty archive..\n");



			else
				printf("second part= %x \n",
					       	( (secondPartOfSig[0] << 8) | secondPartOfSig[1]));

			// read next two byte for check type of zip structure
			unsigned short zipStructType;
			fread(&zipStructType, sizeof(short), 1, fp);
			fscanf(fp, "%uc",);
			printf("zipStructtype =0x%x \n", zipStructType);
		
			if(zipStructType == 0x403)
			{
 
			// move to "file name len" inside structure 	
			const long int filename_len_offset = 21l; 
			fseek(fp, filename_len_offset, SEEK_CUR);			
			unsigned short fileName_len;
			fread(&fileName_len, 2, 1, fp);
				
			// check  LSByte
			if ( (fileName_len & 0xff) == 0) 
				fileName_len = fileName_len >> 8;
				
			//printf("fileName_len = 0x%x \n", fileName_len);
			//if (fileName_len > 0)
			//{
				// move to "filename" field inside to structure
				const long int filename_offset = 3L;
				fseek(fp,filename_offset, SEEK_CUR);				
				char fileName[fileName_len + 1];
				fread(&fileName, 1, fileName_len, fp);	
				fileName[fileName_len] = '\0';
				printf("%s \n", fileName);
			//}
			}
		//	else
				//printf("Empty archive...\n");
#endif
		
		//printf("0x%x \n", c);
		} 
	}
	if(findZip == 0)
		printf("File not contain archive. \n");
	fclose(fp);
	return 0;
}

unsigned short convert_FileNameLen(char fileNameLen[2])
{
	/* file name lenght consist from two byte, [MSB][LSB]
	 * it is [0][1] element in field .fileNameLen respectively
	 * 
	 * func check the lenght of filename lenght variable
	 * and return correct value of filename lenght field
	*/
	return (fileNameLen[1] & 0xff)?
		fileNameLen[0] << 8 | fileNameLen[0]:
		fileNameLen[0];
}



int sigMatch(unsigned char *beginOfSig , unsigned char *endOfSig, unsigned char c)
{
	// init pointer for check
	static unsigned char *ptr_in_sig;
	static int init_ptr = 0; // incorrect value for start init

	if(init_ptr == 0)
	{
		ptr_in_sig = beginOfSig;
		init_ptr = 1;
	}
	//printf("c = 0x%x ptr = 0x%x \n", c, *ptr_in_sig);
	if (c == *ptr_in_sig)
	{
		if(ptr_in_sig == endOfSig)
		{
			init_ptr = 0;
			return 1;
		}
	      	++ptr_in_sig;
	}
	else
		ptr_in_sig = beginOfSig;

	// find full signature, reset to start condition and return true
	return 0;
}

void checkInput(int argc, int arc_num)
{
	if (argc < arc_num)
	{
		printf("Error. Not correct count of argv...\n");
		exit(1);
	}
}
