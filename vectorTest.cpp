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

int main() {

	vector<string> filePathways;
	
	filePathways.push_back("/home/students/nonsense/lol");
	filePathways.push_back("/home/students/nonsense/butts");
	filePathways.push_back("/home/students/nonsense/69");
	filePathways.push_back("/home/students/nonsense/420");
	filePathways.push_back("/home/students/nonsense/memes");

	
	ofstream outputFile("/home/students/wilhelmj/gutenbergPathways/pathways.txt");
    ostream_iterator<string> outputIterator(outputFile, "\n");
    copy(filePathways.begin(), filePathways.end(), outputIterator);
	
	outputFile.close();
	
	return 0;
}