#pragma once
#include <string>
#include <vector>
#include "Workout.h"

class Person {
public:
    std::string name;
    int age;
    float height;
    float weight;
    std::vector<Workout> workouts;

    Person(std::string n = "", int a = 0, float h = 0.0f, float w = 0.0f);
    float getBMI() const;
    float calculateBMI() const;
    std::string getBMICategory() const;
};
