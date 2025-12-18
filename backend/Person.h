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

    Person(std::string n, int a, float h, float w)
        : name(n), age(a), height(h), weight(w) {}

    float calculateBMI() const {
        float hMeters = height / 100.0f;
        return weight / (hMeters * hMeters);
    }

    std::string getBMICategory() const {
        float bmi = calculateBMI();
        if (bmi < 18.5) return "Underweight";
        if (bmi < 25) return "Normal";
        if (bmi < 30) return "Overweight";
        return "Obese";
    }

   
    std::string getRecommendations() const;
};
