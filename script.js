// MQTT client setup using Paho MQTT library
const client = new Paho.MQTT.Client(
    "a5bc5fc65f7f4fc1b391cff98bfdcff2.s1.eu.hivemq.cloud",
    8884,
    "web-client" + Math.random().toString(16).substr(2, 8)
  );
  
  // MQTT connection options
  const options = {
    useSSL: true,
    userName: "hivemq.webclient.1741594079161",
    password: "2#cZgCmXY>kJ7rd10&@E",
    onSuccess: onConnect,
    onFailure: function (err) {
      console.error("Connection failed:", err.errorMessage);
    },
  };
  
  // Connect to the MQTT broker
  client.connect(options);
  
  client.onMessageArrived = function (message) {
    const [temperature, pressure, light] = message.payloadString.split(",");
    document.getElementById("temperature").textContent = temperature;
    document.getElementById("pressure").textContent = pressure;
    document.getElementById("light").textContent = light;
  };
  
  // Called when the client connects
  function onConnect() {
    console.log("Connected to MQTT broker!");
    client.subscribe("home/sensors/data");
  }
  
  // Publish control messages to the broker
  function controlDevice(device, state) {
    const message = new Paho.MQTT.Message(`${device},${state}`);
    message.destinationName = "esp/cmd";
    client.send(message);
    console.log(`${device} state set to ${state}`);
  }
  
  function updateServo(angle) {
    document.getElementById("servo-angle").textContent = angle;
    controlDevice("servo", angle);
  }
  