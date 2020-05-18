#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifndef EDITOR_H
#define EDITOR_H

using namespace std;

class Editor
{
public:
	Editor(const char* filename);
	int open(const char* filename);

	// all utility functions
	bool save();
	bool type(int pos, const char input);
	bool del(int pos, int size);
	bool newline(int pos);

	// variables for the current, prev, next lines
	int curLinePos = 0;
	int curLineLength = 0;
	int prevLinePos = 0;
	int nextLineLength = 0;
  
	const char* filename = NULL;
	bool overwriteMode = false;

  	std::string* text = NULL; // entire file in memory

private:
	void internal_load_from_stream(ifstream* input);
};
#endif