#include <iostream>
#include <string>
#include <iostream>
#include <algorithm> // Needed for transform
#include <fstream>
#include <dirent.h> // Needed for directory search
#include <vector>
#include <map>
#include <unistd.h>

using namespace std;

string path = "/home/skon/books/";
string dumpPath = "/home/students/wilhelmj/indexOffsetDump/";
string delimiters = " ,.;:?'\"()[]-";
int fileCount = 0;
int currentIndexOfFilePathway = 0;

struct indexOffset {
	int index;
	int offset;
};

vector <string> filePathways;
map <string, string> stopWords;
map <string, vector <indexOffset> > tempMap;
map <char, string> afMap;
map <char, string> glMap;
map <char, string> mrMap;
map <char, string> szMap;


int mapCounter = 0;
const int MAP_QUEUE_SIZE = 5000000;

int txtFileCounter = 0;
const int MAX_FILES_TO_SCAN = 1000;

int wordCounter = 0;
const int WORD_NOTIFICATION_LIMIT = 1000000;
int percentLoaded = 0;

void ProcessDirectory(string directory, string word);
void ProcessEntity(struct dirent* entity, string word); 
void ProcessFile(string filePath);
void writeIndexOffsetToFile();
void storeWordInfoInMap(string word, indexOffset indexOffsetOfCurrentWord);
void displayLines(string word);
string GetNextWord(string& line);
void fillStopWordMap();
bool hasEnding (string const &fullString, string const &ending);

int main() {

	fillStopWordMap();
  	string term = "dummyterm";
	string directory = "";
	ProcessDirectory(directory, term);
	writeIndexOffsetToFile(); //this function needs to be called one last time to get everything left in the map
	
	//outer loop actually loops them through the process of displayLines
	while (1) {
		//inner loop makes sure they're not searching for a stopWord word
		while (1) {
		cout << "Enter a term to search for!" << endl;
		 cin >> term;
		 // Normalize string to lower case
		 transform(term.begin(), term.end(), term.begin(), ::tolower);
		 if (stopWords.count(term) == 0) {
			break;
			}
		}
		displayLines(term);
	}
	return 0;
}

void ProcessDirectory(string directory, string word)
{
  string dirToOpen = path + directory;
  DIR *dir;
  dir = opendir(dirToOpen.c_str());

  //set the new path for the content of the directory
  path = dirToOpen + "/";

  //  cout << "Process directory: " << dirToOpen.c_str() << endl;

  if(NULL == dir)
    {
      cout << "could not open directory: " << dirToOpen.c_str() <<\
	endl;
      return;
    }
  struct dirent *entity;
  entity = readdir(dir);
  //string nameOfDirectory = entity->d_name;
  //cout << nameOfDirectory << endl;

  while(entity != NULL)
    {
      ProcessEntity(entity,word);
      entity = readdir(dir);
    }

  //we finished with the directory so remove it from the path
  path.resize(path.length() - 1 - directory.length());
  closedir(dir);
}

