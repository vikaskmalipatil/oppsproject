#pragma once
#include <vector>
#include <string>
#include "Person.h"

class Tracker {
public:
    std::vector<Person> users;

    void addUser(const Person& p);
    Person* getUser(const std::string& name);
};
