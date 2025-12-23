#pragma once
#include <Arduino.h>

static const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="uk">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>OverTime Thermostat</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            background-color: #121212; 
            color: #ffffff; 
            text-align: center;
            padding: 20px; 
        }
        h1 { color: #facc15; }
        .grid { 
            display: grid; 
            grid-template-columns: 1fr 1fr 1fr; 
            gap: 10px; 
            max-width: 600px; 
            margin: 0 auto; 
            align-items: center; 
            font-size: 16px;
        }
        .label { 
            color: #aaa; 
            text-align: left; 

        }
        .value { 
            text-align: center; 
            font-weight: bold; 
        }
        .control { 
            text-align: center; 
            padding-left: 10px; 
        }
        .status-indicator {
            display: inline-block;
            width: 55px;
            height: 20px;
            border-radius: 5px;
            background-color: #ff0000ff;
            border: 2px solid #fff;
        }
        .status-indicator.blue {
            background-color: #007bff;
        } 
        input[type="number"] {
            width: 60px; padding: 5px; 
            border-radius: 5px; 
            border: none; 
        }
        button { 
            background-color: #facc15; 
            border: none; 
            padding: 5px 15px; 
            border-radius: 5px; 
            cursor: pointer; 
            font-weight: bold;
            color: #000; 
        }
        button:hover { background-color: #44ff00ff; }
        .col1 { 
        grid-column: 1;
        }
        .col2 { 
            grid-column: 2;
        }
        .col3 { 
            grid-column: 3; 
        }
        .label, .value, .control {
            margin-top: 15px;
        }
    </style>
</head>
<body>
    <h1>TeMpErAtUrE CoNtRoL KlEbaN</h1>
    <div class="grid">
        <div class = "col1">
            <div class="label">Temperature:</div>
            <div class="label">Humidity:</div>
            <div class="label">Led color:</div>
            <div class="label">Des. Temperature:</div>
        </div>
        <div class = "col2">
            <div class="value"><span id="temp">--</span> &deg;C</div>
            <div class="value"><span id="hum">--</span> %</div>
            <div class="value">
            <div id="ledIndicator" class="status-indicator"></div>
            </div>
            <div class="value"><span id="targetTemp">--</span> &deg;C</div>
        </div>
        <div class = "col3">
            <div class="control" style = "color:black;">--</div>
            <div class="control" style = "color:black;">--</div>
            <div class="control" style = "color:black;">--</div>

            <div class="control">
                <input type="number" id="inputTemp">
                <button onclick="sendTarget()">SET</button>
            </div>
        </div>
    </div>
   


    
    <script>
        function updateData() {
            fetch('/readings')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('temp').innerText = data.temperature.toFixed(1);
                    document.getElementById('hum').innerText = data.humidity.toFixed(1);
                    document.getElementById('targetTemp').innerText = data.target.toFixed(1);

                    const indicator = document.getElementById('ledIndicator');
                    if (parseFloat(data.temperature) < parseFloat(data.target)) {
                        indicator.classList.add('blue');
                    } else {
                        indicator.classList.remove('blue');
                    }
                })
                .catch(error => console.error('Error:', error));
        }

        function sendTarget() {
            const val = document.getElementById('inputTemp').value;
            if(val) {
                fetch('/set-target?val=' + val)
                .then(response => {
                    if(response.ok) {
                        alert("Temperature Saved!");
                        updateData();
                    }
                });
            }
        }
        setInterval(updateData, 200);
        window.onload = updateData;
    </script>
</body>
</html>
)HTML";