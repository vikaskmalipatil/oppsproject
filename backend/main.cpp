#include <iostream>
#include <string>
#include <algorithm>
#include "SimpleServer.h"
#include "Tracker.h"
#include "Storage.h"
#include "Workout.h"
#include "Person.h"
#include "json.hpp" // nlohmann::json

using json = nlohmann::json;
Tracker tracker;

static std::string httpResponse(
    const std::string& body,
    const std::string& status = "200 OK",
    const std::string& contentType = "application/json")
{
    std::string res = 
        "HTTP/1.1 " + status + "\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: *\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" +
        body;

    return res;
}



static std::string getRequestLine(const std::string& req) {
    size_t pos = req.find("\r\n");
    if (pos == std::string::npos) return "";
    return req.substr(0, pos);
}

static std::string getBody(const std::string& req) {
    std::string sep = "\r\n\r\n";
    size_t pos = req.find(sep);
    if (pos == std::string::npos) return "";
    return req.substr(pos + sep.length());
}

static json parseJsonBody(const std::string& body, bool& ok) {
    ok = false;
    if (body.empty()) return json();
    try {
        json j = json::parse(body);
        ok = true;
        return j;
    } catch (...) {
        ok = false;
        return json();
    }
}

static std::string handleRequest(std::string req, std::string portStr) {
    // Handle CORS preflight
// ALWAYS handle CORS preflight
if (req.find("OPTIONS") != std::string::npos) {
    return 
        "HTTP/1.1 200 OK\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: *\r\n"
        "Access-Control-Max-Age: 86400\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
}



    std::string line = getRequestLine(req);
    if (line.empty()) {
        json err = { {"error","bad request"} };
        return httpResponse(err.dump(), "400 Bad Request");
    }

    std::string method, path;
    {
        size_t sp1 = line.find(' ');
        if (sp1 == std::string::npos) {
            json err = { {"error","bad request"} };
            return httpResponse(err.dump(), "400 Bad Request");
        }
        size_t sp2 = line.find(' ', sp1+1);
        if (sp2 == std::string::npos) {
            json err = { {"error","bad request"} };
            return httpResponse(err.dump(), "400 Bad Request");
        }
        method = line.substr(0, sp1);
        path = line.substr(sp1+1, sp2-sp1-1);
    }

    std::string body = getBody(req);

    if (method == "POST" && (path == "/api/addUser" || path == "/addUser")) {
        bool ok = false;
        json j = parseJsonBody(body, ok);
        if (!ok) {
            json err = { {"error","invalid json"} };
            return httpResponse(err.dump(), "400 Bad Request");
        }

        std::string name = j.value("name", "");
        int age = j.value("age", 0);
        double height = j.value("height", 0.0);
        double weight = j.value("weight", 0.0);

        if (name.empty() || age == 0 || height == 0.0 || weight == 0.0) {
            json err = { {"error","missing or invalid parameters"} };
            return httpResponse(err.dump(), "400 Bad Request");
        }

        if (tracker.getUser(name)) {
            json err = { {"error","user already exists"} };
            return httpResponse(err.dump(), "409 Conflict");
        }

        tracker.addUser(Person(name, age, (float)height, (float)weight));
        Storage::save(tracker);
        json out = { {"message","User added"}, {"name", name} };
        return httpResponse(out.dump(), "201 Created");
    }

    if (method == "POST" && (path == "/api/addWorkout" || path == "/addWorkout")) {
        bool ok = false;
        json j = parseJsonBody(body, ok);
        if (!ok) {
            json err = { {"error","invalid json"} };
            return httpResponse(err.dump(), "400 Bad Request");
        }

        std::string name = j.value("name", "");
        std::string type = j.value("type", "");
        int duration = j.value("duration", 0);

        if (name.empty() || type.empty() || duration <= 0) {
            json err = { {"error","missing or invalid parameters"} };
            return httpResponse(err.dump(), "400 Bad Request");
        }

        Person* p = tracker.getUser(name);
        if (!p) {
            json err = { {"error","user not found"} };
            return httpResponse(err.dump(), "404 Not Found");
        }

        Workout w(type, duration, 0);
        w.calories = w.estimateCalories();
        p->workouts.push_back(w);
        Storage::save(tracker);

        json out = { {"message","Workout added"}, {"user", name} };
        return httpResponse(out.dump(), "201 Created");
    }

    if (method == "GET" && (path.rfind("/api/stats", 0) == 0 || path.rfind("/stats",0) == 0)) {
        std::string name;
        size_t qpos = path.find('?');
        if (qpos != std::string::npos) {
            std::string qs = path.substr(qpos+1);
            size_t pos = qs.find("name=");
            if (pos != std::string::npos) {
                name = qs.substr(pos+5);
            }
        } else {
            std::string prefix = "/api/stats/";
            std::string prefix2 = "/stats/";
            if (path.size() > prefix.size() && path.substr(0,prefix.size()) == prefix) {
                name = path.substr(prefix.size());
            } else if (path.size() > prefix2.size() && path.substr(0,prefix2.size()) == prefix2) {
                name = path.substr(prefix2.size());
            }
        }

        if (name.empty()) {
            json err = { {"error","missing name"} };
            return httpResponse(err.dump(), "400 Bad Request");
        }

        Person* p = tracker.getUser(name);
        if (!p) {
            json err = { {"error","user not found"} };
            return httpResponse(err.dump(), "404 Not Found");
        }

        json out;
        out["name"] = p->name;
        out["bmi"] = p->calculateBMI(); 
        out["category"] = p->getBMICategory();
        out["totalWorkouts"] = (int)p->workouts.size();

// Add workout list
json workoutList = json::array();

for (auto& w : p->workouts) {
    json jw;
    jw["type"] = w.type;
    jw["duration"] = w.duration;
    jw["calories"] = w.calories;
    workoutList.push_back(jw);
}

out["workouts"] = workoutList;


        return httpResponse(out.dump(), "200 OK");
    }

    if (method == "GET" && (path == "/" || path == "/api")) {
        json out = { {"message","Fitness Tracker API running"}, {"version","1.0"} };
        return httpResponse(out.dump(), "200 OK");
    }

    json err = { {"error","not found"} };
    return httpResponse(err.dump(), "404 Not Found");
}

int main() {
    Storage::load(tracker);
    SimpleServer srv;
    srv.start(8080, handleRequest);
    return 0;
}
