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

int read_args (int argc, char **argv, char **key_file, char **input_file, char **output_file/*, char **iv_file*/);
size_t add_padding(char **text, size_t text_len);
size_t remove_padding(char **text, size_t text_len);
size_t read_file(const char *filename, char **ret);
size_t print_file(const char *filename, const char *str, size_t str_len);
char* encode(const char *key, const char *data, size_t len);
char* decode(const char *key, const char *data, size_t len);

int main(int argc, char **argv)
{
	char *key_file, *key, *in_file, *in, *out_file, *out/*, *iv_file, *iv*/, *buf;
	char m_block[BLOCK_SIZE], c_block[BLOCK_SIZE];
	size_t in_len;

	srand(time(NULL));
	if ( 1 == read_args (argc, argv, &key_file, &in_file, &out_file/*, &iv_file*/) )
		return 0;

	read_file(key_file, &key);
	if (key == NULL) return 0;
	in_len = read_file(in_file, &in);
	if (in == NULL) return 0;
	/*if (iv_file != NULL)
	{
		read_file(iv_file, &iv);
		if (iv == NULL) return 0;
	}
	else
	{
		iv = (char *) malloc( sizeof(char) * (BLOCK_SIZE) );
		if (iv == NULL)
		{
			printf("unable to allocate enough memory.\n");
			return 0;
		}
		for(int i = 0; i < BLOCK_SIZE; i++)
			iv[i] = rand() % 256;
	}*/

	in_len = add_padding(&in, in_len);
	if (in == NULL) return 0;

	out = (char *) malloc( sizeof(char) * (/*in_len +*/ BLOCK_SIZE) );
	if (out == NULL)
	{
		printf("unable to allocate enough memory.\n");
		return 0;
	}

	//encode in to out
	//memcpy(c_block, iv, BLOCK_SIZE);
	//memcpy(out, c_block, BLOCK_SIZE);
	//printf("in len: %d\n", in_len);
	//memcpy(m_block, in + (i * BLOCK_SIZE), BLOCK_SIZE);
	snprintf(m_block, BLOCK_SIZE, "%.*lu", BLOCK_SIZE, in_len); //prepend length
	snprintf(c_block, BLOCK_SIZE, "%.*d", BLOCK_SIZE, 0); //IV = 0
	for (int j = 0; j < BLOCK_SIZE; j++) m_block[j] = m_block[j] ^ c_block[j];
	buf = encode(key, m_block, BLOCK_SIZE);
	if (buf == NULL) return 0;
	memcpy(c_block, buf, BLOCK_SIZE);
	free(buf);
	SHA256((unsigned char*)in, in_len, (unsigned char*)in);
	for (int i = 0; i < 1; i++)
	{
		memcpy(m_block, in + (i * BLOCK_SIZE), BLOCK_SIZE);
		for (int j = 0; j < BLOCK_SIZE; j++) m_block[j] = m_block[j] ^ c_block[j];
		buf = encode(key, m_block, BLOCK_SIZE);
		if (buf == NULL) return 0;
		memcpy(c_block, buf, BLOCK_SIZE);
		free(buf);
		//memcpy(out + ((i+1) * BLOCK_SIZE), c_block, BLOCK_SIZE);
	}
	memcpy(out, c_block, BLOCK_SIZE);

	//printf("final len: %d\n", out_len);
	print_file(out_file, out, /*in_len +*/ BLOCK_SIZE);
	if (key != NULL) free(key);
	if (in != NULL) free(in);
	/*if (iv != NULL) free(iv);*/
	return 0;
}

int read_args (int argc,
			   char **argv,
			   char **key_file,
			   char **input_file,
			   char **output_file/*,
			   char **iv_file*/)
{
	*key_file = NULL;
	*input_file = NULL;
	*output_file = NULL;
	/**iv_file = NULL;*/

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
		else if( strcmp(argv[i], "-m") == 0 )
		{
			if ( (i+1 >= argc) || (argv[i+1][0] == '-') )
			{
				printf("missing argument '-m'\n");
				return 1;
			}
			if (*input_file != NULL)
			{
				printf("duplicate argument '-m'\n");
				return 1;
			}
			i++;
			*input_file = argv[i];
		}
		else if( strcmp(argv[i], "-t") == 0 )
		{
			if ( (i+1 >= argc) || (argv[i+1][0] == '-') )
			{
				printf("missing argument '-t'\n");
				return 1;
			}
			if (*output_file != NULL)
			{
				printf("duplicate argument '-t'\n");
				return 1;
			}
			i++;
			*output_file = argv[i];
		}/*
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
		}*/
	}
	if ( (*key_file == NULL) ||
		 (*input_file == NULL) ||
		 (*output_file == NULL) )
	{
		printf("Options -k, -m, -t must be given.\n");
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
	//printf("pad: %d\n", pad_size);
	return text_len + pad_size;
}

size_t remove_padding(char **text, size_t text_len)
{
	unsigned char pad_size;

	pad_size = (*text)[text_len-1];
	//printf("pad: %d\n", pad_size);
	
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

