#pragma once
#include <fstream>
#include <sstream>
#include "Tracker.h"
#include "Workout.h"
#include "json.hpp"

using json = nlohmann::json;

class Storage {
public:
    static void save(const Tracker& t) {
        json root = json::array();
        for (const auto& u : t.users) {
            json juser;
            juser["name"] = u.name;
            juser["age"] = u.age;
            juser["height"] = u.height;
            juser["weight"] = u.weight;

            juser["workouts"] = json::array();
            for (const auto& w : u.workouts) {
                json jw;
                jw["type"] = w.type;
                jw["duration"] = w.duration;
                jw["calories"] = w.calories;
                juser["workouts"].push_back(jw);
            }
            root.push_back(juser);
        }

        std::ofstream ofs("data.json");
        ofs << root.dump(2);
    }

    static void load(Tracker& t) {
        std::ifstream ifs("data.json");
        if (!ifs.is_open()) return;

        json root;
        try { ifs >> root; } catch (...) { return; }

        if (!root.is_array()) return;

        for (auto& juser : root) {
            try {
                std::string name = juser["name"];
                int age = juser["age"];
                float height = juser["height"];
                float weight = juser["weight"];

                t.addUser(Person(name, age, height, weight));
                Person* p = t.getUser(name);

                if (!p) continue;

                if (juser.find("workouts") != juser.end() &&
                    juser["workouts"].is_array()) {

                    for (auto& jw : juser["workouts"]) {
                        std::string type = jw["type"];
                        int duration = jw["duration"];
                        int calories = jw["calories"];
                        p->workouts.push_back(Workout(type, duration, calories));
                    }
                }
            } catch (...) {
                continue;
            }
        }
    }
};
