##download files from github

CC = g++

CFLAGS = -L/usr/lib -lssl -lcrypto

EXECUTABLES = cbcmac-tag cbcmac-validate cbc-enc cbc-dec

all: $(EXECUTABLES)

$(EXECUTABLES): %: %.cpp
	$(CC) -o $@ $< $(CFLAGS)

#make clean will rid your directory of the executable,
#object files, and any core dumps you've caused
clean:
	rm core $(EXECUTABLES) *.o


