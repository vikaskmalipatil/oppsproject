#pragma once
#include <string>

class Workout {
public:
    std::string type;
    int duration;  // minutes
    int calories;

    Workout();
    Workout(const std::string& t, int d, int c);

    int estimateCalories() const;
};
