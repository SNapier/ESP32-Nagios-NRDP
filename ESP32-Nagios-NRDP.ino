#include <WiFi.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>


void setup() {
  Serial.begin(115200);
  delay(10);

  
  //Wifi Setup
  const char* ssid     = "YourSSID";
  const char* password = "YourPassword";
  const char* hostname = "Arduino-ESP32";

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println("SIGNAL STRENGTH:");
  Serial.println(WiFi.RSSI());

  //Mem Usage
  int freeHeap = ESP.getFreeHeap();  
  Serial.println();
  Serial.print("FREEHEAP=");
  Serial.println(freeHeap);
  Serial.println();
}

void loop() {
  // Send NRDP Data Every 60 Seconds
  delay(60000);
  nrdpchecks();
}

void nrdpchecks() {
  Serial.println();
  Serial.println("===============================");
  Serial.println("NAGIOS JSONDATA STRING FOR NRDP");
  Serial.println("===============================");
  Serial.println();

  JSONVar jsondata;
  
  jsondata["checkresults"][0] = "";
  jsondata["checkresults"][0]["checkresult"]["type"] = "host";
  jsondata["checkresults"][0]["checkresult"]["checktype"] = "1";
  jsondata["checkresults"][0]["hostname"] = hostname;
  jsondata["checkresults"][0]["state"] = "0";
  jsondata["checkresults"][0]["output"] = "Arduino-ESP32-NAGIOS|up=100;checks=6;";

  //Exapmle Service Check - Sens1 = OK
  jsondata["checkresults"][1]["checkresult"]["type"] = "service";
  jsondata["checkresults"][1]["checkresult"]["checktype"] = "1";
  jsondata["checkresults"][1]["hostname"] = "Arduino-ESP32";
  jsondata["checkresults"][1]["servicename"] = "arduino--esp32--example-sens1-OK";
  jsondata["checkresults"][1]["state"] = "0";
  jsondata["checkresults"][1]["output"] = "WARNING:Arduino-ESP32-Sens1=(111)|sens1=111;200;300;";

  //Exapmle Service Check - Sens2 = WARNING
  jsondata["checkresults"][2]["checkresult"]["type"] = "service";
  jsondata["checkresults"][2]["checkresult"]["checktype"] = "1";
  jsondata["checkresults"][2]["hostname"] = hostname;
  jsondata["checkresults"][2]["servicename"] = "arduino--esp32--example-sens2-WARNING";
  jsondata["checkresults"][2]["state"] = "1";
  jsondata["checkresults"][2]["output"] = "WARNING:Arduino-ESP32-Sens2=(222)|sens1=222;200;300;";

  //Exapmle Service Check - Sens3 = CRITICAL
  jsondata["checkresults"][3]["checkresult"]["type"] = "service";
  jsondata["checkresults"][3]["checkresult"]["checktype"] = "1";
  jsondata["checkresults"][3]["hostname"] = hostname;
  jsondata["checkresults"][3]["servicename"] = "arduino--esp32--example-sens3-CRITICAL";
  jsondata["checkresults"][3]["state"] = "2";
  jsondata["checkresults"][3]["output"] = "CRITICAL:Arduino-ESP32-Sens3=(333)|sens1=333;200;300;";

  //Exapmle Service Check - Sens4 = WARNING
  jsondata["checkresults"][4]["checkresult"]["type"] = "service";
  jsondata["checkresults"][4]["checkresult"]["checktype"] = "1";
  jsondata["checkresults"][4]["hostname"] = "Arduino-ESP32";
  jsondata["checkresults"][4]["servicename"] = "arduino--esp32--example-sens4-UNKNOWN";
  jsondata["checkresults"][4]["state"] = "3";
  jsondata["checkresults"][4]["output"] = "UNKNOWN:Arduino-ESP32-Sens2=()|sens4=;200;300;";

  //Exapmle Service Check - Sens5
  //Calculate the service state based upon the value of the sensor data 
  int state = 0;
  
  //Sensor Data generated at random
  int sens5 = random(111, 333);
  
  //Sensor Data Thresholds Defined
  int sens5warn = 200;
  int sens5crit = 300;
  String displayState = "";
  
  //Evaluate the sens5 value if greater than or equal to the critical and warning values, set the state based on the evaluations
  //Critical
  if(sens5 >= 300){
    state = 2;
    displayState = "CRITICAL:";
  }
  //Warning
  else if((sens5 < 300 ) && (sens5 >= 200)){
    state = 1;
    displayState = "WARNING:";
  }
  //OK
  else if ((sens5 >= 0) && (sens5 < 200)){
    state = 0;
    displayState = "OK:";  
  }
  //No Data = Unknown
  else{
    state = 3;
    displayState = "UNKNOWN:"; 
  }
  
  // Service Output
  String output = displayState;
  output += "Arduino-ESP32-Sens5=(";
  output += sens5;
  output += ")|sens5=";
  output += sens5;
  output += ";";
  output += sens5warn;
  output += ";";
  output += sens5crit;
  output += ";";
  
  // Add the service check to the checkresults array
  jsondata["checkresults"][5]["checkresult"]["type"] = "service";
  jsondata["checkresults"][5]["checkresult"]["checktype"] = "1";
  jsondata["checkresults"][5]["hostname"] = hostname;
  jsondata["checkresults"][5]["servicename"] = "arduino--esp32--example-sens5-ALL";
  jsondata["checkresults"][5]["state"] = state;
  jsondata["checkresults"][5]["output"] = output;

  // JSON.stringify(myVar) can be used to convert the json var to a String
  // We prepend the JSONDATA= to the string for processing via Nagios
  // See NRDP Documentation for full description
  String jdata = "JSONDATA=";
  String jsonString = JSON.stringify(jsondata);
  jdata += jsonString;
  Serial.println(jdata);
  Serial.println();
  
  //Submit check data in JSON format to NRDP via HTTP(Post) 
  HTTPClient http;
  
  // The NRDP url from the Nagios server
  // Includes the query operator
  String url = "http://YourNRDPIP/nrdp/?";
  
  //NRDP Token (With trialing apmersand)
  url += "token=token&";
  // NRDP Command (Submit Check)
  url += "cmd=submitcheck&";
  url += jdata;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.POST("");
  String result = http.getString();
  
  //Convert the response string of httpclient into JSON
  JSONVar doc = result;
  
  //Parse the JSON object doc into an array
  JSONVar myResult = JSON.parse(doc);
  
  //The response from nagios is in the result array
  Serial.println();
  Serial.print("RESULT=");
  Serial.println(myResult["result"]);
  Serial.println();
  
  http.writeToStream(&Serial);
  http.end();
}
