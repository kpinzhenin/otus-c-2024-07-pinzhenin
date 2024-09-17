#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CP1251 1
#define KOI8_R 2
#define ISO_8859_5 3

#define ENCODING_START_CODE 0xB0 //0xC0
#define ENCODING_OFFSET_FROM_CP1251_TO_UTF8 0x350
#define ENCODING_OFFSET_FROM_ISO8859_5_TO_UTF8 0x360

struct utf8_octet
{
	unsigned char msb; // Most Significant Byte of UTF8 octet
	unsigned char lsb; // Lost Significant Byte of UTF8 octet
};

int choiseDecoding(char *arg2);

unsigned short conv_CP1251_to_UTF8(unsigned char c);
unsigned short conv_KOI8R_to_UTF8(unsigned char c);
unsigned short conv_ISO8859_5_to_UTF8(unsigned char c);

struct utf8_octet makeUTF8_2octet(unsigned short utf_plus);

int main(int argc, char *argv[])
{
	// check input argument
	if (argc != 4)
	{
		printf("Uncorrect count of arg \n");
		exit(1);
	}
	
	// check correct open file
	FILE *fp = fopen(argv[1], "rb");
	if (fp == NULL)
	{
		printf("Error. Open input file \n");
		exit(1);
	}

	// check correct encoding style
	int coding_style = choiseDecoding(argv[2]);
	if (coding_style == 0)
	{
		printf("Error. Uncorrect encoding style \n");
		exit(1);
	}

	// check create out file 
	FILE *fp_w = fopen(argv[3], "wb"); // wb - write byte
	if (fp == NULL)
	{
		printf("Error. Open out file \n");
		exit(1);
	}

	int next_readed_char;
	while( (next_readed_char = getc(fp)) != EOF)
	{
		// cyrillic symbol start from 0xC0 in cp1251 and koi8-r
		// and from 0xb0 in iso8859-5.
		// than choice 0xb0 as start code from rncoding for all
		if (next_readed_char >= ENCODING_START_CODE)	
		{
			// convert to utf8 format
			wchar_t u_plus_utf8; 
			if (coding_style == CP1251)
				u_plus_utf8 = conv_CP1251_to_UTF8(next_readed_char);		
			
			else if (coding_style == KOI8_R)
				u_plus_utf8 = conv_KOI8R_to_UTF8(next_readed_char);
			
			else if ( coding_style == ISO_8859_5)
				u_plus_utf8 = conv_ISO8859_5_to_UTF8(next_readed_char);
			// UTF-8 have specific representation for cirillic alfa,
			// named - octet (special 2-byte)
			// next make octet from utf8+ format
			struct utf8_octet ut8octet = makeUTF8_2octet(u_plus_utf8);

			fwrite(&ut8octet, sizeof(char), sizeof(struct utf8_octet), fp_w);	
								      
		}
		else
			// if getted char low then ENCODING_START_CODE - mean it's 
			// space, comma and etc. symbol. Just trasfer it.
			fwrite(&next_readed_char, sizeof(char), 1, fp_w);
				
	}

	fclose(fp);
	fclose(fp_w);
	return 0;
}
unsigned short conv_CP1251_to_UTF8(unsigned char c)
{
	//cyrillic coding in range 0xC0 - 0xFF;
	return c + ENCODING_OFFSET_FROM_CP1251_TO_UTF8;
}


unsigned short conv_ISO8859_5_to_UTF8(unsigned char c)
{
	//cyrillic coding in range 0xB0 - 0xEF;
	return c + ENCODING_OFFSET_FROM_ISO8859_5_TO_UTF8;
}

unsigned short conv_KOI8R_to_UTF8(unsigned char c)
{
	// choice symbol
	switch(c){

		case 0xc0: return 0x44e;
		case 0xc1: return 0x430;
		case 0xc2: return 0x431;
		case 0xc3: return 0x446;
		case 0xc4: return 0x434;
		case 0xc5: return 0x435;
		case 0xc6: return 0x444;
		case 0xc7: return 0x433;
		case 0xc8: return 0x445;
		case 0xc9: return 0x438;
		case 0xca: return 0x439;
		case 0xcb: return 0x43a;
		case 0xcc: return 0x43b;
		case 0xcd: return 0x43c;
		case 0xce: return 0x43d;
		case 0xcf: return 0x43e;
		case 0xd0: return 0x43f;
		case 0xd1: return 0x44f;
		case 0xd2: return 0x440;
		case 0xd3: return 0x441;
		case 0xd4: return 0x442;
		case 0xd5: return 0x443;
		case 0xd6: return 0x436;
		case 0xd7: return 0x432;
		case 0xd8: return 0x44c;
		case 0xd9: return 0x44b;
		case 0xda: return 0x437;
		case 0xdb: return 0x448;
		case 0xdc: return 0x44d;
		case 0xdd: return 0x449;
		case 0xde: return 0x447;
		case 0xdf: return 0x44a;
		case 0xe0: return 0x42e;
		case 0xe1: return 0x410;
		case 0xe2: return 0x411;
		case 0xe3: return 0x426;
		case 0xe4: return 0x414;
		case 0xe5: return 0x415;
		case 0xe6: return 0x424;
		case 0xe7: return 0x413;
		case 0xe8: return 0x425;
		case 0xe9: return 0x418;
		case 0xea: return 0x419;
		case 0xeb: return 0x41a;
		case 0xec: return 0x41b;
		case 0xed: return 0x41c;
		case 0xee: return 0x41d;
		case 0xef: return 0x41e;
		case 0xf0: return 0x41f;
		case 0xf1: return 0x42f;
		case 0xf2: return 0x420;
		case 0xf3: return 0x421;
		case 0xf4: return 0x422;
		case 0xf5: return 0x423;
		case 0xf6: return 0x416;
		case 0xf7: return 0x412;
		case 0xf8: return 0x42c;
		case 0xf9: return 0x42b;
		case 0xfa: return 0x417;
		case 0xfb: return 0x428;
		case 0xfc: return 0x42d;
		case 0xfd: return 0x429;
		case 0xfe: return 0x427;
		case 0xff: return 0x42a;
		
	}
}

struct utf8_octet makeUTF8_2octet(unsigned short utf_plus)
{
	// make mask for 2 octet 110xxxxx 10xxxxxx
	struct utf8_octet utf8_octet_var;
	// make LSB byte
	utf8_octet_var.lsb = (utf_plus & 0x3f) | 0x80;
	// make MSB byte
	utf8_octet_var.msb  = ( ((utf_plus << 2) & (0x1f00)) >> 8) | 0xc0;
	return utf8_octet_var;
}

void checkInput(FILE *in_fp, FILE *out_fp,  char *conv_type)
{
	if (in_fp == NULL)
		printf("Error open input file \n");
	if (out_fp == NULL)
		printf("Error open output file \n");
	if (strcmp(conv_type,"CP-1251") != 0 &&
			strcmp(conv_type, "koi8-r") != 0 &&
			strcmp(conv_type, "iso8859-5") != 0 )	
		printf("Error '%s' is not support convertion type \n", conv_type);
}


int choiseDecoding(char *arg2)
{
	if (strcmp(arg2, "CP1251") == 0)
		return CP1251;
	else if(strcmp(arg2, "KOI8-R") == 0)	
		return KOI8_R;
	else if(strcmp(arg2, "ISO-8859-5") == 0)	
		return ISO_8859_5;
	else
		return 0;
}

