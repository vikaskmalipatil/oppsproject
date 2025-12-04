#include "Tracker.h"

void Tracker::addUser(const Person& p) {
    users.push_back(p);
}

Person* Tracker::getUser(const std::string& name) {
    for (auto& u : users) {
        if (u.name == name)
            return &u;
    }
    return nullptr;
}
