##download files from github
#sudo pacman -S git

CC = g++

CFLAGS = -L/usr/lib -lssl -lcrypto -lm -lpthread

EXECUTABLES = cbc-enc cbc-dec ctr-enc ctr-dec

all: $(EXECUTABLES)

$(EXECUTABLES): %: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

#make clean will rid your directory of the executable,
#object files, and any core dumps you've caused
clean:
	rm core $(EXECUTABLES) *.o