void ProcessEntity(struct dirent* entity, string word)
{
  //find entity type
  if(entity->d_type == DT_DIR)
    {//it's an directory
     //don't process the  '..' and the '.' directories
      if(entity->d_name[0] == '.')
	{
	  return;
	}

      //it's an directory so process it
      ProcessDirectory(string(entity->d_name),word);
      return;
    }

  if(entity->d_type == DT_REG)
    {
      //if we're here, we found a text file
      //we want to add the filepath of the text file to the vector.
      string filePath = path + entity->d_name;
      filePathways.push_back(filePath);
      
      //now, we want to take this file and add every single word it contains to the map
      //when we're adding stuff to the map, we need to have the current index of the filePathway
      //starts at 0, and increases by one after we're scanned through that file.
      ProcessFile(filePath);
      txtFileCounter++;
      currentIndexOfFilePathway++;
      
      if (txtFileCounter >= MAX_FILES_TO_SCAN) {
      	//get the heck outta here	
	  }
	  
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

void ProcessFile(string filePath) {

	ifstream infile;
	const char* fn = filePath.c_str();
	//cout << filePath;
	cout << fn << endl;
	int cantOpen = access(fn, R_OK);
	//cout << check << endl;
	if (cantOpen) {
		return;
	}
	
	infile.open(filePath.c_str());
	string line, word;
	if (!hasEnding(filePath, ".txt")) {
		return;
	}
	while (!infile.eof()) {
		//as long as we're still in the file, get the position of the line we're on and the line
		int pos = infile.tellg();
		getline(infile, line);
		//cout << "The line we're looking at right now is: " << line << endl;
		
		while (line.length() > 0) {
		//as long as the line length isn't 0, keep getting the next word 
				
			indexOffset indexOffsetOfCurrentWord;
			indexOffsetOfCurrentWord.index = currentIndexOfFilePathway;
			indexOffsetOfCurrentWord.offset = pos;
			//store the current file's index and the position in the file
			
			word = GetNextWord(line);
			//store the word... this also removes that word from the line itself, so eventually
			//we'll exit the loop when no words are left 

			
			//here is the if condition where you'll also want to be checking for stopwords
			//as it stands, checks only for words greater than 2 characters in length
			//and filters out words that don't contain alpha characters.
			
			
			//when you're adding words to the file, you should instead add the appropriate information to a map
			//that IS STORED IN MEMORY
			//and after ~25 million words, you add all the words + their information stored in the map to a file
			//clear out the map, fill it up to ~25 mil and write to the files again.
			
			size_t found = word.find_first_not_of("abcdefghijklmnopqrstuvwxyz ");
			if (word.length() > 2 && found == string::npos && stopWords.count(word) == 0) {
				//if we've queued up requisite size of map in the memory, let's dump it all into files!!
				if (mapCounter >= MAP_QUEUE_SIZE) {
					writeIndexOffsetToFile();
					mapCounter = 0;
					tempMap.clear();
				} else {
				//otherwise, keep storing word info in the map.
				//cout << "Wrote this word to file: " << word << endl;
				storeWordInfoInMap(word, indexOffsetOfCurrentWord);
				}
			}
		}
	}
	return;
}

void storeWordInfoInMap(string word, indexOffset indexOffsetOfCurrentWord) {
	tempMap[word].push_back(indexOffsetOfCurrentWord);
	mapCounter++;
	return;
}

void writeIndexOffsetToFile() {

	map<string, vector<indexOffset> >::iterator it;
	for (it = tempMap.begin(); it != tempMap.end(); it++ )
	{
		string word = it->first;
		//now that we've found the word, open the file associated with that word.
		string fullPath = dumpPath + word + "Index.bin";
		ofstream outputFile;
		outputFile.open(fullPath.c_str(), ios::app | ios::binary);
		//cout << "Adding word: " << word << endl;
		vector <indexOffset> valueVector = it->second;
		for (int i = 0; i < valueVector.size(); i++) {
			indexOffset tempIndexOffset = valueVector[i];
			outputFile.write((char*)&tempIndexOffset, sizeof(indexOffset));
			wordCounter++;
			if (wordCounter >= WORD_NOTIFICATION_LIMIT) {
				cout << "1,000,000 nonunique words added." << endl;
				percentLoaded++;
				cout << percentLoaded << "/1000 loaded." << endl;
				wordCounter = 0;
			}
			//int tempIndex = tempIndexOffset.index;
			//int tempOffset = tempIndexOffset.offset;
			//cout << "Adding index and offset: " << tempIndex << " and " << tempOffset << endl;
		}
		outputFile.close();
	}
	return;
}

void displayLines(string word) {
	
	indexOffset value;
	int indexOfCurrentWord;
	int offsetOfCurrentWord;
	int indexOfLastWord;
	int offsetOfLastWord;
	string fullPath = dumpPath + word + "Index.bin";
	ifstream infoFile;
	
	//if the file exists, open that file.
	//if it doesn't exist, then that word wasn't found in the whole project because a file
	//was not created for that word.
	if (ifstream(fullPath.c_str())) {
		infoFile.open(fullPath.c_str(), ios::in | ios::binary);
	} else {
		cout << "That word was not found in all of project gutenberg! Wow!" << endl;
		return;
	}
	
	while (!infoFile.eof()) {
		infoFile.read((char*)&value,sizeof(indexOffset));
		indexOfCurrentWord = value.index;
		offsetOfCurrentWord = value.offset;
		
		if (offsetOfCurrentWord != offsetOfLastWord) {
			string fileToOpen = filePathways[indexOfCurrentWord];
			//should be the file pathway you want
		
			ifstream gutenbergFile;
			gutenbergFile.open(fileToOpen.c_str(), ios::in); //open the file
			gutenbergFile.seekg(offsetOfCurrentWord, gutenbergFile.beg); //seekg to the offset of the line
			string line;
			getline(gutenbergFile, line); //get that line
			gutenbergFile.close();
			cout << line << endl;
		}
		offsetOfLastWord = offsetOfCurrentWord;
		indexOfLastWord = indexOfCurrentWord;
	}
	infoFile.close();
	return;
}

string GetNextWord(string& line) {
	string next;
	size_t start = line.find_first_not_of(delimiters);
  if (start != string::npos) {
    //cout << ":" << start;
    size_t end = line.find_first_of(delimiters,start);
    if (end != string::npos) {
      //cout << "#" << end;
      // word with delimiters on both sides
      next = line.substr(start,end-start);
      line.erase(0,end+1);
    } else {
      // word with delimiter only at start
      next = line.substr(start);
      line = "";
    }
  } else {
    // no delimiters found at all
    next = line;
    //cout << "The line we're looking at right now is: " << line << endl;
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

	string meaninglessWord = "butts";

	ifstream stopWordFile;
	stopWordFile.open("listOfStopWords.txt");
	string stopWord;
	while (!stopWordFile.eof()) {
		getline(stopWordFile, stopWord);
		stopWords.insert(map<string, string>::value_type(stopWord, meaninglessWord));
	}
	stopWordFile.close();
	
	//this program also fills maps of the letters of the alphabet now
	
	afMap.insert(map<char, string>::valuetype_('a', meaninglessWord));
	afMap.insert(map<char, string>::valuetype_('b', meaninglessWord));
	afMap.insert(map<char, string>::valuetype_('c', meaninglessWord));
	afMap.insert(map<char, string>::valuetype_('d', meaninglessWord));
	afMap.insert(map<char, string>::valuetype_('e', meaninglessWord));
	afMap.insert(map<char, string>::valuetype_('f', meaninglessWord));
	
	glMap.insert(map<char, string>::valuetype_('g', meaninglessWord));
	glMap.insert(map<char, string>::valuetype_('h', meaninglessWord));
	glMap.insert(map<char, string>::valuetype_('i', meaninglessWord));
	glMap.insert(map<char, string>::valuetype_('j', meaninglessWord));
	glMap.insert(map<char, string>::valuetype_('k', meaninglessWord));
	glMap.insert(map<char, string>::valuetype_('l', meaninglessWord));
	
	mrMap.insert(map<char, string>::valuetype_('m', meaninglessWord));
	mrMap.insert(map<char, string>::valuetype_('n', meaninglessWord));
	mrMap.insert(map<char, string>::valuetype_('o', meaninglessWord));
	mrMap.insert(map<char, string>::valuetype_('p', meaninglessWord));
	mrMap.insert(map<char, string>::valuetype_('q', meaninglessWord));
	mrMap.insert(map<char, string>::valuetype_('r', meaninglessWord));

	szMap.insert(map<char, string>::valuetype_('s', meaninglessWord));
	szMap.insert(map<char, string>::valuetype_('t', meaninglessWord));
	szMap.insert(map<char, string>::valuetype_('u', meaninglessWord));
	szMap.insert(map<char, string>::valuetype_('v', meaninglessWord));
	szMap.insert(map<char, string>::valuetype_('w', meaninglessWord));
	szMap.insert(map<char, string>::valuetype_('x', meaninglessWord));
	szMap.insert(map<char, string>::valuetype_('y', meaninglessWord));
	szMap.insert(map<char, string>::valuetype_('z', meaninglessWord));
	
	return;
}