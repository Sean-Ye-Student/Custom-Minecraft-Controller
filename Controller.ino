int rightJoystickPins[3] = {7, A0, A1}; //Button, XVAL, YVAL
int leftJoystickPins[3] = {8, A2, A3}; //Button, XVAL, YVAL

//These are for the ultrasonicsensor https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6
#define echoPin 12 
#define trigPin 13

const int tiltSensorPin = A4;
const int leftButtonPin = A5; //This is the second button that is not part of the joystick
void setup() {
  Serial.begin(2000000);
  // put your setup code here, to run once:
  for(int i=0; i < sizeof(rightJoystickPins)/sizeof(int); i++){
    pinMode(rightJoystickPins[i], INPUT);
  }

  for(int i=0; i<sizeof(leftJoystickPins)/sizeof(int); i++){
    pinMode(leftJoystickPins[i], INPUT);
  }

  digitalWrite(rightJoystickPins[0], HIGH);
  digitalWrite(leftJoystickPins[0], HIGH);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  pinMode(tiltSensorPin, INPUT);
  digitalWrite(tiltSensorPin, HIGH);

  pinMode(leftButtonPin, INPUT);
  digitalWrite(leftButtonPin, LOW);
  delay(5);
}

const int defaultShakingDelayInterval = 300;
const int shakingDelayIncrement = 10;
const int maxShakingDelayInterval = 700;
int shakingDelayInterval = defaultShakingDelayInterval;
long lastSent = millis();
bool previousState = false;
bool shaking = false;
bool currentState;
int tiltReading;
//Note WriteShake must be first as the value if not recieved must be defaulted to zero in python, this is the same for the joystick button which is why write joystick is second
void WriteShake(){ //Also includes the secondary button as the functionality is the same
  tiltReading = analogRead(tiltSensorPin);
  currentState = false;
  if (tiltReading > 800){
    currentState = true;
  }
  
  if (previousState != currentState){
    shaking = (millis() - lastSent) < shakingDelayInterval;
    lastSent = millis();
  }else if(millis() - lastSent >= shakingDelayInterval){
    shaking = false;
  }
  previousState = currentState; 
  Serial.write(shaking + 1);
}

bool leftButtonPressed = 0;
int m = 1000;
void WriteLeftButton(){
  leftButtonPressed = analogRead(leftButtonPin) == 1023;
  if(shaking){
    Serial.write(0 + 1);
  }else{
    Serial.write(leftButtonPressed + 1);
  }
  
}

//Above 100 for button will be invalid as press
//Below 75 is concidered a press

int getJoystickDataValue(int val){
  if (val < 23){
    return 0 + 1;
  }else if (val > 1000){
    return 2 + 1;
  }

  return 1 + 1;
}

void WriteJoystick(){
  int leftButton = digitalRead(leftJoystickPins[0]);
  int leftX = analogRead(leftJoystickPins[1]);
  int leftY = analogRead(leftJoystickPins[2]);
  
  int rightButton = digitalRead(rightJoystickPins[0]);
  int rightX = analogRead(rightJoystickPins[1]);
  int rightY = analogRead(rightJoystickPins[2]);
  
  if (leftButton == LOW and rightButton == LOW){
    Serial.write(3+1); //Buttons are low when they are pressed lol
  }else if (leftButton == HIGH and rightButton == HIGH){
    Serial.write(0+1); //Must increment so the bytes as zero won't get messed up 
  }else if (leftButton == HIGH and rightButton == LOW){
    Serial.write(2+1); //right button is being pressed
  }else{
    Serial.write(1+1); 
  }
  
  Serial.write(getJoystickDataValue(leftX));
  Serial.write(getJoystickDataValue(leftY));
  Serial.write(map(rightX, 0, 1023, 0, 127));
  Serial.write(map(rightY, 0, 1023, 0, 127));
}


int lastTrigPinToggleMode = 0;
int lastTrigPinToggle = -1;
long duration;
int distance = -1;
void WriteUltraSonicSensor(){
   if (lastTrigPinToggle == -1){
    digitalWrite(trigPin, LOW);
    lastTrigPinToggle = millis();
  }

  if (millis() - lastTrigPinToggle > 2 && lastTrigPinToggleMode == 0){
    digitalWrite(trigPin, HIGH);
    lastTrigPinToggle = millis();
    lastTrigPinToggleMode = 1;
  }else if (millis() - lastTrigPinToggle > 10 && lastTrigPinToggleMode == 1){
    digitalWrite(trigPin, LOW);
    lastTrigPinToggle = -1;
    lastTrigPinToggleMode = 0;
    duration = pulseIn(echoPin, HIGH); 
    distance = duration * 0.034 / 2;
  
  }

  //Serial.println(distance); //In centimeters will be -1 by default
  if(distance > 95){
    Serial.write(2 + 1); //jumping
  }else if(distance < 10){
    Serial.write(0 + 1); //crouching
  }else{
    Serial.write(1 + 1); //neutral stance
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  WriteShake();
  WriteLeftButton();
  WriteJoystick();
  WriteUltraSonicSensor();
  Serial.write("\n");
}
