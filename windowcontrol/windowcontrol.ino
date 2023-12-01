// This programme is setted for a simple windows control and need two heat resistances to test the 
#include <Servo.h>
Servo myservo;

#define led_red_pin 4 // when auto if off is red
#define led_green_pin 5 // when auto on is green
#define botton_mode 2 // botton of start/stop auto mode
#define botton_change 3 // botton of switch window status
#define connection_In A2 // connection in with main module
#define connection_Out A3 // connection out 

int pos = 0;
int mode = 0; // 0=auto mode, 1=manual mode
int windowState = 0; // closed window
int changeFlagWindow = 3;
int changeFlagMode = 3;

float voltage(int input) {
  return input * (3.3 / 1023);
}

// constant of temp
float R1 = 10;
float Vsupply = 3.3;
int beta = 3435;
float T00 = 273.0;
float R0 = 27.7;

void ConnectionRead() {
  int information = analogRead(connection_In);
  switch(information) {
    case 0: { break; }
    case 10: {
      closewindow();
      Automode_Off();
    } // remote close window
    case 20: {
      openwindow();
      Automode_Off();
    } // remote open window
    case 30: {
      Automode_On();
    } // re-open auto mode
    default: break;
  }
}

void ConnectionSend() {
  if(mode == changeFlagMode && windowState == changeFlagWindow) {
    analogWrite(connection_Out, 0);
    return;
  }
  if(mode == 0) {
    if(windowState == 1) {
      analogWrite(connection_Out, 10);
      changeFlagMode = 0;
      changeFlagWindow = 1;
    } // automode on, window opened
    if(windowState == 0) {
      analogWrite(connection_Out, 20);
      changeFlagMode = 0;
      changeFlagWindow = 0;
    } // automode on,window close
  } else {
    if(windowState == 1) {
      analogWrite(connection_Out, 30);
      changeFlagMode = 1;
      changeFlagWindow = 1;
    } // automode off,window open
    if(windowState == 0) {
      analogWrite(connection_Out, 40);
      changeFlagMode = 1;
      changeFlagWindow = 0;
    } // automode off,window close
  }
}

void initWindow() {
  myservo.write(0);
}

void openwindow() {
  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
    windowState = 1;                 // set the state
  }
}

void closewindow() {  
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
    windowState = 0;                 // set the state
  }
}

void Automode_On() {
  mode = 0; 
  digitalWrite(led_green_pin, HIGH);
  digitalWrite(led_red_pin, LOW);
}

void Automode_Off() {
  mode = 1; 
  digitalWrite(led_red_pin, HIGH);
  digitalWrite(led_green_pin, LOW);
}

void switchMode() {
  Serial.print("press Mode");
  if(mode == 0) Automode_Off();
  else if(mode == 1) Automode_On();
}

void changeState(){
  Serial.print("change state");
  if(windowState == 0) openwindow();
  else closewindow();
  Serial.print(windowState);
}

void setup() {
  // put your setup code here, to run once:
  myservo.attach(9); // attaches the servo on pin 9 to the servo object
  pinMode(botton_mode, INPUT);
  pinMode(botton_change, INPUT);
  pinMode(led_red_pin, OUTPUT);
  pinMode(led_green_pin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(botton_mode), switchMode, RISING);
  attachInterrupt(digitalPinToInterrupt(botton_change), changeState, RISING);
  initWindow(); // set the window to close at the first place 
  Serial.begin(9600);
  Automode_On();
}

void loop() {
  int sensorOut = analogRead(A0); // read from outside;
  int sensorIn = analogRead(A1); // read from inside;

  float RsensorOut = (voltage(sensorOut) * R1) / (Vsupply - voltage(sensorOut));
  float RsensorIn = (voltage(sensorIn) * R1) / (Vsupply - voltage(sensorIn));
  float TempOut = ((T00*beta) / (beta - T00 * (log(R0 / RsensorOut))) - 273);
  float TempIn = ((T00*beta) / (beta - T00 * (log(R0 / RsensorIn))) - 273);

  Serial.print(TempOut);
  Serial.print(" ");
  Serial.println(TempIn);
  if(mode == 0) {
    if((TempIn >= 26.0) && (TempOut < TempIn)) openwindow();
    if((TempIn <= 20.0) && (TempOut < TempIn)) closewindow();
  }
  delay(10000);
  ConnectionRead();
  ConnectionSend(); // auto mode
}
