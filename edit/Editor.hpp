#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Editor
{
public:
	Editor(const char* filename);
	int open(const char* filename);

	// all utility functions
	const char* contents();
	bool save();
	bool type(int line, int pos, const char input);
	bool del(int line, int pos, int size);
	bool newline(int line, int pos);

	inline int lineCount()
	{
		return (int)lines.size();
	}

	inline int lineLength(int line)
	{
		// includes endline
		return (int)lines[line].size();
	}

	// all lines in current file
	vector<vector<char>> lines;
	const char* filename = NULL;
	bool overwriteMode = false;

private:
	void internal_load_from_stream(istream* input);
	void update_lists();
	std::string* text = NULL; // current copy of contents as a string
};