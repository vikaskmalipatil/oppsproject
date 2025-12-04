#include <iostream>
#include <string>
#include "SimpleServer.h"
#include "Person.h"
#include "Workout.h"
#include "Tracker.h"
#include "Storage.h"
#include "json.hpp"

using json = nlohmann::json;

Tracker tracker;

// ------------- HTTP RESPONSE HELPER -------------
static std::string httpResponse(
    const std::string& body,
    const std::string& status = "200 OK",
    const std::string& contentType = "application/json")
{
    return
        "HTTP/1.1 " + status + "\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization, X-Requested-With\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" +
        body;
}

// ------------- SMALL HELPERS -------------
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

// ------------- MAIN HANDLER -------------
static std::string handleRequest(std::string req, std::string) {

    // CORS preflight
    if (req.rfind("OPTIONS", 0) == 0) {
        return
            "HTTP/1.1 204 No Content\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type, Authorization, X-Requested-With\r\n"
            "Access-Control-Max-Age: 86400\r\n"
            "\r\n";
    }

    std::string line = getRequestLine(req);
    if (line.empty()) {
        return httpResponse("{\"error\":\"bad request\"}", "400 Bad Request");
    }

    std::string method, path;
    {
        size_t sp1 = line.find(' ');
        size_t sp2 = line.find(' ', sp1 + 1);
        method = line.substr(0, sp1);
        path   = line.substr(sp1 + 1, sp2 - sp1 - 1);
    }

    std::string body = getBody(req);

    // --------- POST /api/addUser ---------
    if (method == "POST" && path == "/api/addUser") {
        bool ok = false;
        json j = parseJsonBody(body, ok);
        if (!ok) {
            return httpResponse("{\"error\":\"invalid json\"}", "400 Bad Request");
        }

        std::string name = j.value("name", "");
        int   age        = j.value("age", 0);
        float height     = j.value("height", 0.0f);
        float weight     = j.value("weight", 0.0f);

        if (name.empty() || age <= 0 || height <= 0 || weight <= 0) {
            return httpResponse("{\"error\":\"missing parameters\"}", "400 Bad Request");
        }

        if (tracker.getUser(name)) {
            return httpResponse("{\"error\":\"user already exists\"}", "409 Conflict");
        }

        tracker.addUser(Person{name, age, height, weight});
        Storage::save(tracker);

        return httpResponse("{\"message\":\"User added\"}", "201 Created");
    }

    // --------- POST /api/addWorkout ---------
    if (method == "POST" && path == "/api/addWorkout") {
        bool ok = false;
        json j = parseJsonBody(body, ok);
        if (!ok) {
            return httpResponse("{\"error\":\"invalid json\"}", "400 Bad Request");
        }

        std::string name = j.value("name", "");
        std::string type = j.value("type", "");
        int duration     = j.value("duration", 0);

        if (name.empty() || type.empty() || duration <= 0) {
            return httpResponse("{\"error\":\"missing parameters\"}", "400 Bad Request");
        }

        Person* p = tracker.getUser(name);
        if (!p) {
            return httpResponse("{\"error\":\"user not found\"}", "404 Not Found");
        }

        Workout w(type, duration, 0);
        w.calories = w.estimateCalories();
        p->workouts.push_back(w);

        Storage::save(tracker);

        return httpResponse("{\"message\":\"Workout added\"}", "201 Created");
    }

    // --------- GET /api/stats?name=... ---------
    if (method == "GET" && path.rfind("/api/stats", 0) == 0) {
        std::string name;
        size_t q = path.find("?name=");
        if (q != std::string::npos) {
            name = path.substr(q + 6);
        }

        if (name.empty()) {
            return httpResponse("{\"error\":\"missing name\"}", "400 Bad Request");
        }

        Person* p = tracker.getUser(name);
        if (!p) {
            return httpResponse("{\"error\":\"user not found\"}", "404 Not Found");
        }

        json out;
        out["name"]          = p->name;
        out["bmi"]           = p->calculateBMI();
        out["category"]      = p->getBMICategory();
        out["totalWorkouts"] = (int)p->workouts.size();

        json wArr = json::array();
        for (auto& w : p->workouts) {
            json jw;
            jw["type"]     = w.type;
            jw["duration"] = w.duration;
            jw["calories"] = w.calories;
            wArr.push_back(jw);
        }
        out["workouts"] = wArr;

        return httpResponse(out.dump(), "200 OK");
    }

    // --------- GET /api/recommend?name=... ---------
    if (method == "GET" && path.rfind("/api/recommend", 0) == 0) {
        std::string name;
        size_t q = path.find("?name=");
        if (q != std::string::npos) {
            name = path.substr(q + 6);
        }

        if (name.empty()) {
            return httpResponse("Missing name", "400 Bad Request", "text/plain");
        }

        Person* p = tracker.getUser(name);
        if (!p) {
            return httpResponse("User not found", "404 Not Found", "text/plain");
        }

        std::string msg = p->getRecommendations();
        return httpResponse(msg, "200 OK", "text/plain");
    }

    // Root /api check
    if (method == "GET" && path == "/api") {
        return httpResponse("{\"message\":\"Fitness API running\"}", "200 OK");
    }

    return httpResponse("{\"error\":\"not found\"}", "404 Not Found");
    
}

// ------------- MAIN -------------
int main() {
    Storage::load(tracker);

    SimpleServer server;
    server.start(8080, handleRequest);

    return 0;
}
