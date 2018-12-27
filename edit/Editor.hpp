#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class Editor
{
  public:
  Editor(const char* filename);
  int open(const char* filename);
  const char* contents();

  // all lines in current file
  vector<vector<char>> lines;

  // current copy of contents as a string
  std::string* text = NULL;
};