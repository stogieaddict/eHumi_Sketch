/*
  AM2302 Temperature / Humidity Sensor (DHT22)
*/

#include "DHT.h"          // DHT & AM2302 library
#include <Process.h>
#include <Console.h>

// Data pin connected to AM2302
#define DHTPIN 2
#define DHTTYPE DHT22       // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

#define APIKEY "API_KEY_GOES_HERE"
#define FEEDID FEED_ID_GOES_HERE

String dataString = "";
const unsigned long postingInterval = 5000;  //delay between updates to xively.com
unsigned long lastRequest = 0;      // when you last made a request

static char tbuffer[6];
static char hbuffer[6];

void setup() {
  // Setup serial monitor
  Bridge.begin();
  Console.begin();
  
  while (!Console) {
    ;
  }
  
  Serial.begin(9600);
 
  dht.begin();
}
  

void loop() {

   // get a timestamp so you can calculate reading and sending intervals:
  long now = millis();

  // if the sending interval has passed since your
  // last connection, then connect again and send data:
  if (now - lastRequest >= postingInterval) {
    updateData();
    sendData();
    lastRequest = now;
  }
  
}

void updateData() {
   
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  
  // Convert temperature from C to F
  temp = temp * 9 / 5 + 32;

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(temp) || isnan(humidity)) {
    Serial.println(F("Failed to read from DHT"));
    Console.println("Failed to read from DHT");
  }
  else {
    Serial.print(F("Humidity: ")); 
    Serial.print(humidity);
    Serial.print(F(" %\t"));
    Serial.print(F("Temperature: ")); 
    Serial.print(temp);
    Serial.println(F(" F"));
    
    Console.print("Humidity: "); 
    Console.print(humidity);
    Console.print(" %\t");
    Console.print("Temperature: "); 
    Console.print(temp);
    Console.println(" F");
    
    //convert the sensor data from float to string
    dtostrf(temp,5, 2, tbuffer);
    dtostrf(humidity,5, 2, hbuffer);
  }
  dataString = "Temperature,";
  dataString += tbuffer;
  dataString += "\nHumidity,";
  dataString += hbuffer;
}

void sendData() {
  String apiString = "X-ApiKey: ";
  apiString += APIKEY;
  
// form the string for the URL parameter:
  String url = "https://api.xively.com/v2/feeds/";
  url += FEEDID;
  url += ".csv";
  
// Send the HTTP PUT request

// Is better to declare the Process here, so when the
// sendData function finishes the resources are immediately
// released. Declaring it global works too, BTW.
  Process xively;
  Serial.print("\n\nSending data to Xively... ");
  Console.print("\n\nSending data to Xively... ");
  xively.begin("curl");
  xively.addParameter("-k");
  xively.addParameter("--request");
  xively.addParameter("PUT");
  xively.addParameter("--data");
  xively.addParameter(dataString);
  xively.addParameter("--header");
  xively.addParameter(apiString); 
  xively.addParameter(url);
  xively.run();
  Serial.println("Done.");
  Console.println("Done.");

}
