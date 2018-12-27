#include "Editor.hpp"

Editor::Editor(const char* filename)
{
  this->filename = filename;

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

  // TODO: check and prompt for hex files

	internal_load_from_stream(&input);

  input.close();
	
  // file loaded successfully
	return 0;
}

void Editor::internal_load_from_stream(istream* input)
{
  // clear any existing lines structure
  lines.clear();
  
  // current line being read
  string line;

  while (getline(*input, line))
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
}

void Editor::update_lists()
{
  // TODO: if we're going to update vectors like this every time, we
  // might as well not bother using vectors at all...
  stringstream s;
  s << contents();
  internal_load_from_stream(&s);
}

bool Editor::save()
{
  ofstream output;
  output.open(filename);

  for (auto& line : lines)
    for (auto& letter : line)
      output << letter;
  
  output.close();
  return true;
}

bool Editor::del(int line, int pos, int size)
{
  // TODO: take in a Selection instead and work accordingly

  auto target = lines[line].begin() + pos;

  if (size == 1)
    lines[line].erase(target);
  else
    lines[line].erase(target, target + size);

  update_lists();

  return true;
}