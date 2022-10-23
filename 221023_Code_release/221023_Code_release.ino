//Air_Environment_Information_collection_device_with_Web-Based_IOT
//Copyright 2022. Jung Jiwon all rights reserved.

//This code has been tested in the environment of the link.
//https://github.com/Wiznet/WizFi360_arduino_library

//Before uploading, make changes to suit your situation. ----------------------------------------------------
char ssid[] = "YOUR-SSID-NAME";      // WIFI SSID 
char pass[] = "YOUR-SSID-PASSWORD";  // WIFI password

//RGB LED
#define RED 12
#define GREEN 11
#define BLUE 10

//SENSORS
#define DUST_PIN 27           //Analog input pin connected to PM2.5 sensor
#define DUST_LED 26           //Digital pin connected to infrared LED inside PM2.5 sensor
#define DHT11_PIN 14          //Digital pin connected to DHT11(Temperature and Humidity sensor)

/* Sensor name and control method
 * DHT11 : Temperature and Humidity sensor / digital input.
 * MH-Z19B : CO2 sensor / UART
 * GP2Y1014AU(PM2.5) : Fine dust sensor / analog input(PM2.5 concentration in air) and digital output(Control the infrared LED inside the sensor)
 */
//-----------------------------------------------------------------------------------------------------------

//Libraries
#include <LiquidCrystal.h> //Arduino Official Library
#include <DHT.h>           //https://github.com/adafruit/DHT-sensor-library
#include <MHZ19.h>         //https://github.com/strange-v/MHZ19
#include "WizFi360.h"      //https://github.com/Wiznet/WizFi360_arduino_library

//Declarations
LiquidCrystal lcd(16, 17, 18, 19, 21, 22);  //(rs, en, d4, d5, d6, d7)
DHT dht11(DHT11_PIN, DHT11);           
MHZ19 mhz(&Serial1);
WiFiServer server(80);

//for WIFI
int status = WL_IDLE_STATUS;  // WIFI status
int reqCount = 0;             // number of requests received
IPAddress ip;


//DHT11, MH-Z19B  
float temp, humi = 0;          //Temperature, Humidity
unsigned short dust_read = 0;  //analog input value (0~1023)
float Voltage = 0.00;          //analog(0~1023) to Volts(V) 
float Lowest_Voltage = 0.28;   //Reference volt values in a clean environment without PM2.5
float ugm3 = 0.0;              //PM2.5 micrograms per cubic meter
int CO2;

//PM2.5 sensor value average 
const short numReadings = 25;                 //How many PM2.5 measurements will be averaged (Measured every 0.2 seconds.)
unsigned short readings[numReadings] = {0,};  //Create an array from 0 to numReadings, initial value of all arrays = 0
unsigned short readIndex = 0;                 //array index number
unsigned int total = 0;                       //sum of measures
float average = 0.00;                         //PM2.5 average value

//Time-related variables 
unsigned long now = 0;                 //save current time
unsigned long last_Dust = 0;           //Time of last pm2.5 variable value refresh
unsigned long last_LCD = 0;            //Time of last LCD screen refresh

//for WIFI WEB Server
String html_temp = "Temperature : " + String(temp, 1) + " °C<br>\r\n";
String html_humi = "Humidity : " + String(humi, 0) + " %<br>\r\n";
String html_CO2 = "CO2 : " + String(CO2) + " ppm<br>\r\n";
String html_dust = "PM2.5 : " + String(average, 2) + " ug<br><br>\r\n";


//SETUP for First CORE (Sensors and LCD)
void setup()
{
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  
  pinMode(DUST_PIN,INPUT);             
  pinMode(DUST_LED, OUTPUT);          

  lcd.begin(20,4);
  dht11.begin();                
  Serial1.begin(9600);  //for MH-Z19B(CO2)
}


//SETUP for Second CORE (WIFI WEB Server based on WizFi360)
void setup1()
{
  Serial2.begin(115200);
  WiFi.init(&Serial2);

  while ( status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
  }
  
  ip = WiFi.localIP();
  server.begin();
}

