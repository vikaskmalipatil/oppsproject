#include "Storage.h"
#include <iostream>

std::string Storage::getDatabasePath() {
    // Always use: PROJECT_ROOT/data/data.json
    std::string folder = "../data";

    // Create folder if missing
    std::filesystem::create_directories(folder);

    return folder + "/data.json";
}

void Storage::save(const Tracker& t) {
    json root;
    root["users"] = json::array();

    for (auto& p : t.users) {
        json jp;
        jp["name"] = p.name;
        jp["age"] = p.age;
        jp["height"] = p.height;
        jp["weight"] = p.weight;

        json wArr = json::array();
        for (auto& w : p.workouts) {
            json jw;
            jw["type"] = w.type;
            jw["duration"] = w.duration;
            jw["calories"] = w.calories;
            wArr.push_back(jw);
        }

        jp["workouts"] = wArr;
        root["users"].push_back(jp);
    }

    std::string path = getDatabasePath();
    std::ofstream out(path);

    out << root.dump(4);

    std::cout << "[Storage] Data saved to: " << path << "\n";
}

void Storage::load(Tracker& t) {
    std::string path = getDatabasePath();

    // If file does not exist → create empty JSON
    if (!std::filesystem::exists(path)) {
        std::cout << "[Storage] No database found. Creating new one.\n";

        json empty;
        empty["users"] = json::array();
        std::ofstream out(path);
        out << empty.dump(4);
        return;
    }

    std::ifstream in(path);
    if (!in.good()) {
        std::cout << "[Storage] Failed to read database.\n";
        return;
    }

    json root;
    in >> root;

    t.users.clear();

    for (auto& jp : root["users"]) {
        Person p(
            jp["name"],
            jp["age"],
            jp["height"],
            jp["weight"]
        );

        for (auto& jw : jp["workouts"]) {
            Workout w(
                jw["type"],
                jw["duration"],
                jw["calories"]
            );
            p.workouts.push_back(w);
        }

        t.users.push_back(p);
    }

    std::cout << "[Storage] Loaded data from: " << path << "\n";
}
