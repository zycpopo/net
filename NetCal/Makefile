.PHONY:all
all:  Tcpserver

Tcpserver:Main.cc
	g++ -o $@ $^ -std=c++17 -lpthread

.PHONY:clean
clean:
	rm -f Tcpserver