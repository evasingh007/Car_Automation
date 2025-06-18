const tempElem = document.getElementById("temperature");
const humElem = document.getElementById("humidity");
const presElem = document.getElementById("pressure");
const engineElem = document.getElementById("engineStatus");

const tempStatus = document.getElementById("tempStatus");
const humStatus = document.getElementById("humStatus");
const presStatus = document.getElementById("presStatus");
const engineLabel = document.getElementById("engineLabel");

// Chart setup
const ctx = document.getElementById("sensorChart").getContext("2d");
const chart = new Chart(ctx, {
  type: "line",
  data: {
    labels: [],
    datasets: [
      {
        label: "Temperature (°C)",
        data: [],
        borderColor: "red",
        fill: false
      },
      {
        label: "Humidity (%)",
        data: [],
        borderColor: "blue",
        fill: false
      },
      {
        label: "Pressure (hPa)",
        data: [],
        borderColor: "purple",
        fill: false
      }
    ]
  },
  options: {
    responsive: true,
    scales: {
      y: {
        beginAtZero: true
      }
    }
  }
});

// Listen to Firebase for real-time updates
firebase.database().ref("/car-data").on("value", (snapshot) => {
  const data = snapshot.val();
  if (data) {
    const { temperature, humidity, pressure, engine } = data;
    const timeLabel = new Date().toLocaleTimeString();

    // Update UI
    tempElem.textContent = `${temperature}°C`;
    humElem.textContent = `${humidity}%`;
    presElem.textContent = `${pressure} hPa`;

    engineElem.textContent = engine ? "ENABLED" : "DISABLED";
    engineLabel.textContent = engine ? "ENABLED" : "DISABLED";
    engineLabel.className = `status ${engine ? "normal" : "danger"}`;

    tempStatus.className = `status ${temperature > 35 ? "danger" : "normal"}`;
    humStatus.className = `status ${humidity > 70 ? "danger" : "normal"}`;
    presStatus.className = `status ${pressure < 980 ? "danger" : "normal"}`;

    // Update Chart
    chart.data.labels.push(timeLabel);
    chart.data.datasets[0].data.push(temperature);
    chart.data.datasets[1].data.push(humidity);
    chart.data.datasets[2].data.push(pressure);

    // Limit history to 10 data points
    if (chart.data.labels.length > 10) {
      chart.data.labels.shift();
      chart.data.datasets.forEach(ds => ds.data.shift());
    }

    chart.update();
  }
});
