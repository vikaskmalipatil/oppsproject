#include "Workout.h"

Workout::Workout() : type(""), duration(0), calories(0) {}

Workout::Workout(const std::string& t, int d, int c)
    : type(t), duration(d), calories(c) {}

int Workout::estimateCalories() const {
    if (type == "running") return duration * 10;
    if (type == "cycling") return duration * 8;
    if (type == "walking") return duration * 5;
    return duration * 6; // default
}
