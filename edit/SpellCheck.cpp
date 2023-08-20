#include "SpellCheck.hpp"
#include "../gui/MainDisplay.hpp"
#include <iostream>
#include <fstream>

using namespace std;

SpellCheck::SpellCheck()
{
    dictionary.reserve(370105);
    loadDictionary(RAMFS "res/words_alpha.txt");
}

void SpellCheck::loadDictionary(const std::string& filename) {
    fstream newfile;
    newfile.open(filename.c_str(), ios::in);
    if (newfile.is_open()){
        std::string tp;
        while(getline(newfile, tp)){
            // insert word without newline
            dictionary.insert(tp.substr(0, tp.length() - 1));
        }
        newfile.close();
    }
}

bool SpellCheck::checkWord(const std::string& word) const {
    return dictionary.count(word) == 1;
}