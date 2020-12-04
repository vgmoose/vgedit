#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifndef EDITOR_H
#define EDITOR_H

using namespace std;

// stores characters that were added/deleted at a position in history
struct HistoryData {
	std::string chars;
	bool isDelete;
	int pos;
};

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
	bool appendHistory(const char* chars, int pos, bool isDelete);

	// variables for the current, prev, next lines
	int curLinePos = 0;
	int curLineLength = 0;
	int prevLinePos = 0;
	int nextLineLength = 0;

	int lineNoPlaces = 0;

	// undo/redo tracking variables
	// historyPos is where we are in the undoHistory vector
	vector<HistoryData> undoHistory;
	int historyPos = -1;
  
	const char* filename = NULL;

	bool overwriteMode = false;

  	std::string* text = NULL; // entire file in memory

private:
	void internal_load_from_stream(ifstream* input);
};
#endif