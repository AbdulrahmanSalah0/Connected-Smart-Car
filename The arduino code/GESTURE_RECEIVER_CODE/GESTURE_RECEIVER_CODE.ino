#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "time.h"
#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include <ESP32Servo.h>
// Provide the token generation process info.
/*#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Redmi"
#define WIFI_PASSWORD "123456788"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBhrGH0xtRk6Uq2XViXfYpP1foPb2SC8Zc"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "aya@gmail.com"
#define USER_PASSWORD "123456"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://car-robot-fd300-default-rtdb.firebaseio.com/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid ;

// Database main path (to be updated in setup with the user UID)
String databasePath;

// Database child nodes
int Ultrasonic;
int flamesensor;
String Ultra_path = "/Distance";
String flame_path ="/Flame";
String timePath = "/timestamp";
String parentPath;
String Command ="stop";

int timestamp;
FirebaseJson json;

// Network time Protocl Server
const char* ntpServer = "pool.ntp.org";

// Timer variables (send new readings every thirt seconds)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 500;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
WiFiMulti multi;
#endif

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("\nConnected Successfully\nIp is: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    // Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}
*/
Servo servo; 
int trig=2;
int echo=15;
int Servo=13;
int flamepin=34;
int buz =22;
int led =21;

int ENA = 12;
int ENB = 33;
int MotorA1 = 14;
int MotorA2 = 27;
int MotorB1 = 26;
int MotorB2 = 25;
int stopDist=30;

RF24 radio(4, 5);

const byte address[6] = "00001";


int receive_data[2];

