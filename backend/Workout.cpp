#include "Workout.h"

Workout::Workout(std::string t, int d, int c)
    : type(t), duration(d), calories(c) {}

int Workout::estimateCalories() const {
    return duration * 5;
}
