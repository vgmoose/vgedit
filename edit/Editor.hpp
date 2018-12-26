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

  // all lines in current file
  vector<vector<char>> lines;
};