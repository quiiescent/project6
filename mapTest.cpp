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
	map<string, string> myMap;
	ifstream stopWordFile;
	stopWordFile.open("listOfStopWords.txt");
	string stopWord;
	while (!stopWordFile.eof()) {
		getline(stopWordFile, stopWord);
		myMap.insert(map<string, string>::value_type(stopWord, "butts"));
	}
	if (myMap.find("zoroaster") == myMap.end() ) {
  		cout << "Not found";
	} else {
		cout << "Found!";	
	}
	return 0;
}