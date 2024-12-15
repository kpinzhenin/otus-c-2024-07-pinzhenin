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
	printf("file descriptor - ok\n");
	//get the size of file
	size_t file_size = get_file_size(fd);
	//printf("file_size = %lu\n", file_size);

	// page count 
	//printf("sysconf pagesize = %lu", sysconf(_SC_PAGE_SIZE));
	// set page size 
	size_t page_size = sysconf(_SC_PAGE_SIZE);
	size_t page_count = ( file_size % page_size  == 0) ?
		file_size / page_size :
		(file_size / page_size) + 1;
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
		size_t length = file_size - ( read_page * page_size ); 
		if ( length > page_size )
			length = page_size;

		unsigned char *mm = mmap( 0 , length, PROT_READ, MAP_PRIVATE, fd, read_page * page_size);
		memcpy(&buf, mm, 10);
		if (read_page == 0)
			//crc = crc32(&buf, 10);
			crc = crc32(mm, length);
		else
			//crc = combine_crc32(crc,(const uint8_t*)&buf, 10);
			crc = combine_crc32(crc,(const uint8_t*)mm, length); // length 
		read_page += 1;
		munmap( mm, length );
		// test info
		/*printf("read %lu, from size %lu, total page read %lu [%lu] with crc %u\n",
			       	length, file_size, read_page, (file_size - (read_page * page_size)), crc); */
	}
	printf("crc32 = %x\n", crc);
/*	
	// read the first page of file
	unsigned char full_msg[] = "000000000011111111112222222222\n";
	 printf("full crc = %x\n", crc32(&full_msg, strlen(full_msg)));
*/
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
