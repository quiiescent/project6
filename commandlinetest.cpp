#include <iostream>
#include <string>
#include <iostream>
#include <algorithm> // Needed for transform
#include <fstream>
#include <dirent.h> // Needed for directory search
#include <vector>
#include <map>

using namespace std;

string path = "/home/students/wilhelmj/testingGutenberg";
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

void ProcessDirectory(string directory, string word);
void ProcessEntity(struct dirent* entity, string word); 
void ProcessFile(string filePath);
void writeIndexOffsetToFile(string word, indexOffset);
void displayLines(string word);
string GetNextWord(string& line);
void fillStopWordMap();

int main() {

	fillStopWordMap();

	string directory = "";
  cout << "Enter a term to search for!" << endl;
  string term;
  cin >> term;
  // Normalize string to lower case
  transform(term.begin(), term.end(), term.begin(), ::tolower);

	ProcessDirectory(directory, term);
	displayLines(term);
	
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
      
      currentIndexOfFilePathway++;
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

void ProcessFile(string filePath) {
	ifstream infile;
	infile.open(filePath.c_str());
	string line, word;
	cout << filePath << endl;
	while (!infile.eof()) {
		//as long as we're still in the file, get the position of the line we're on and the line
		int pos = infile.tellg();
		getline(infile, line);
		cout << "The line we're looking at right now is: " << line << endl;
		
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
			size_t found = word.find_first_not_of("abcdefghijklmnopqrstuvwxyz ");
			if (word.length() > 2 && found == string::npos && stopWords.find("zoroaster") == stopWords.end()) {
				cout << "Wrote this word to file: " << word << endl;
				writeIndexOffsetToFile(word, indexOffsetOfCurrentWord);
			}
		}
	}
	return;
}

void writeIndexOffsetToFile(string word, indexOffset indexOffsetOfCurrentWord) {
	string fullPath = dumpPath + word + "Index.bin";
	ofstream outputFile;
	outputFile.open(fullPath.c_str(), ios::app | ios::binary);
	outputFile.write((char*)&indexOffsetOfCurrentWord, sizeof(indexOffset));
	outputFile.close();
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
	infoFile.open(fullPath.c_str(), ios::in | ios::binary);
	
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