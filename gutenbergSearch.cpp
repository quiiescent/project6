// Search Books Directory Structure
// Simple version, Searchs the hard way
// Jim Skon, Kenyon College, 2017

#include <iostream>
// Stuff for AJAX
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include <string>
#include <iostream>
#include <algorithm> // Needed for transform
#include <fstream>
#include <dirent.h> // Needed for directory search

// Time
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

#define XML_USE_STL

using namespace std;
using namespace cgicc; // Needed for AJAX functions.

void ProcessDirectory(string directory,string word);
void ProcessFile(string file,string word);
void ProcessEntity(struct dirent* entity,string word);
bool hasEnding (string const &fullString, string const &ending);
int stringMatchCount(string file, string word);

string path = "/home/students/wilhelmj/testingGutenberg";
int fileCount = 0;
int matchCount = 0;
int fileMatchCount = 0;
long long wordCount = 0;
string delimiters = " ,.;:?'\"()[]";
timeval start;
timeval current;
int  max_Time;
bool showMatches;

ofstream logFile;

int main() {
  string directory = "";
  
  cout << "Enter a term to search for!" << endl;
  string term;
  cin << term;
  // Normalize string to lower case
  transform(term.begin(), term.end(), term.begin(), ::tolower);
  
  ProcessDirectory(directory,term);

return 0;
}

// Check if a string has a given ending
bool hasEnding (string const &fullString, string const &ending) {
  if (fullString.length() >= ending.length()) {
    return (0 == fullString.compare (fullString.length() - ending.\
				     length(), ending.length(), ending));
  } else {
    return false;
  }
}

// Process a given directory
// This routine in mutually recursive with process entry
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

  while(entity != NULL)
    {
      ProcessEntity(entity,word);
      entity = readdir(dir);
    }

  //we finished with the directory so remove it from the path
  path.resize(path.length() - 1 - directory.length());
  closedir(dir);
}

// Process a given entry in a directory
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
    {//regular file
      //logFile.open("/tmp/skon.log",ios::out | ios::app);
      //logFile << "File: " << path << entity->d_name << endl;
      //logFile.close();  

      ProcessFile(string(entity->d_name), word);
      return;
    }

  //there are some other types
  //read here http://linux.die.net/man/3/readdir
  cout << "Not a file or directory: " << entity->d_name << endl;
}

// Process a given file
// Only look at it if it has a ".txt" extension
// Searches the file fot the given word, counting the matches
void ProcessFile(string file, string word)
{
  string fileType = ".txt";
  if (hasEnding(file,fileType)) {
    fileCount++;
    if (word.length()>0) {
      int matches = stringMatchCount(file,word);
      if (matches > 0) {
	fileMatchCount++;
	matchCount += matches;

	cout << "<h4>Matches: " << matches << "</h4>";
      }
    }
  }
  // Get the current time
  gettimeofday(&current, 0);
  
  int delta = current.tv_sec - start.tv_sec;
  if (delta >= max_Time) {
    throw (delta);
  }
  //if you want to do something with the file add your code here
}

// get the next "word" from line, removing it from line,
// and returning the word
string getNext(string & line) {
  string next;
  //cout << "$" << line.length();
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
    line = "";
  }
  return next;
}

// Open up file "file", and then search the entire file for "word"
// Return the match count
int stringMatchCount(string file, string word) {
  ifstream infile;
  int fileMatches = 0;
  string line,w;

  try{
    string fileWithPath = path+file;
    infile.open(fileWithPath.c_str());
    //    cout << "open:" << fileWithPath << endl;
    // Check if line contains null, if so it's not ACSII, so skip
    getline(infile,line);
    int lines = 0; // Used to limit number of lines per file
    int words; // used to limit number of words per line
    string save; // used to save line under consideration
    while(!infile.eof() && lines++ < 10000){
      // normalize to lower case
      words = 0; 
      int lineMatches = 0;  // matches on this line
      save = line; // save line in case we need to print it
      while (line.length()>0 && words++ < 100) {
	w = getNext(line);
	// check for matches of this word
	if (w.length()>0) {
	  transform(w.begin(), w.end(), w.begin(), ::tolower);
	  wordCount++; // Total word count
	  if(word.compare(w) == 0) {
	    ++fileMatches;
	    // if first file match the show heading
	    if (fileMatches == 1) {
	      cout << "<h3>" << fileWithPath << "</h3>";
	    }
	    ++lineMatches;
	  }
	}
      }
      if (showMatches && lineMatches > 0) {
	cout << "<br />" << save;
      }
      // Get the next line
      getline(infile,line);
    }
    logFile.close();  
    infile.close();
  }catch(ifstream::failure e){
    //cout<<e<<endl;
  }
  return fileMatches;;
}
