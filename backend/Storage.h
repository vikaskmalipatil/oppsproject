#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include "Tracker.h"
#include "json.hpp"

using json = nlohmann::json;

class Storage {
public:
    static std::string getDatabasePath();
    static void save(const Tracker& t);
    static void load(Tracker& t);
};
