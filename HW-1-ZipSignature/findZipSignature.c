#include <stdio.h>
#include <stdlib.h>

#define SECOND_PART_OF_LOCAL_HEADER_SIGNATURE 0x0304
#define SECOND_PART_OF_END_OF_CENTRAL_DIRECTORY 0x0506

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

	int findZip = 0; // flag of complete finding first part of signature (first 2 byte) (0x50/0x4b)
        int zipHeader_find = 0; // flag of complete finding second part of signature (second 2 byte)
	while(!feof(fp))
	{		
		findZip = 0; 		
		unsigned char c = getc(fp);
		unsigned char zipSig[4] = {0x50,0x4b}; //first part(two  byte) of zip signature
						       
		findZip = sigMatch(&zipSig[0], &zipSig[1], c);
		
		// when find zip signature 
		if (findZip == 1)
		{
			unsigned char secondPartOfSig[2];
			fread(&secondPartOfSig, sizeof(char), 2, fp);
			
			if ( ( (secondPartOfSig[0] << 8) | secondPartOfSig[1]) == SECOND_PART_OF_LOCAL_HEADER_SIGNATURE)		
			{
				struct local_header_t local_header;
				if ( fread(&local_header, sizeof(char), sizeof(local_header), fp) == sizeof(local_header)  ) // 26
				{
					unsigned short lenghtOfFile = convert_FileNameLen(local_header.fileNameLen);
					char filename[lenghtOfFile + 1];
						
					fread(&filename, sizeof(char), lenghtOfFile, fp);
					filename[lenghtOfFile] = '\0';
				       	printf("%s \n",filename);	
				
					zipHeader_find = 1;
				}
				else
					printf("Error.Broken archive...\n");

			}

			else if( ( (secondPartOfSig[0] << 8) | secondPartOfSig[1]) == SECOND_PART_OF_END_OF_CENTRAL_DIRECTORY )
			{
				if (zipHeader_find == 0)
				{
					printf("Empty archive. \n");
					zipHeader_find = 1;
				}
			}

		
		} 
	}
	if(zipHeader_find  == 0)
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
