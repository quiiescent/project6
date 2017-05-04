#include <iostream>
#include <string>
#include <iostream>
#include <algorithm> // Needed for transform
#include <fstream>
#include <dirent.h> // Needed for directory search
#include <vector>
#include <map>

using namespace std;

int main() {
	string indexOffsetDumpPath = "/home/students/wilhelmj/indexOffsetDump/newWord.txt";
	ofstream outputFile;
	outputFile.open(indexOffsetDumpPath.c_str(), ios::app);
	
	return 0;
}