#include "Editor.hpp"

Editor::Editor(const char* filename)
{
  if (filename != NULL)
    this->open(filename);
}

int Editor::open(const char* filename)
{
  // open input stream for file
	ifstream input(filename);

	if (input.tellg() == -1)
  {
    // quit if file is missing
		cout << "target file does not exist" << endl;
    return -1;
  }

  // current line being read
  string line;

	while (getline(input, line))
	{
    // vector representing this line
		vector<char> cur;

    // push each letter of current line into the current vector
		for (auto& letter : line)
			cur.push_back(letter);
    
    // add this line to the editor
		lines.push_back(cur);
	}

	for (auto& cur_line : lines)
	{
    // print out every letter of current line
		for (auto& letter : cur_line)
			cout << letter;

    // new line
		cout << endl;
	}
	
  // file loaded successfully
	return 0;
}
