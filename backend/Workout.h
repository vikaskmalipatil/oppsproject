#pragma once
#include <string>

class Workout {
public:
    std::string type;
    int duration;
    int calories;

    Workout(std::string t = "", int d = 0, int c = 0);
    int estimateCalories() const;
};
