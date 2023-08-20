#pragma once
#include <string>
#include "../libs/robin-map/include/tsl/robin_set.h"

class SpellCheck {
public:
    SpellCheck();
    void loadDictionary(const std::string& filename);
    bool checkWord(const std::string& word) const;
    tsl::robin_set<std::string> dictionary;
};