#include <iostream>
#include <string>
#include <iostream>
#include <algorithm> // Needed for transform
#include <fstream>
#include <dirent.h> // Needed for directory search
#include <vector>
#include <map>

using namespace std;

struct indexOffset {
	int index;
	int offset;
};

int main() {
	
	indexOffset butts;
	butts.index = 69;
	butts.offset = 420;
	
	vector<indexOffset> buttVector;
	for (int i = 0; i < 3; i++) {
		buttVector.push_back(butts);
	}
	
	map<string, vector<indexOffset> > myMap;
	ifstream stopWordFile;
	stopWordFile.open("listOfStopWords.txt");
	string stopWord;
	
	while (!stopWordFile.eof()) {
		getline(stopWordFile, stopWord);
		myMap.insert(map<string, vector<indexOffset> >::value_type(stopWord, buttVector));
	}
	
	map<string, vector<indexOffset> >::iterator it;

	for ( it = myMap.begin(); it != myMap.end(); it++ )
	{
		string keyWord = it->first;
		cout << keyWord << " ";
		vector <indexOffset> valueVector = it->second;
		for (int i = 0; i < valueVector.size(); i++) {
			indexOffset tempIndexOffset = valueVector[i];
			int tempIndex = tempIndexOffset.index;
			cout << tempIndex << " ";
			int tempOffset = tempIndexOffset.offset;
			cout << tempOffset << " ";
		}
		cout << endl;
	}
	
	if (myMap.find("dsgadgdg") == myMap.end() ) {
  		cout << "Not found";
	} else {
		cout << "Found!";	
	}
	return 0;
}