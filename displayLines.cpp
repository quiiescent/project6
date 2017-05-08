#include <iostream>
#include <string>
#include <iostream>
#include <algorithm> // Needed for transform
#include <fstream>
#include <dirent.h> // Needed for directory search
#include <vector>
#include <map>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iterator>
#include "fifo.h"

string receive_fifo = "Namerequest";
string send_fifo = "Namereply";

using namespace std;

struct indexOffset {
	int index;
	int offset;
};

int main() {

	string vectorPath = "/home/students/wilhelmj/gutenbergPathways/pathways.txt";
	ifstream vectorFile;
	string line;
	vector<string> filePathways;
	vectorFile.open(vectorPath.c_str());
	
	//create the vector of file pathways 
	while (!vectorFile.eof()) {
		getline(vectorFile, line);
		filePathways.push_back(line);
	}
	
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);
	
	string firstPart = "/home/students/wilhelmj/dump";
	
	while (1) {
	
		int dumpNumber = 1;
		int indexOfLastWord = -2;
		int offsetOfLastWord = -2; //comparison in line 80 needs to be false, so we assign -2
		int currentIndex;
		int currentOffset;
		int runs = 0;
		
		ifstream wordFile;
		indexOffset value;
		string bigString;
		
		recfifo.openread();
		string word = recfifo.recv();
		recfifo.fifoclose();
		
		cout << "Received: " << word << endl;
	
		sendfifo.openwrite();
		for (int i = 0; i < 9; i++) {
			stringstream sstm;
			sstm << firstPart << dumpNumber << "/dump/" << word[0] << "/" << word << "Index.bin";
			string fullPath = sstm.str(); //get the full file pathway
			cout << fullPath << endl;
			if (!ifstream(fullPath.c_str())) { //if the file doesn't exist, well...move on to the next one
				cout << "That word wasn't found in this directory." << endl;
				cout << "We'll check the next one..." << endl;
			} else { //the file exists, so open it
				wordFile.open(fullPath.c_str(), ios::in | ios::binary);
				while (!wordFile.eof()) { //within the file, read through all indexOffsets
					wordFile.read((char*)&value,sizeof(indexOffset));
					currentIndex = value.index;
					currentOffset = value.offset;
					cout << currentIndex << endl;
					cout << currentOffset << endl;
					if (currentOffset != offsetOfLastWord) { //we don't want to output a line we already sent
															 //so if the offsets are the same, skip that output
						string fileToOpen = filePathways[currentIndex]; //
						ifstream gutenbergFile;
						gutenbergFile.open(fileToOpen.c_str(), ios::in); //open the file in project gutenberg
						gutenbergFile.seekg(currentOffset, gutenbergFile.beg); //seekg to the offset of the line
						string line;
						getline(gutenbergFile, line); //get that line
						gutenbergFile.close();
						sendfifo.send(line);
						runs++;
					}
					if (runs > 10) { //at 10 lines, just break out of the while loop and check the next 100 million.
						runs = 0;
						break;
					}
					offsetOfLastWord = currentOffset;
					indexOfLastWord = currentIndex; 
				}
			}
			wordFile.close();
			dumpNumber++;
		}
		sendfifo.send("$END");
		sendfifo.fifoclose();
	}
	return 0;
}