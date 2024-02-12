#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); 
char key;
bool ok=true;
char press;
const int passwordLength = 6;
char password[passwordLength + 1] = "123456";
char enteredPassword[passwordLength + 1];
int passwordIndex = 0;
const int ROWS = 4;
const int COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, 3, 2, A7};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String passwordMask="*";
const int x_out = A0;
const int y_out = A1;

RF24 radio(9,10);
const byte address[6] = "00001";
int send_data;
int angle;
int mode ;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
radio.begin();
radio.openWritingPipe(address);
radio.setPALevel(RF24_PA_MIN);
radio.setDataRate(RF24_250KBPS);
radio.stopListening();
lcd.init();                  
lcd.backlight();
lcd.setCursor(0,0);
lcd.print("Enter Password");

}

void loop() {
  // put your main code here, to run repeatedly:
while(ok){
char key = keypad.getKey();
  // If a key is pressed, add it to the entered password and display masked input on LCD
  if (key != NO_KEY && passwordIndex < passwordLength) {
    enteredPassword[passwordIndex] = key;
    lcd.setCursor(passwordIndex,1);
    lcd.print(passwordMask);
    passwordIndex++;
  }
  // If the entered password is complete, verify it
  if (passwordIndex == passwordLength) {
    enteredPassword[passwordIndex] = '\0'; // Null-terminate the entered password
    // Check if the password is correct
    if (strcmp(enteredPassword, password) == 0) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Password Correct");
      Serial.println("\nPassword Correct");
      ok=false;
    }else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Password Incorrect");
      Serial.println("\nPassword Incorrect");
       // Reset entered password
      memset(enteredPassword, 0, sizeof(enteredPassword));
      passwordIndex = 0;
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
  }}}
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("choose mode:");
  angle=0;
  radio.write(&angle,sizeof(angle));
  char press =keypad.getKey();
  if(press!=NO_KEY){
    Serial.println(press);
      if(press=='1'){
        ok =true;
        while(ok){
          key=keypad.getKey();
          Serial.println(key);
          if(key=='5'){
            ok=false;
            angle=0;
            lcd.clear();
          }else{
          angle=6;
          radio.write(&angle,sizeof(angle));
          Serial.println("Radar On");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Radar On");
        }
      }          
      }
      else if(press=='2'){
        ok =true;
        while(ok){
          key=keypad.getKey();

          if(key=='5'){
            ok=false;
            lcd.clear();
          }else{
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Manual car");
          lcd.setCursor(5,1);
          lcd.print("Activated");
          send_data = analogRead(y_out);
          int send =analogRead(x_out);
          Serial.print(send_data);
          Serial.print(",");
          Serial.println(send);

          if(send_data < 325){
            angle=1;
            Serial.println(angle);
            radio.write(&angle, sizeof(angle));
          }else if(send_data > 375){
            angle=2;
            Serial.println(angle);
            radio.write(&angle, sizeof(angle));
          } else if(send < 335){
            angle=3;
            Serial.println(angle);
            radio.write(&angle, sizeof(angle));
          }else if(send > 380){
            angle=4;
            Serial.println(angle);
            radio.write(&angle, sizeof(angle));
          }else {
            angle=0;
            Serial.println(angle);
            radio.write(&angle, sizeof(angle));
          }
        }
      }
      }else if(press=='3'){
        ok =true;
        while(ok){
          key=keypad.getKey();

          if(key=='5'){
            ok=false;
            lcd.clear();
          }else{
            angle=7;
            radio.write(&angle,sizeof(angle));
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Flutter APP");
            lcd.setCursor(5,1);
            lcd.print("Activated");
          }
        }
      }


    }
}

