#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
//#include <sys/param.h>
//#include <sys/param.h>

#include <sys/mman.h>
#include <string.h>
#include <stdint.h>

static size_t get_file_size( int fd);
static int get_file_descriptor(const char *path);
uint32_t crc32(const void *buf, size_t size);
uint32_t combine_crc32(uint32_t crc1, const uint8_t *crc2, size_t crc2_size);

int main(int argc, char *argv[])
{
	// check count of argument
	if ( argc != 2 )
	{
		printf("Incorrect count of argument\n");
		exit(1);
	}
	
	// take file descriptor
	int fd = get_file_descriptor( argv[1] );

	//get the size of file
	size_t file_size = get_file_size(fd);
//	printf("file_size = %lu\n", file_size);

	// page count 
	//printf("sysconf pagesize = %lu", sysconf(_SC_PAGE_SIZE));
	// set page size 
	size_t page_size = sysconf(_SC_PAGE_SIZE);
	size_t page_count = ( file_size % page_size  == 0) ?
		file_size / page_size :
		(file_size / page_size) + 1;
//	printf("page count = %lu\n", page_count);
//==========================================================
	// for first read calc size and calc 
	size_t read_page = 0;
	uint32_t crc;	

	// test info
	unsigned char buf[11]; buf[11] = 0;
#if 0	// print first 10 symbol
	// read first part
	unsigned char *mm = mmap( 0 , page_size, PROT_READ, MAP_PRIVATE, fd, 0);
	uint32_t crc;	
	unsigned char buf[11]; buf[11] = 0;
	memcpy(&buf, mm, 10);
	crc = crc32(&buf, 10);
	printf("10 symb from mm first part = %s, crc = %x\n", buf, crc);
	read_page += 1;
#endif
//==========================================================
	while (file_size > ( read_page * page_size ) )
	{
		size_t lenght = file_size - ( read_page * page_size ); 
		if ( lenght > page_size )
			lenght = page_size;

		unsigned char *mm = mmap( 0 , lenght, PROT_READ, MAP_PRIVATE, fd, read_page * page_size);
		memcpy(&buf, mm, 10);
		if (read_page == 0)
			crc = crc32(&buf, 10);
		else
			crc = combine_crc32(crc,(const uint8_t*)&buf, 10);
		read_page += 1;

		// test info
		printf("10 symb from mm = %s, crc = %x\n", buf, crc);
	}
	
	// read the first page of file
	unsigned char full_msg[] = "000000000011111111112222222222";
	 printf("full crc = %x\n", crc32(&full_msg, 30));
	// try to mem, first read for calc crc 
#if 0
	unsigned char *mm = mmap( 0 , 10UL, PROT_READ, MAP_PRIVATE, fd, 0);
	off_t off = page_size;

	while(read_page < page_count)
	{
		mm = mmap( 0 , 10UL, PROT_READ, MAP_PRIVATE, fd, off);
		read_page += 1;
		off += page_size;
	
		unsigned char buf[11];
		memcpy(&buf, mm , 10);
		buf[11] = 0;

		printf("read char =%s\n", buf);
	}
#endif	
	close(fd);

	return 0;
}

static int get_file_descriptor(const char *path)
{
	int fd = open( path, O_RDONLY );
	if (fd == -1)
	{
		printf("error to open\n");
		exit(1);
	}
	return fd;
}

static size_t get_file_size( int fd)
{
	struct stat sb;

	if (fstat( fd, &sb ) == -1)
	{
		printf("error to get file size\n");
		exit(1);
	}

	return sb.st_size;
}
