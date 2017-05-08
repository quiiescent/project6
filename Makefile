#MakeFile to build and deploy the tictactoe server and testclient               
# For CSC3004 Software Development                                              

# Put your user name below:                                                     
USER= wilhelmj

CC= g++

#For Optimization                                                               
#CFLAGS= -O2                                                                    
#For debugging                                                                  
CFLAGS= -g

RM= /bin/rm -f

all: commandlinetest openFileTest mapTest vectorTest displayLines retrieveFileInfo testclient gutenbergcgi PutCGI

commandlinetest: commandlinetest.o
	$(CC) commandlinetest.o -o x
	rm -r ~/indexOffsetDump
	mkdir ~/indexOffsetDump	

testclient: testclient.o fifo.o
	$(CC) testclient.o fifo.o -o testclient

displayLines: displayLines.o fifo.o
	$(CC) displayLines.o fifo.o -o displayLines

retrieveFileInfo: retrieveFileInfo.o
	$(CC) retrieveFileInfo.o -o retrieveFileInfo

vectorTest: vectorTest.o
	$(CC) vectorTest.o -o vectorTest

openFileTest: openFileTest.o
	$(CC) openFileTest.o -o openFileTest

mapTest: mapTest.o
	$(CC) mapTest.o -o mapTest

gutenbergcgi: gutenbergcgi.o  fifo.h
	$(CC) gutenbergcgi.o  fifo.o -o gutenbergcgi -L/usr/local/lib -lcgicc

fifo.o:		fifo.cpp fifo.h
		g++ -c fifo.cpp

gutenbergcgi.o: gutenbergcgi.cpp fifo.h
	$(CC) -c $(CFLAGS) gutenbergcgi.cpp

testclient.o: testclient.cpp fifo.h
	$(CC) -c $(CFLAGS) testclient.cpp

retrieveFileInfo.o: retrieveFileInfo.cpp
	$(CC) -c $(CFLAGS) retrieveFileInfo.cpp


displayLines.o: displayLines.cpp fifo.h
	$(CC) -c $(CFLAGS) displayLines.cpp

vectorTest.o: vectorTest.cpp
	$(CC) -c $(CFLAGS) vectorTest.cpp

mapTest.o: mapTest.cpp
	$(CC) -c $(CFLAGS) mapTest.cpp

commandlinetest.o: commandlinetest.cpp
	$(CC) -c $(CFLAGS) commandlinetest.cpp

openFileTest.o: openFileTest.cpp
	$(CC) -c $(CFLAGS) openFileTest.cpp

PutCGI: gutenbergcgi
	chmod 757 gutenbergcgi
	cp gutenbergcgi /usr/lib/cgi-bin/$(USER)_gutenbergcgi.cgi

	echo "Current contents of your cgi-bin directory: "
	ls -l /usr/lib/cgi-bin/
