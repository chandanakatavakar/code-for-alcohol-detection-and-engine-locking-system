#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD setup, adjust the address accordingly

const int touchPin = 2;    // Digital pin connected to the touch sensor
const int mq3Pin = A0;     // Analog pin connected to the MQ-3 sensor
const int motorPin = 9;    // Digital pin connected to the Relay
const int buzzerPin = 8;  // Digital pin connected to the buzzer
const int ledPin1 = 4;    // Digital pin connected to the first LED
const int ledPin2 = 5;    // Digital pin connected to the second LED
const int vibratorPin = 3; // Digital pin connected to the vibrator

int touchCounter = 0;     // Counter to keep track of touch sensor LOW states


// Declare functions
void sendSMS(String message);

int flag=0;
String url;
// SIM800A configuration
SoftwareSerial sim800a(11,10 ); // RX, TX connect TX, RX


SoftwareSerial gpsSerial(12, 13);  // RX, TX connect TX, RX


void setup() {
  lcd.begin();
  lcd.backlight();

  pinMode(touchPin, INPUT);   // Set touch sensor pin as input
  pinMode(motorPin, OUTPUT);  // Set motor pin as output
  pinMode(buzzerPin, OUTPUT); // Set buzzer pin as output
  pinMode(ledPin1, OUTPUT);   // Set first LED pin as output
  pinMode(ledPin2, OUTPUT);   // Set second LED pin as output
  pinMode(vibratorPin, OUTPUT); // Set vibrator pin as output

  Serial.begin(9600); // Initialize serial communication with PC
  sim800a.begin(9600); // Initialize SIM800A communication

    Serial.begin(9600); // Serial monitor baud rate
  gpsSerial.begin(9600); // GPS module baud rate
  
  lcd.print("System Started");
  
  delay(3000);
}

void loop() {

  float alcoholConcentration = analogRead(mq3Pin);
  alcoholConcentration=map(alcoholConcentration, 0, 1023, 0, 100); 
  
  // Print the alcohol concentration
  Serial.print("Alcohol Concentration: ");
  Serial.print(alcoholConcentration);
  Serial.println(" %");

  // Display sensor values on the LCD
  lcd.clear();
  lcd.print("Alcohol :");
  lcd.print(alcoholConcentration);
  lcd.print("%");
  // Read the state of the touch sensor
  int touchState = digitalRead(touchPin);

  // Print the state of the touch sensor
  Serial.print("Touch Sensor State: ");
  if (touchState == LOW) {
    Serial.println("LOW");
    
  } else {
    Serial.println("HIGH");
  }

  // Check if any sensor is triggered or alcohol concentration is more than 20
  if (touchState == LOW ) {
    // Increment the touch counter
    touchCounter++;

    lcd.setCursor(0, 1);// 0 is column 1 is row
    lcd.print("Sleep Detected:");
    lcd.print(touchCounter);
    // Check if the touch sensor count is 3 or alcohol concentration is above the threshold, make the vibrator vibrate
    if (touchCounter > 3 ) {
      digitalWrite(vibratorPin, HIGH);
      delay(100);
      digitalWrite(vibratorPin, LOW);
      delay(100);
      
    }


    // Check if the touch sensor count is more than 10 and alcohol detected, stop the motor and turn on the lights and buzzer
    if (touchCounter > 5 ) {
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(buzzerPin, HIGH); // Send SMS indicating the emergency status
      //sendSMS("Emergency: Motor Stopped and Lights On");
       lcd.setCursor(0, 1); //0 is column 1 is row
    lcd.print("The Car Stopped  ");
    }
  } else {
    // Reset the touch counter and alcohol detection flag when the touch sensor is not LOW
    touchCounter = 0;
    
    digitalWrite(vibratorPin, LOW); // Turn off the vibrator
    digitalWrite(buzzerPin, LOW); // Turn off the buzzer
    digitalWrite(ledPin1, LOW); // Turn off the first LED
    digitalWrite(ledPin2, LOW); // Turn off the second LED
  }

  if(alcoholConcentration>30 && flag<5)
  {
      sendSMS("Alocohol detected at location : "+url);
      flag++; 
      lcd.setCursor(0, 1);
      lcd.print("Message sent");
      
  }
  
  if(gpsSerial.available()) {
    String sentence = gpsSerial.readStringUntil('\n');
    if (sentence.startsWith("$GPGGA")) {
      // Split the sentence into individual fields
      String fields[15];
      int fieldCount = 0;
      int startIndex = 0;
      for (int i = 0; i < sentence.length(); i++) {
        if (sentence.charAt(i) == ',') {
          fields[fieldCount] = sentence.substring(startIndex, i);
          fieldCount++;
          startIndex = i + 1;
        }
      }

      // Extract latitude and longitude
      String latitude = fields[2];
      String longitude = fields[4];

      // Print latitude and longitude
      float lon = atof(longitude.c_str()) / 100;
      float latu = atof(latitude.c_str()) / 100;

      // Concatenate the string values and store them in the 'url' variable
      url = "https://www.google.com/maps/@";                               
      url += String(latu, 7);
      url += ",";
      url += String(lon, 7);
      url += ",15z";

      // Print the GPS location to the serial monitor
      Serial.println(url);
    }
  }
    if(alcoholConcentration>30 || touchCounter > 5)
  {
     digitalWrite(motorPin, HIGH);
  }
  else
  {
    digitalWrite(motorPin, LOW); // Spin the motor 
  }
 delay(1000); 
}
   

void sendSMS(String message) {
  sim800a.println("AT+CMGF=1"); // Set the GSM module to SMS mode
  delay(100);
  sim800a.println("AT+CMGS=\"+91123456789\""); // Replace +1234567890 with the recipient's phone number
  delay(100);
  sim800a.println(message); // The content of the message
  delay(100);
  sim800a.println((char)26); // ASCII code of CTRL+Z to send the message
  delay(1000);
  sendSMS1(message);
}

void sendSMS1(String message) {
  sim800a.println("AT+CMGF=1"); // Set the GSM module to SMS mode
  delay(100);
  sim800a.println("AT+CMGS=\"+91123456789\""); // Replace +1234567890 with the recipient's phone number
  delay(100);
  sim800a.println(message); // The content of the message
  delay(100);
  sim800a.println((char)26); // ASCII code of CTRL+Z to send the message
  delay(1000);
}

// Function to display GPS location
void displayGPSLocation() {
  // Print the GPS location to the serial monitor
  Serial.println(url);
}

