#include <Arduino.h>

// HTML web page to handle 3 input fields (inputString, inputInt, inputFloat)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
    if (!!window.EventSource) {
      var source = new EventSource('/events');
      source.addEventListener('open', function(e) {
        console.log("Events Connected");
      }, false);
      source.addEventListener('error', function(e) {
        if (e.target.readyState != EventSource.OPEN) {
          console.log("Events Disconnected");
        }
      }, false);
      source.addEventListener('message', function(e) {
        console.log("message", e.data);
      }, false);
      source.addEventListener('SensorValue', function(e) {
        console.log("SensorValue", e.data);
        document.getElementById("SensorValue").innerHTML = e.data;
      }, false);
    }
  </script></head><body>
  <p>Sensor Value:</p>
  <p><span id =SensorValue>%SensorValue%</p>
  <br></br>
  <form action="/get" target="hidden-form">
    Stop Limit (current value %StopLimit%): <input type="number " name="StopLimit">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <form action="/get" target="hidden-form">
    Ok End (current value %OkEnd%): <input type="number " name="OkEnd">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <form action="/get" target="hidden-form">
    Good End (current value %GoodEnd%): <input type="number " name="GoodEnd">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <form action="/get" target="hidden-form">
    Good Start (current value %GoodStart%): <input type="number " name="GoodStart">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <form action="/get" target="hidden-form">
    Far Start (current value %FarStart%): <input type="number " name="FarStart">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <br></br>
  <p>The display will turn off after the Timeout (in seconds):</p>
  <form action="/get" target="hidden-form">
    Timout (current value %Timeout%): <input type="number " name="Timeout">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <br></br>
  <p>Adding 'default' to any of the entries above will reset that value to the defaults</p>
  <p>Adding 'reset' to any of the entries above will reset all values to the defaults</p>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";