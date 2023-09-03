CC = g++ -std=c++14
CFLAGS = -g 

all: SearchNewBooks

SearchNewBooks: SearchNewBooks.cc
	g++ -Wall -Werror -o SearchNewBooks SearchNewBooks.cc

clean:
	rm -f SearchNewBooks