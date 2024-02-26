#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

#define DHTPIN 2         // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11    // DHT 11
#define BUZZER_PIN 5     // Digital pin connected to the buzzer

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Yarana";
const char* password = "12345678";

WebSocketsServer webSocket = WebSocketsServer(81);
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Real-time Monitor DHT Sensor</title>
    <style>
        @property --temperature-percent {
            syntax: '<integer>';
            inherits: false;
            initial-value: 0;
        }

        @keyframes humidity-progress {
            to { --progress-value: 50; }
        }
        
        @keyframes temperature-progress {
            to { --progress-value: var(--temperature-percent); }
        }

        .progress-bar {
            width: 100px;
            height: 100px;
            border-radius: 50%;
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .progress-bar::before {
            content: attr(data-value) '';
        }

        .humidity-progress {
            background: radial-gradient(closest-side, #222 79%, transparent 80% 100%),
                        conic-gradient(#43f94a calc(var(--progress-value) * 1%), rgb(121, 108, 110) 0);
            animation: humidity-progress 2s 1 forwards;
        }

        .temperature-progress {
            background: radial-gradient(closest-side, #222 79%, transparent 80% 100%),
                        conic-gradient(#f00 calc(var(--progress-value) * 1%), rgb(121, 108, 110) 0);
            animation: temperature-progress 2s 1 forwards;
        }

        body {
            font-family: -apple-system, system-ui, Helvetica, Arial, sans-serif;
            margin: 30px auto;
            display: flex;
            justify-content: space-around;
            align-items: center;
            flex-wrap: wrap;
            max-width: 600px;
            background: #222;
        }

        h1, h2 {
            text-align: center;
            color: white;
        }

        progress {
            visibility: hidden;
            width: 0;
            height: 0;
        }

        span {
            color: white;
        }
    </style>
</head>
<body>

    <h1>Real-time Monitor DHT Sensor</h1>

    <div class="progress-bar-container" id="humidity">
        <h2>
            <label for="humidity">Humidity</label>
        </h2>
        <div class="progress-bar humidity-progress">
            <progress id="humidityProgress" min="0" max="100" value="0"></progress>
            <span class="value-humidity">0%</span>
        </div>
    </div>
    
    <div class="progress-bar-container" id="temperature">
        <h2>
            <label for="temperature">Temperature</label>
        </h2>
        <div class="progress-bar temperature-progress">
            <progress id="temperatureProgress" min="0" max="100" value="0"></progress>
            <span class="value-temperature">0°C</span>
        </div>
    </div>

<div class="youtube-subscriber" style="margin-top: 100px;">
    <div  class="g-ytsubscribe" data-channelid="UColOAMvdtSuwGFHAIF3cnnQ" data-layout="full" data-theme="dark" data-count="default"></div>

</div>

  <script src="https://apis.google.com/js/platform.js"></script>

   <script>
    var ws = new WebSocket('ws://' + window.location.hostname + ':81/');

    ws.onmessage = function(event) {
        var data = event.data.split(',');
        if (data.length >= 2) {
            var temperature = parseFloat(data[0]);
            var humidity = parseFloat(data[1]);
            updateProgressBars(temperature, humidity);
        }
    };
function updateProgressBars(temperature, humidity) {
    var temperatureProgress = document.getElementById('temperatureProgress');
    var humidityProgress = document.getElementById('humidityProgress');
    var valueTemperature = document.querySelector('.value-temperature');
    var valueHumidity = document.querySelector('.value-humidity');

    // Calculate progress values based on the received temperature and humidity
    var temperatureProgressValue = temperature; // Set temperature directly
    var humidityProgressValue = humidity; // Set humidity directly

    // Set the progress values
    temperatureProgress.value = temperatureProgressValue;
    humidityProgress.value = humidityProgressValue;

    // Set the temperature percentage for keyframe animation
    document.documentElement.style.setProperty('--temperature-percent', temperatureProgressValue);

    // Update the displayed temperature and humidity values
    valueTemperature.textContent = temperature + '°C';
    valueHumidity.textContent = humidity + '%';

    // Set keyframe values for temperature and humidity animations
    setKeyframeValues('temperature-progress', temperature);
    setKeyframeValues('humidity-progress', humidity);

    // Check if temperature exceeds 30°C
    if (temperature > 30) {
        // Send a request to the server to trigger the buzzer
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/triggerBuzzer", true);
        xhttp.send();
    }
}
// Function to dynamically set keyframe values
function setKeyframeValues(animationName, value) {
    var styleSheet = document.styleSheets[0];
    var keyframeName = animationName;

    // Check if the keyframe already exists, and remove it
    for (var i = 0; i < styleSheet.cssRules.length; i++) {
        if (styleSheet.cssRules[i].type === CSSRule.KEYFRAMES_RULE && styleSheet.cssRules[i].name === keyframeName) {
            styleSheet.deleteRule(i);
            break;
        }
    }

    // Dynamically inject the updated keyframe
    var keyframeValue = `to { --progress-value: ${value}; }`;
    var keyframeRule = `@keyframes ${keyframeName} { ${keyframeValue} }`;
    styleSheet.insertRule(keyframeRule, styleSheet.cssRules.length);
}

</script>

</body>
</html>

)rawliteral";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());

pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as output

  webSocket.begin();
  dht.begin();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });


  server.begin();
}

void loop() {
  webSocket.loop();

  float temperature = dht.readTemperature();  // Read temperature from DHT sensor
  float humidity = dht.readHumidity();        // Read humidity from DHT sensor

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

   if (temperature > 30.0) {
        digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
        delay(2000); // Keep the buzzer on for 2 seconds
        digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
    }

  webSocket.broadcastTXT(String(temperature) + "," + String(humidity));

  // Check if data is available on Serial Monitor
  if (Serial.available() > 0) {
    // Read data from Serial Monitor
    String input = Serial.readString();

    // Send data to WebSocket clients
    webSocket.broadcastTXT(input);
  }

  // Delay for 1 second
  delay(1000);
}
