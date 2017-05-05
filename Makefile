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

all: commandlinetest openFileTest mapTest

commandlinetest: commandlinetest.o
	$(CC) commandlinetest.o -o x

openFileTest: openFileTest.o
	$(CC) openFileTest.o -o openFileTest

mapTest: mapTest.o
	$(CC) mapTest.o -o mapTest

mapTest.o: mapTest.cpp
	$(CC) -c $(CFLAGS) mapTest.cpp

commandlinetest.o: commandlinetest.cpp
	$(CC) -c $(CFLAGS) commandlinetest.cpp

openFileTest.o: openFileTest.cpp
	$(CC) -c $(CFLAGS) openFileTest.cpp
