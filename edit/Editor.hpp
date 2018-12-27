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

  // all utility functions
  const char* contents();
  bool save();
  bool del(int line, int pos, int size);

  // all lines in current file
  vector<vector<char>> lines;
  const char* filename = NULL;

  private:
  void internal_load_from_stream(istream* input);
  void update_lists();
  std::string* text = NULL; // current copy of contents as a string
};