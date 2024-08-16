#include <stdio.h>
#include <stdlib.h>

void checkInput(int argc, int arc_num);

int sigMatch(unsigned char *beginOfSig , unsigned char *endOfSig, unsigned char c);

int main(int argc, char* argv[])
{
	checkInput(argc, 2);
	unsigned char zipSig[4] = {0x50,0x4b,0x03,0x04}; // {0xff,0xd9}; {0x36,0x35}; 
	unsigned char c;  
	
	FILE *fp = fopen(argv[1], "rb");
	if (fp == NULL)
	{
		printf("Error. Not open file... \n");
		exit(1);
	}
	int findZip = 0; // sign of ZIP signature finded
	while(!feof(fp))
	{	
		c = getc(fp);
		if (findZip == 0)
		{
			findZip = sigMatch(&zipSig[0], &zipSig[3], c);
#if 1			// when find zip signature 
			if (findZip == 1)
			{
				// move to know file lenght	
				const long int filename_len_offset = 21l; 
				fseek(fp, filename_len_offset, SEEK_CUR);			
				unsigned short fileName_len;
				fread(&fileName_len, 2, 1, fp);
				
				// check  LSByte
				if ( (fileName_len & 0xff) == 0) 
					fileName_len = fileName_len >> 8;
				
				//printf("fileName_len = 0x%x \n", fileName_len);
				
				// move to filename
				const long int filename_offset = 3L;
				fseek(fp,filename_offset, SEEK_CUR);
				char fileName[fileName_len + 1];
				fread(&fileName, 1, fileName_len, fp);	
				fileName[fileName_len] = '\0';
				printf("%s \n", fileName);

				findZip	= 0;
			} 
#endif
		}
#if 0
		// end of file
		else
		printf("0x%x \n", c);
#endif	
	}	
	fclose(fp);
	return 0;
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
		printf("Error. Not correct count of argv.. \n");
		exit(1);
	}
}
