#include "Person.h"

Person::Person(std::string n, int a, float h, float w)
    : name(n), age(a), height(h), weight(w) {}

float Person::calculateBMI() const {
    if (height <= 0) return 0.0f;
    return weight / (height * height);
}

float Person::getBMI() const {
    return calculateBMI();
}

std::string Person::getBMICategory() const {
    float bmi = calculateBMI();
    if (bmi < 18.5f) return "Underweight";
    if (bmi < 24.9f) return "Normal";
    if (bmi < 29.9f) return "Overweight";
    return "Obese";
}
