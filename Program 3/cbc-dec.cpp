/*
 * Jacob Vargo
 * COSC 483 Programming Assignment 1
 * cbc-enc.cpp
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <string>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#define BLOCK_SIZE 128
#define ABORT() (fprintf(stderr, "%s\nAborting in %s at %s:%d\n", ERR_error_string(ERR_get_error(), NULL), __PRETTY_FUNCTION__, __FILE__, __LINE__), abort(), 0)

int read_args (int argc, char **argv, char **key_file, char **input_file, char **output_file, char **iv_file);
size_t add_padding(char **text, size_t text_len);
size_t remove_padding(char **text, size_t text_len);
size_t read_file(const char *filename, char **ret);
size_t print_file(const char *filename, const char *str, size_t str_len);
char* encode(const char *key, const char *data, size_t len);
char* decode(const char *key, const char *data, size_t len);

int main(int argc, char **argv)
{
	char *key_file, *key, *in_file, *in, *out_file, *out, *iv_file, *buf;
	char m_block[BLOCK_SIZE], c_block[BLOCK_SIZE], iv[BLOCK_SIZE];
	size_t in_len, out_len, c_len;

	srand(time(NULL));
	if ( 1 == read_args (argc, argv, &key_file, &in_file, &out_file, &iv_file) )
		return 0;

	read_file(key_file, &key);
	if (key == NULL) return 0;
	in_len = read_file(in_file, &in);
	//printf("in len: %d\n", in_len);
	if (in == NULL) return 0;
	if (iv_file != NULL)
	{
		printf("ignoring iv file. iv is read from cypher text\n"); 
	}
	if (in_len < BLOCK_SIZE)
	{
		printf("cypher text is shorter than the block size.\n");
		return 0;
	}
	for(int i = 0; i < BLOCK_SIZE; i++)
		iv[i] = in[i];

	out = (char *) malloc( sizeof(char) * (in_len + BLOCK_SIZE) );
	if (out == NULL)
	{
		printf("unable to allocate enough memory.\n");
		return 0;
	}

	//decode in to out
	for (int i = (in_len / BLOCK_SIZE); 1 <= i; i--)
	{
		memcpy(m_block, in + (i * BLOCK_SIZE), BLOCK_SIZE);
		buf = decode(key, m_block, BLOCK_SIZE);
		memcpy(c_block, buf, BLOCK_SIZE);
		if (buf != NULL) free(buf);
		memcpy(m_block, in + ((i-1) * BLOCK_SIZE), BLOCK_SIZE);
		for (int j = 0; j < BLOCK_SIZE; j++) m_block[j] = m_block[j] ^ c_block[j];
		memcpy(out + (i * BLOCK_SIZE), m_block, BLOCK_SIZE);
	}
	memcpy(out, in, BLOCK_SIZE); //copy the original iv

	in_len = remove_padding(&out, in_len);
	print_file(out_file, out + BLOCK_SIZE, in_len - BLOCK_SIZE); //shifting out by BLOCK_SIZE removes iv from printed file
	if (key != NULL) free(key);
	if (in != NULL) free(in);
	return 0;
}

int read_args (int argc,
			   char **argv,
			   char **key_file,
			   char **input_file,
			   char **output_file,
			   char **iv_file)
{
	*key_file = NULL;
	*input_file = NULL;
	*output_file = NULL;
	*iv_file = NULL;

	//read arguments
	for (int i = 1; i < argc; i++)
	{
		if ( strcmp(argv[i], "-k") == 0 )
		{
			if ( (argc <= i+1) || (argv[i+1][0] == '-') )
			{
				printf("missing argument '-k'\n");
				return 1;
			}
			if (*key_file != NULL)
			{
				printf("duplicate argument '-k'\n");
				return 1;
			}
			i++;
			*key_file = argv[i];
		}
		else if( strcmp(argv[i], "-i") == 0 )
		{
			if ( (i+1 >= argc) || (argv[i+1][0] == '-') )
			{
				printf("missing argument '-i'\n");
				return 1;
			}
			if (*input_file != NULL)
			{
				printf("duplicate argument '-i'\n");
				return 1;
			}
			i++;
			*input_file = argv[i];
		}
		else if( strcmp(argv[i], "-o") == 0 )
		{
			if ( (i+1 >= argc) || (argv[i+1][0] == '-') )
			{
				printf("missing argument '-o'\n");
				return 1;
			}
			if (*output_file != NULL)
			{
				printf("duplicate argument '-o'\n");
				return 1;
			}
			i++;
			*output_file = argv[i];
		}
		else if( strcmp(argv[i], "-v") == 0 )
		{
			if ( (i+1 >= argc) || (argv[i+1][0] == '-') )
			{
				printf("missing argument '-v'\n");
				return 1;
			}
			if (*iv_file != NULL)
			{
				printf("duplicate argument '-v'\n");
				return 1;
			}
			i++;
			*iv_file = argv[i];
		}
	}
	if ( (*key_file == NULL) ||
		 (*input_file == NULL) ||
		 (*output_file == NULL) )
	{
		printf("Options -k, -i, -o must be given.\n");
		return 1;
	}
	return 0;
}

size_t add_padding(char **text, size_t text_len)
{
	unsigned char pad_size;
	char *ret;
	
	pad_size = BLOCK_SIZE - (text_len % BLOCK_SIZE);
	ret = (char *) malloc( sizeof(char) * (text_len + pad_size +1) );
	if (ret == NULL)
	{
		printf("Unable to allocate enough memory.\n");
		return 0;
	}
	memcpy(ret, *text, text_len);
	for (int i = 0; i < pad_size; i++)
		ret[text_len + i] = (char)pad_size;
	
	ret[text_len + pad_size] = '\0';
	free(*text);
	*text = ret;
	printf("pad: %d\n", pad_size);
	return text_len + pad_size;
}

size_t remove_padding(char **text, size_t text_len)
{
	unsigned char pad_size;

	pad_size = (unsigned char)(*text)[text_len-1];
	//printf("pad: %ld\n", pad_size);
	
	for (int i = 0; i < pad_size; i++)
	{
		(*text)[text_len - i] = '\0';
	}
	return text_len - pad_size;
}

size_t read_file(const char *filename, char **ret)
{
	FILE *file;
	size_t file_size, bytes_read;
	
	*ret = NULL;
	bytes_read = 0;
	file = fopen(filename, "r");
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		fseek(file, 0, SEEK_SET);
		*ret = (char *) malloc( sizeof(char) * file_size +1);
		if (*ret != NULL)
		{
			bytes_read = fread(*ret, sizeof(char), file_size, file);
			(*ret)[file_size] = '\0';
		}
		else printf("Unable to allocate enough memory.\n");
		fclose(file);
	}
	else printf("Unable to open file %s.\n", filename);
	return bytes_read;
}

size_t print_file(const char *filename, const char *str, size_t str_len)
{
	FILE *file;
	size_t bytes_written;
	
	bytes_written = 0;
	if (str == NULL) return 0;
	file = fopen(filename, "w");
	if (file != NULL)
	{
		bytes_written = fwrite(str, sizeof(char), str_len, file); 
		fclose(file);
	}
	else printf("Unable to open file %s.\n", filename);
	return bytes_written;
}

char* encode(const char *key, const char *data, size_t len)
{
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex (ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)key, NULL);
	EVP_CIPHER_CTX_set_padding(ctx, false);
	unsigned char *buffer, *pointer;
	int outlen;
	buffer = (unsigned char *) malloc( sizeof(unsigned char) * (len + BLOCK_SIZE) );
	if (buffer == NULL)
	{
		printf("Unable to allocate enough memory.\n");
		return NULL;
	}
	pointer = buffer;
	EVP_EncryptUpdate (ctx, pointer, &outlen, (const unsigned char*)data, len) or ABORT();
	pointer += outlen;
	EVP_EncryptFinal_ex(ctx, pointer, &outlen) or ABORT();
	pointer += outlen;
	EVP_CIPHER_CTX_free(ctx);
	
	return (char*)buffer;
}

char* decode(const char *key, const char *data, size_t len)
{
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex (ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)key, NULL);
	EVP_CIPHER_CTX_set_padding(ctx, false);
	unsigned char *buffer, *pointer;
	int outlen;
	buffer = (unsigned char *) malloc( sizeof(unsigned char) * 1024 );
	if (buffer == NULL)
	{
		printf("Unable to allocate enough memory.\n");
		return NULL;
	}
	pointer = buffer;
	EVP_DecryptUpdate (ctx, pointer, &outlen, (const unsigned char*)data, len) or ABORT();
	pointer += outlen;
	EVP_DecryptFinal_ex(ctx, pointer, &outlen) or ABORT();
	pointer += outlen;
	EVP_CIPHER_CTX_free(ctx);
	return (char*)buffer;
}