int dis(){
digitalWrite(trig, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trig, HIGH);
delayMicroseconds(10);
digitalWrite(trig, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
long duration = pulseIn(echo, HIGH);
// Calculating the distance
int distance = duration* 0.034 / 2;
  return distance;
}
int checkDirection()                                            //Check the left and right directions and decide which way to turn
{
  int distances [2] = {0,0};                                    //Left and right distances
  int turnDir = 1;                                              //Direction to turn, 0 left, 1 reverse, 2 right
  servo.write(180);                                         //Turn servo to look left
  delay(500);
  distances [0] = dis();                                //Get the left object distance
  servo.write(0);                                           //Turn servo to look right
  delay(1000);
  distances [1] = dis();                                //Get the right object distance
  if (distances[0]>=200 && distances[1]>=200)                   //If both directions are clear, turn left
    turnDir = 0;
  else if (distances[0]<=stopDist && distances[1]<=stopDist)    //If both directions are blocked, turn around
    turnDir = 1;
  else if (distances[0]>=distances[1])                          //If left has more space, turn left
    turnDir = 0;
  else if (distances[0]<distances[1])                           //If right has more space, turn right
    turnDir = 2;
  return turnDir;
}

void setup() {
  // put your setup code here, to run once:

//servo.attach(Servo);
//servo.write(90);
Serial.begin(115200);
radio.begin();
radio.openReadingPipe(0,address);
radio.setPALevel(RF24_PA_MIN);
radio.setDataRate(RF24_250KBPS);
radio.startListening();
pinMode(ENA, OUTPUT);
pinMode(ENB, OUTPUT);
pinMode(MotorA1, OUTPUT);
pinMode(MotorA2, OUTPUT);
pinMode(MotorB1, OUTPUT);
pinMode(MotorB2, OUTPUT);
/*pinMode(flamepin,INPUT);
pinMode(buz,OUTPUT);
pinMode(led,OUTPUT);
pinMode(trig,OUTPUT);
pinMode(echo,INPUT);

//start Wifi
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    multi.addAP(WIFI_SSID, WIFI_PASSWORD);
    multi.run();
#else
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#endif
    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
        if (millis() - ms > 10000)
            break;
#endif
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  //Configure times
  configTime(0, 0, ntpServer);

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    config.wifi.clearAP();
    config.wifi.addAP(WIFI_SSID, WIFI_PASSWORD);
#endif

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task 
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  // Serial.println(String(Firebase.getToken()));

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";
  */
}

void loop() {
//if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
   // sendDataPrevMillis = millis();
 while(radio.available()) {
    radio.read(&receive_data, sizeof(receive_data));
    Serial.println(receive_data[0]);
 if(receive_data[0] ==1){
  digitalWrite(MotorA1, HIGH);
  digitalWrite(MotorA2, LOW);
  digitalWrite(MotorB1, LOW);
  digitalWrite(MotorB2, HIGH);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}else if(receive_data[0] ==2){
  digitalWrite(MotorA1, LOW);
  digitalWrite(MotorA2, HIGH);
  digitalWrite(MotorB1, HIGH);
  digitalWrite(MotorB2, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
} else if(receive_data[0] ==3){
  digitalWrite(MotorA1, HIGH);
  digitalWrite(MotorA2, LOW);
  digitalWrite(MotorB1, HIGH);
  digitalWrite(MotorB2, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}else if(receive_data[0] ==4){
  digitalWrite(MotorA1, LOW);
  digitalWrite(MotorA2, HIGH);
  digitalWrite(MotorB1, LOW);
  digitalWrite(MotorB2, HIGH);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}else if(receive_data[0] ==0){
  digitalWrite(MotorA1, LOW);
  digitalWrite(MotorA2, LOW);
  digitalWrite(MotorB1, LOW);
  digitalWrite(MotorB2, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  }
  /*else if(receive_data[0]==6){
  servo.write(90);  
  int distance = dis();
  int flame=analogRead(flamepin);
  flame = map(flame,0,4095,0,25);
  timestamp = getTime();
  parentPath = databasePath + "/" + String(timestamp);
  json.set(Ultra_path.c_str(), distance);
  json.set(flame_path.c_str(), flame);
  json.set(timePath, String(timestamp));
  Serial.printf("Uploading... %s\n", Firebase.RTDB.setJSON(&fbdo, databasePath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());                   //Check that there are no objects ahead
  if(distance >= stopDist)                        //If there are no objects within the stopping distance, move forward
  {
  digitalWrite(MotorA1, HIGH);
  digitalWrite(MotorA2, LOW);
  digitalWrite(MotorB1, LOW);
  digitalWrite(MotorB2, HIGH);
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
  }
  while(distance >= stopDist)                     //Keep checking the object distance until it is within the minimum stopping distance
  {
  int flame=analogRead(flamepin);
  flame = map(flame,0,4095,0,25);  
  //Serial.print("flame =");
  //Serial.println(flame);
  if (flame<20){
    Serial.println("Flame Detected....");
    digitalWrite(led,LOW);
    digitalWrite(buz,HIGH);
 
  }else{
    digitalWrite(led,HIGH);
    digitalWrite(buz,LOW);
  }
    timestamp = getTime();
    json.set(Ultra_path.c_str(), distance);
    json.set(flame_path.c_str(), flame);
    json.set(timePath, String(timestamp));
    Serial.printf("Uploading... %s\n", Firebase.RTDB.setJSON(&fbdo, databasePath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str()); 
    distance =dis();
    delay(250);
  }
  digitalWrite(MotorA1, LOW);
  digitalWrite(MotorA2, LOW);
  digitalWrite(MotorB1, LOW);
  digitalWrite(MotorB2, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);                            //Stop the motors
  int turnDir = checkDirection();                 //Check the left and right object distances and get the turning instruction
  Serial.print(turnDir);
  switch (turnDir)                                //Turn left, turn around or turn right depending on the instruction
  {
    case 0:                                       //Turn left
      digitalWrite(MotorA1, LOW);
      digitalWrite(MotorA2, HIGH);
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, HIGH);
      analogWrite(ENA, 200);
      analogWrite(ENB, 200);
      delay(400);
      break;
    case 1:                                       //Turn around
      digitalWrite(MotorA1, LOW);
      digitalWrite(MotorA2, HIGH);
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, HIGH);
      analogWrite(ENA, 200);
      analogWrite(ENB, 200);
      delay(700);
      break;
    case 2:                                       //Turn right
      digitalWrite(MotorA1, HIGH);
      digitalWrite(MotorA2, LOW);
      digitalWrite(MotorB1, HIGH);
      digitalWrite(MotorB2, LOW);
      analogWrite(ENA, 200);
      analogWrite(ENB, 200);
      delay(400);
      break;
  }
  }else if(receive_data[0]==7){
    
    Serial.printf("Get Directions... %s\n", Firebase.RTDB.getString(&fbdo, String(databasePath + "/Command")) ? "get new directions " : fbdo.errorReason().c_str());
    String get_Command = fbdo.to<String>();
    Serial.println(get_Command);
    if(get_Command.equals("forward")){
      digitalWrite(MotorA1, HIGH);
      digitalWrite(MotorA2, LOW);
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, HIGH);
      analogWrite(ENA, 150);
      analogWrite(ENB, 150);

    }else if(get_Command.equals("backward")){
      digitalWrite(MotorA1, LOW);
      digitalWrite(MotorA2, HIGH);
      digitalWrite(MotorB1, HIGH);
      digitalWrite(MotorB2, LOW);
      analogWrite(ENA, 150);
      analogWrite(ENB, 150);

    }else if(get_Command.equals("right")){
      digitalWrite(MotorA1, HIGH);
      digitalWrite(MotorA2, LOW);
      digitalWrite(MotorB1, HIGH);
      digitalWrite(MotorB2, LOW);
      analogWrite(ENA, 150);
      analogWrite(ENB, 150);
    }else if(get_Command.equals("left")){
      digitalWrite(MotorA1, LOW);
      digitalWrite(MotorA2, HIGH);
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, HIGH);
      analogWrite(ENA, 150);
      analogWrite(ENB, 150);
    }else{
      digitalWrite(MotorA1, LOW);
      digitalWrite(MotorA2, LOW);
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, LOW);
      analogWrite(ENA, 0);
      analogWrite(ENB, 0);

    }
  }
  } */
  }
}
 