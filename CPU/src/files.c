#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "files.h"

#include "exitingalloc.h"

FILE *exiting_fopen(const char *fname, const char *mod)
{
	assert(fname != NULL);
	assert(mod != NULL);
	
	FILE *retval = fopen(fname, mod);
	
	if (retval == NULL) {
		printf("# ERROR: Failed to open file: %s. Exiting...\n", fname);
		exit(1);
	}

	return retval;
}

size_t file_size(FILE *fp)
{
	assert(fp != NULL);

	size_t retval = 0;
	
#ifdef __unix__
	struct stat st;
	int err = fstat(fileno(fp), &st);
	if (err != 0) {
#endif
		fseek(fp, 0L, SEEK_END);
		retval = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
#ifdef __unix__
	}
	retval = st.st_size;
#endif

	return retval;
}

void write_file(FileData data, const char* filename)
{
	FILE *fp = exiting_fopen(filename, "wb");
	
	size_t writed = fwrite(data.ptr, 1, data.size, fp);
	
	if (writed != data.size) {
		printf("# ERROR: Failed to write file: %s. \
					Exiting...\n", filename);
                    
		fclose(fp);
		exit(2);
	}
	
	fclose(fp);
}

char* read_file_str(const char *filename)
{
	assert(filename);

	FILE *fp = exiting_fopen(filename, "rb");
	size_t size = file_size(fp);

	char* retval = reinterpret_cast<char*>(exiting_malloc(size + 1));

	size_t readed = fread(retval, 1, size, fp);
	if (readed != size) {
		printf("# ERROR: Failed to read file: %s. \
			Exiting...\n", filename);
		
		free(retval);
		retval = 0;
	}
	else retval[size] = '\0';

	fclose(fp);

	return retval;
}

FileData read_file_bin(const char *filename)
{
	assert(filename);
	
	FILE *fp = exiting_fopen(filename, "rb");
	size_t size = file_size(fp);
	
	void* ptr = exiting_malloc(size);
	
	size_t readed = fread(ptr, 1, size, fp);
	
	if (readed != size) {
		printf("# ERROR: Failed to read file: %s. \
			Exiting...\n", filename);
			
		free(ptr);
		ptr = 0;
        size = 0;
	}
	
	return {ptr, size};
}
