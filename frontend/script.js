const API = "http://localhost:8080/api";

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


async function getStats() {
    let name = document.getElementById("statsName").value;

    if (!name) {
        document.getElementById("statsBox").innerHTML = `<p class="error">Please enter a username.</p>`;
        return;
    }

    let res = await fetch(API + "/stats?name=" + name);
    let json = await res.json();

    let box = document.getElementById("statsBox");

    if (json.error) {
        box.innerHTML = `<p class="error">${json.error}</p>`;
        return;
    }

    // Display workouts
    let workoutsHTML = "";
    json.workouts.forEach((w, i) => {
        workoutsHTML += `
            <div class="workout-card">
                <p><strong>${i + 1}. ${w.type}</strong></p>
                <p>Duration: ${w.duration} min</p>
                <p>Calories: ${w.calories}</p>
            </div>
            <hr>
        `;
    });

    box.innerHTML = `
        <h3>${json.name}</h3>
        <p><strong>BMI:</strong> ${json.bmi.toFixed(2)}</p>
        <p><strong>Category:</strong> ${json.category}</p>
        <p><strong>Total Workouts:</strong> ${json.totalWorkouts}</p>

        <h3>Workouts</h3>
        ${workoutsHTML}
    `;
}
