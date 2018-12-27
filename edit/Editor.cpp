#include "Editor.hpp"

Editor::Editor(const char* filename)
{
  if (filename != NULL)
    this->open(filename);
}

const char* Editor::contents()
{
  // just return full contents of the file
  stringstream s;
  for (auto& line : lines)
    for (auto& letter : line)
      s << letter;

  s << '\0';
  
  if (text != NULL)
    delete text;

  text = new std::string(s.str());
  return text->c_str();;
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

  // TODO: check and prompt for hex files

	while (getline(input, line))
	{
    // vector representing this line
		vector<char> cur;

    // push each letter of current line into the current vector
		for (auto& letter : line)
			cur.push_back(letter);
    
    // add a new line at the end (POSIX-defined lines)
    // https://stackoverflow.com/a/31426524
    cur.push_back('\n');
    
    // add this line to the editor
		lines.push_back(cur);
	}
	
  // file loaded successfully
	return 0;
}
