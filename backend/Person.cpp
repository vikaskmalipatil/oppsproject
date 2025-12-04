#include "Person.h"
#include "json.hpp"

using json = nlohmann::json;
#include "Person.h"
#include "json.hpp"
using json = nlohmann::json;

std::string Person::getRecommendations() const {
    int totalCalories = 0;
    for (const auto& w : workouts) {
        totalCalories += w.calories;
    }

    float bmi = calculateBMI();
    std::string category = getBMICategory();

    // convert height from cm → meters
    float hMeters = height / 100.0f;

    float targetMin = 18.5f * hMeters * hMeters;
    float targetMax = 24.9f * hMeters * hMeters;

    float weightToGain = 0;
    float weightToLose = 0;

    if (weight < targetMin)
        weightToGain = targetMin - weight;

    if (weight > targetMax)
        weightToLose = weight - targetMax;

    json out;
    out["name"] = name;
    out["bmi"] = bmi;
    out["category"] = category;
    out["totalCaloriesBurned"] = totalCalories;

    // ------------------ UNDERWEIGHT ------------------
    if (category == "Underweight") {
        out["status"] = "Underweight – needs healthy weight gain";
        out["weightToGain"] = round(weightToGain * 10) / 10.0f;

        out["advice"] = {
            "Increase calorie intake",
            "Add protein-rich meals",
            "Eat 5–6 times a day",
            "Do light strength workouts"
        };

        out["foodSuggestions"] = {
            "Bananas", "Eggs", "Peanut Butter",
            "Milkshakes", "Paneer", "Dry fruits"
        };
    }

    // ------------------ NORMAL ------------------
    else if (category == "Normal") {
        out["status"] = "Healthy weight – maintain balance";
        out["weightToGain"] = 0;
        out["weightToLose"] = 0;

        if (totalCalories < 300) {
            out["advice"] = {
                "Add 20–30 minutes walking",
                "Improve daily movement",
                "Maintain balanced diet"
            };
        } else if (totalCalories <= 600) {
            out["advice"] = {
                "Good activity level",
                "Stay hydrated",
                "Continue regular exercise"
            };
        } else {
            out["advice"] = {
                "High activity level",
                "Take proper recovery",
                "Increase electrolytes"
            };
        }

        out["foodSuggestions"] = {
            "Vegetables", "Fruits", "Oats", "Lean protein"
        };
    }

    // ------------------ OVERWEIGHT / OBESE ------------------
    else {
        out["status"] = "High BMI – needs calorie reduction";
        out["weightToLose"] = round(weightToLose * 10) / 10.0f;

        if (totalCalories < 300) {
            out["advice"] = {
                "Increase daily activity",
                "Start light walking",
                "Avoid sugary foods"
            };
        } else {
            out["advice"] = {
                "Continue workouts",
                "Reduce junk food",
                "Avoid fried items",
                "Track your calories"
            };
        }

        out["foodSuggestions"] = {
            "Salads", "Soup", "Oats", "Green tea",
            "Boiled vegetables", "Fruits"
        };
    }

    return out.dump(4);
}