//Function to measure PM2.5
void dustSensor(){
  //PM2.5 sensor measurement formula 
  digitalWrite(DUST_LED,LOW);        //Infrared LED on
  delayMicroseconds(280);            //Wait for it to fully turn on
  dust_read = analogRead(DUST_PIN);  //Read analog value(0~1023)
  delayMicroseconds(40);             //Wait for stability
  digitalWrite(DUST_LED,HIGH);       //Infrared LED off
  //delayMicroseconds(9680);         //Unnecessary, because it is executed every 0.2 seconds by LOOP.

  //Sensor reading averaging algorithm 
  total = total - readings[readIndex];  //Remove old array value from total
  readings[readIndex] = dust_read;      //Save sensor values in an array
  total = total + readings[readIndex];  //Save new array value in total
  readIndex = readIndex + 1;            //Index number +1

  if (readIndex >= numReadings) readIndex = 0;   //If the index number range is exceeded, it is initialized to 0.
  
  ugm3 = total / numReadings;                    //PM2.5 ug/m3
  Voltage = ugm3 * 3.30 / 1023.00 ;              //Convert to volts
  average = (Voltage - Lowest_Voltage) / 0.005;  //Convert to ug/m3

  if(average <= 0) average = 0;
}


//The color of RGB LED will change according to fine dust standards of Korea's Ministry of Environment.
void rgb(){
  if(0 <= average && average <= 30){
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
  }

  else if(30 < average && average <= 80){
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
  }

  else if(80 < average && average <= 150){
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
  }
    
  else{
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
  }
}


//LOOP for First CORE 
void loop(){ 
  now = millis() + 1000;  //now = Time since power on + 1000(1s) 
                          //To satisfy the condition at the time of first execution.                                      
  if(now - last_Dust >= 200){        
    last_Dust = now;
    dustSensor();
    rgb();
  }
  
  if(now - last_LCD >= 1000){         
    last_LCD = now;

    //Refresh sensor values 
    temp = dht11.readTemperature();
    humi = dht11.readHumidity();
    
    MHZ19_RESULT response = mhz.retrieveData();
    if(response == MHZ19_RESULT_OK) CO2 = mhz.getCO2();
    
    html_temp = "Temperature : " + String(temp, 1) + " °C<br>\r\n";
    html_humi = "Humidity : " + String(humi, 0) + " %<br>\r\n";
    html_CO2 = "CO2 : " + String(CO2) + " ppm<br>\r\n";
    html_dust = "PM2.5 : " + String(average, 2) + " ug<br><br>\r\n";

    //LCD
    lcd.setCursor(1,0);
    lcd.print(temp, 1);
    lcd.print("C  ");
    lcd.print(humi, 0);
    lcd.print("%  ");
    lcd.print(average, 1);
    lcd.print("ug ");
    lcd.setCursor(0,1);
    lcd.print("CO2  : ");
    lcd.print(CO2);
    lcd.print(" ppm ");
    lcd.setCursor(0,2);
    lcd.print("SSID : ");
    lcd.print(ssid);
    lcd.setCursor(0,3);
    lcd.print("IP   : ");
    lcd.print(ip);
  }
}


//for WIFI WEB Server
String html_1 = 
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n\r\n"
"<!DOCTYPE HTML>\r\n"
"<html>\r\n"
"<head>\r\n"
"<meta charset=\"UTF-8\" http-equiv=\"refresh\" content=\"1\">\r\n"
"<title>WizFi360 IoT Web</title>\r\n"
"</head>\r\n"
"<body>\r\n"
"<h2>Air Environment Information </h2>\r\n"
"<br>\r\n";

String html_good = "The concentration of fine dust in the air is Good.\r\n";
String html_moderate = "The concentration of fine dust in the air is Moderate.\r\n";
String html_unhealthy = "The concentration of fine dust in the air is Unhealthy.\r\n";
String html_veryunhealthy = "The concentration of fine dust in the air is Very Unhealthy.\r\n";

String html_2 =
"</body>\r\n"
"</html>\r\n";


//LOOP for Second CORE 
void loop1() {
  //WIFI WEB Server based on WizFi360 
  WiFiClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          client.print(html_1);
          client.print(html_temp);
          client.print(html_humi);
          client.print(html_CO2);
          client.print(html_dust);

          if(0 <= average && average <= 30){
            client.print(html_good);
          }
          else if(30 < average && average <= 80){
            client.print(html_moderate);
          }
          else if(80 < average && average <= 150){
            client.print(html_unhealthy);
          } 
          else{
            client.print(html_veryunhealthy);
          }

          client.print(html_2);
          break;
        }
        
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(10);
    client.stop();
  }
}
