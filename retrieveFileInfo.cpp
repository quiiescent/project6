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

using namespace std;

struct indexOffset {
	int index;
	int offset;
};

int main() {
	
	string pathway = "/home/students/wilhelmj/dump1/dump/a/";
	cout << "Enter word" << endl;
	string word;
	cin >> word;
	string fullPath = pathway + word + "Index.bin";
	
	indexOffset value;
	int currentIndex;
	int currentOffset;
		
	ifstream infoFile;

	//if the file exists, open that file.
	//if it doesn't exist, then that word wasn't found in the whole project because a file
	//was not created for that word.
	cout << fullPath << endl;
	if (ifstream(fullPath.c_str())) {
		infoFile.open(fullPath.c_str(), ios::in | ios::binary);
	} else {
		cout << "That word was not found in all of project gutenberg! Wow!" << endl;
		return 0;
	}
	
	while (!infoFile.eof()) {
		infoFile.read((char*)&value,sizeof(indexOffset));
		currentIndex = value.index;
		currentOffset = value.offset;
		cout << currentIndex << endl;
		cout << currentOffset << endl;
		/*if (offsetOfCurrentWord != offsetOfLastWord) {
			string fileToOpen = filePathways[indexOfCurrentWord];
			//should be the file pathway you want
		
			ifstream gutenbergFile;
			gutenbergFile.open(fileToOpen.c_str(), ios::in); //open the file
			gutenbergFile.seekg(offsetOfCurrentWord, gutenbergFile.beg); //seekg to the offset of the line
			string line;
			getline(gutenbergFile, line); //get that line
			gutenbergFile.close();
			cout << line << endl;
		} */
	}
	return 0;
}