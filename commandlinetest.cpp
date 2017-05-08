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

string path = "/home/skon/books/";
string dumpPath = "/home/students/wilhelmj/dump";
string delimiters = " ,.;:?'\"()[]-";

struct indexOffset {
	int index;
	int offset;
};

vector <string> filePathways;
map <string, string> stopWords;
map <string, vector <indexOffset> > tempMap;

int fileCount = 0;
int currentIndexOfFilePathway = 0;
int currentDumpPathway = 1;
int percentFinishedWithDump = 0;

int mapCounter = 0;
const int MAP_QUEUE_SIZE = 100000000;

int wordCounter = 0;
const int WORD_NOTIFICATION_LIMIT = 1000000;
int percentLoaded = 0;

//navigate directories, write information to map and files
void ProcessDirectory(string directory);
void ProcessEntity(struct dirent* entity); 
void ProcessFile(string filePath);
void writeIndexOffsetToFile();
void storeWordInfoInMap(string word, indexOffset indexOffsetOfCurrentWord);
void writePathwaysToFile();

//manage strings
string GetNextWord(string& line);
bool hasEnding (string const &fullString, string const &ending);

//make map of stopwords 
void fillStopWordMap();


int main() {

	fillStopWordMap();
	string directory = "";
	ProcessDirectory(directory);
	writeIndexOffsetToFile(); //this function needs to be called one last time to get everything left in the map
	writePathwaysToFile();
	
	return 0;
}

void writePathwaysToFile() {
	cout << "Now writing pathways to file..." << endl;
	ofstream outputFile("/home/students/wilhelmj/gutenbergPathways/pathways.txt");
    ostream_iterator<string> outputIterator(outputFile, "\n");
    copy(filePathways.begin(), filePathways.end(), outputIterator);
    outputFile.close();
    cout << "Done!" << endl;
    return;
}

void ProcessDirectory(string directory) {
	
	string dirToOpen = path + directory;
	DIR *dir;
	dir = opendir(dirToOpen.c_str());

	//set the new path for the content of the directory
	path = dirToOpen + "/";

	if (NULL == dir) {
		cout << "Could not open directory: " << dirToOpen.c_str() << endl;
		return;
	}
	struct dirent *entity;
	entity = readdir(dir);

	while (entity != NULL) {
		ProcessEntity(entity);
		entity = readdir(dir);
	}
	//If we're done with this directory, remove that directory from the path
	path.resize(path.length() - 1 - directory.length());
	closedir(dir);
}

void ProcessEntity(struct dirent* entity) {

	//find the entity type
	if(entity->d_type == DT_DIR) { 	//it's an directory
	
		if(entity->d_name[0] == '.') { //don't process the  '..' and the '.' directories
			return;
		}

		//it's a directory, so process it
		ProcessDirectory(string(entity->d_name));
		return;
	}

	if(entity->d_type == DT_REG) {
		//if we're here, we found a text file
		string filePath = path + entity->d_name;
		//now, we want to take this file and add every single word it contains to the map
		ProcessFile(filePath);
		return;
    }
    //otherwise...
	cout << "Not a file or directory: " << entity->d_name << endl;
}

void ProcessFile(string filePath) {

	const char* fn = filePath.c_str();
	int cantOpen = access(fn, R_OK);
	if (cantOpen) { //if we don't have access to the file, move on to another one.
		return;
	}
	if (!hasEnding(filePath, ".txt")) { //if it's not a text file, move on. 
		return;
	}
	filePathways.push_back(filePath); //if it's a text file we have access to, add it to the vector!
	ifstream infile;
	infile.open(filePath.c_str());
	string line, word;
	while (!infile.eof()) { 
		int pos = infile.tellg(); //get the position of the line we're on
		getline(infile, line);	  //get the line
		
		while (line.length() > 0) { //as long as the line length isn't 0, keep getting the next word 
				
			indexOffset indexOffsetOfCurrentWord;
			indexOffsetOfCurrentWord.index = currentIndexOfFilePathway; //index is the current index
			indexOffsetOfCurrentWord.offset = pos; //the offset within the file is its current position
			
			word = GetNextWord(line);
			//store the word... this also removes that word from the line itself, so eventually
			//we'll exit this loop when no words are left 
			
			size_t found = word.find_first_not_of("abcdefghijklmnopqrstuvwxyz ");
			//first, check if the word is longer than 2 characters, contains only alpha characters and isn't a stopword
			if (word.length() > 2 && found == string::npos && stopWords.count(word) == 0) {
				//if we've queued up 100 million words, write them to files.
				if (mapCounter >= MAP_QUEUE_SIZE) {
					writeIndexOffsetToFile();
					mapCounter = 0; //set the counter back to 0
					tempMap.clear(); //remove all the words from the map
				} else {
				//otherwise, keep storing word info in the map.
				storeWordInfoInMap(word, indexOffsetOfCurrentWord);
				}
			}
		}
	}
	//after processing the file, we increase the index.
	currentIndexOfFilePathway++;
	return;
}

void storeWordInfoInMap(string word, indexOffset indexOffsetOfCurrentWord) {
	tempMap[word].push_back(indexOffsetOfCurrentWord);
	mapCounter++;
	return;
}

void writeIndexOffsetToFile() {

	map<string, vector<indexOffset> >::iterator it;
	stringstream sstm;
	sstm << dumpPath << currentDumpPathway << "/" << "dump/";
	string currentPathway = sstm.str(); //need this to convert currentDumpPathway (integer) to string
	for (it = tempMap.begin(); it != tempMap.end(); it++) {
		string word = it->first;
		//now that we've found the word, create the full file pathway using that word
		string fullPath = currentPathway + word[0] + "/" + word + "Index.bin";
		ofstream outputFile;
		outputFile.open(fullPath.c_str(), ios::app | ios::binary);
		vector <indexOffset> valueVector = it->second;
		for (int i = 0; i < valueVector.size(); i++) {
			outputFile.write((char*)&valueVector[i], sizeof(indexOffset));
			wordCounter++;
			if (wordCounter >= WORD_NOTIFICATION_LIMIT) { //just notify when 1 million words are added
				cout << ++percentFinishedWithDump << "% finished." << endl; //helps keep track of addition to files
				wordCounter = 0;
			}
		}
		outputFile.close();
	}
	//reset all counters, up the dumpPathWay so the next 100 million go to a new directory
	currentDumpPathway++;
	percentFinishedWithDump = 0; 
	wordCounter = 0;
	return;
}

string GetNextWord(string& line) {
	string next;
	size_t start = line.find_first_not_of(delimiters);
	if (start != string::npos) {
		size_t end = line.find_first_of(delimiters, start);
		if (end != string::npos) {
			next = line.substr(start,end-start);
			line.erase(0, end+1);
		} else {
			next = line.substr(start);
			line = "";
		}
	} else {
		next = line;
		line = "";
	}
	transform(next.begin(), next.end(), next.begin(), ::tolower);
	return next;
}

bool hasEnding (string const &fullString, string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare (fullString.length() - ending.\
		length(), ending.length(), ending));
	} else {
		return false;
	}
}

void fillStopWordMap() {
	//build a map of stopwords for easy comparison
	ifstream stopWordFile;
	stopWordFile.open("listOfStopWords.txt");
	string stopWord;
	while (!stopWordFile.eof()) {
		getline(stopWordFile, stopWord);
		stopWords.insert(map<string, string>::value_type(stopWord, "butts"));
	}
	stopWordFile.close();
	return;
}