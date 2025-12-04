const API = "http://localhost:8080/api";

// ---------------------- ADD USER ----------------------
async function addUser() {
    let data = {
        name: document.getElementById("addName").value,
        age: Number(document.getElementById("addAge").value),
        height: Number(document.getElementById("addHeight").value),
        weight: Number(document.getElementById("addWeight").value)
    };

    let res = await fetch(API + "/addUser", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    });

    let json = await res.json();
    document.getElementById("addUserResult").innerText = json.message || json.error;
}

// ---------------------- ADD WORKOUT ----------------------
async function addWorkout() {
    let data = {
        name: document.getElementById("workoutName").value,
        type: document.getElementById("workoutType").value,
        duration: Number(document.getElementById("workoutDuration").value)
    };

    let res = await fetch(API + "/addWorkout", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    });

    let json = await res.json();
    document.getElementById("addWorkoutResult").innerText = json.message || json.error;
}

// ---------------------- GET STATS ----------------------
async function getStats() {
    let name = document.getElementById("statsName").value;

    let res = await fetch(API + "/stats?name=" + name);
    let json = await res.json();

    if (json.error) {
        document.getElementById("statsBox").innerHTML = json.error;
        return;
    }

    let html = `
        <h3>${json.name}</h3>
        <p><strong>BMI:</strong> ${json.bmi}</p>
        <p><strong>Category:</strong> ${json.category}</p>
        <h3>Workouts</h3>
    `;

    json.workouts.forEach((w, i) => {
        html += `
            <div class="workout-card">
                <p><strong>${i + 1}. ${w.type}</strong></p>
                <p>Duration: ${w.duration} min</p>
                <p>Calories: ${w.calories}</p>
            </div>
            <hr>
        `;
    });

    document.getElementById("statsBox").innerHTML = html;
}

// ---------------------- GET RECOMMENDATIONS ----------------------
function getRecommendations() {
    const name = document.getElementById("r_name").value;

    fetch(`${API}/recommend?name=${name}`)
        .then(r => r.text())
        .then(t => document.getElementById("recOut").innerText = t);
}
