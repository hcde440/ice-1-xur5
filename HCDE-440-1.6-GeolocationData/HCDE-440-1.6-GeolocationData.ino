/*A sketch to get the ESP8266 on the network and connect to some open services via HTTP to
 * get our external IP address and (approximate) geolocative information in the getGeo()
 * function. To do this we will connect to http://freegeoip.net/json/, an endpoint which
 * requires our external IP address after the last slash in the endpoint to return location
 * data, thus http://freegeoip.net/json/XXX.XXX.XXX.XXX
 * 
 * This sketch also introduces the flexible type definition struct, which allows us to define
 * more complex data structures to make receiving larger data sets a bit cleaner/clearer.
 * 
 * jeg 2017
 * 
 * updated to new API format for Geolocation data from ipistack.com
 * brc 2019
*/

#include "arduino_secrets.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> //provides the ability to parse and construct JSON objects

String ssid = "hcde440";
String pass = "yoloswag";

String weatherKey = MET_ID;
// temperature, humidity, windspeed, wind direction, and cloud conditions
typedef struct { //here we create a new data type definition, a box to hold other data types
  String tp; // temperature
  String pr; // pressure
  String hd; // humidity
  String ws; // windspeed
  String wd; // wind direction
  String cd; // cloud conditions
  
} MetData;     //then we give our new data structure a name so we can use it in our code
MetData conditions; //we have created a MetData type, but not an instance of that type,
                  //so we create the variable 'condition' of type GeoData
                 
void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));

  // Connecting to WiFi
  Serial.print("Connecting to "); Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(); Serial.println("WiFi connected"); Serial.println();
  Serial.print("Your ESP has been assigned the internal IP address ");
  Serial.println(WiFi.localIP());
  getMet(); // Get conditions
}

void loop() {
}

void getMet() {
  HTTPClient theClient; // Creating the client for calling the API
  String apiCall = "http://api.openweathermap.org/data/2.5/weather?id=5809844";  //ID for Seattle is 5809844
  apiCall += "&APPID=";
  apiCall += weatherKey;
  apiCall += "&units=imperial";
  theClient.begin(apiCall); // Calling API using the URL
  int httpCode = theClient.GET(); // Getting http code
  if (httpCode > 0) {

    if (httpCode == HTTP_CODE_OK) {             // If the code is 200
      String payload = theClient.getString(); // Getting the string of information
      DynamicJsonBuffer jsonBuffer;                       // Storing JSON
      JsonObject& root = jsonBuffer.parseObject(payload); // Converting into JSON
      if (!root.success()) {                              // If parsing failed
        Serial.println("parseObject() failed in getMet()."); 
        return;
      }
      // Below is accessing the JSON library for the conditions
      conditions.tp = root["main"]["temp"].as<String>();
      Serial.println("temperature:" + conditions.tp);
      conditions.pr = root["main"]["pressure"].as<String>();
      Serial.println("pressure:" + conditions.pr);
      conditions.hd = root["main"]["humidity"].as<String>();
      Serial.println("humidity:" + conditions.hd);
      conditions.cd = root["clouds"]["all"].as<String>();
      Serial.println("cloud conditions:" + conditions.cd);
      conditions.ws = root["wind"]["speed"].as<String>();
      Serial.println("wind speed:" + conditions.ws);
      int deg = root["wind"]["deg"].as<int>();
      conditions.wd = getNSEW(deg);
      Serial.println("wind direction:" + conditions.wd);
    }
  }
  else {
    Serial.printf("Something went wrong with connecting to the endpoint in getMet().");
  }
}


// The below converts degrees into directions
String getNSEW(int d) {
  String direct;

  //Conversion based upon http://climate.umn.edu/snow_fence/Components/winddirectionanddegreeswithouttable3.htm
  if (d > 348.75 && d < 360 || d >= 0  && d < 11.25) {
    direct = "north";
  };
  if (d > 11.25 && d < 33.75) {
    direct = "north northeast";
  };
  if (d > 33.75 && d < 56.25) {
    direct = "northeast";
  };
  if (d > 56.25 && d < 78.75) {
    direct = "east northeast";
  };
  if (d < 78.75 && d < 101.25) {
    direct = "east";
  };
  if (d < 101.25 && d < 123.75) {
    direct = "east southeast";
  };
  if (d < 123.75 && d < 146.25) {
    direct = "southeast";
  };
  if (d < 146.25 && d < 168.75) {
    direct = "south southeast";
  };
  if (d < 168.75 && d < 191.25) {
    direct = "south";
  };
  if (d < 191.25 && d < 213.75) {
    direct = "south southwest";
  };
  if (d < 213.25 && d < 236.25) {
    direct = "southwest";
  };
  if (d < 236.25 && d < 258.75) {
    direct = "west southwest";
  };
  if (d < 258.75 && d < 281.25) {
    direct = "west";
  };
  if (d < 281.25 && d < 303.75) {
    direct = "west northwest";
  };
  if (d < 303.75 && d < 326.25) {
    direct = "south southeast";
  };
  if (d < 326.25 && d < 348.75) {
    direct = "north northwest";
  };
  return direct;
}


