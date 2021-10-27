#include <Servo.h>
#include <LiquidCrystal.h>

Servo myservo_in;
Servo myservo_out;

const unsigned int TRIG_PIN_IN = 9;
const unsigned int ECHO_PIN_IN = 8;
const unsigned int servoPin_in = 13;
const unsigned int TRIG_PIN_OUT = 7;
const unsigned int ECHO_PIN_OUT = 6;
const unsigned int servoPin_out = 10;

int pos_in = 0;
int lastPos_in = 0;
int pos_out = 0;
int lastPos_out = 0;
int slots = 3;  // Available slots
int take = 3;  // Taken slots
long lastTime_in = 0;
long lastTime_out = 0;
float distance_in = 0;
float distance_out = 0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// This function will return the distance between object and sensor
float getDistance(int TRIG_PIN, int ECHO_PIN) {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH) / 29 / 2;
}

void setup() {
  // set up the lcd's number of columns and rows:
  lcd.begin(16, 2);
  pinMode(TRIG_PIN_IN, OUTPUT);
  pinMode(ECHO_PIN_IN, INPUT);
  myservo_in.attach(servoPin_in);
  pinMode(TRIG_PIN_OUT, OUTPUT);
  pinMode(ECHO_PIN_OUT, INPUT);
  myservo_out.attach(servoPin_out);
  Serial.begin(9600);
  myservo_in.write(0);
  myservo_out.write(0);
}

void loop() {
  distance_in = getDistance(TRIG_PIN_IN, ECHO_PIN_IN);
  distance_out = getDistance(TRIG_PIN_OUT, ECHO_PIN_OUT);

  Serial.println(distance_out);
  Serial.println(distance_in);
  Serial.println("");
  lcd.setCursor(0, 0);

  if (distance_in == 0 || distance_out == 0) {
    lcd.clear();
    lcd.print("Sensor error");
    delay(1000);
    lcd.clear();
  } else {
    lcd.print("Taken:");
    lcd.setCursor(12, 0);
    lcd.print(take);
    lcd.setCursor(0, 1);
    lcd.print("Available:");
    lcd.setCursor(12, 1);
    lcd.print(slots);

    // for in
    if (distance_in <= 50) {
      pos_in = 90;
      lastTime_in = millis();
    } else {
      // setting delay for closing the door after object move out of sensor range
      if ( millis() - lastTime_in >= 5000 && pos_in == 90) {
        pos_in = 0;
      }
    }

    // If no available slot left, door will not open
    if (slots > 0) {
      if (pos_in == 0 && lastPos_in == 90) {
        slots -= 1;
        take += 1;
      }
      myservo_in.write(pos_in);
    }

    lastPos_in = pos_in;
    Serial.println(pos_in);

    // for out
    if (distance_out <= 50) {
      pos_out = 90;
      lastTime_out = millis();
    } else {
      // setting delay for closing the door after object move out of sensor range
      if ( millis() - lastTime_out >= 5000 && pos_out == 90) {
        pos_out = 0;
      }
    }
    
    // If all slots are free, door will not open, for security
    if (take > 0) {
      if (pos_out == 0 && lastPos_out == 90) {
        slots += 1;
        take -= 1;
      }
      myservo_out.write(pos_out);
    }
    lastPos_out = pos_out;
  }
    Serial.println(pos_out);
    delay(1000);    // intentionally limit the scanning cycle to prevent error
}
