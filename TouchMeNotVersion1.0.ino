#include <Servo.h>

//pin numbers: 

Servo servoMotor; //servo motor  purpose: hids the on/off button

//Pins
int redLed = 13, greenLed, blueLed; //RGBLeds purpose: visual cues
int button1 = 9, button2 = 10, button3 = 11, button4 = 12; //combination buttons purpose: for passcode
int tiltSensor1 = 4, tiltSensor2 = 5, tiltSensor3 = 6, tiltSensorCenter; //tilt sensors purpose: to trigger alarm when tilted
//ultra sonic sensor  purpose: to trigger alarm with assigned distance
int ussTrig = 7;
int ussEcho = 8; 
int alarmBuzzer = 2; //alarm buzzer  purpose: creates the sound when sensors are triggered
int battery; //battery  pupose: power supply


//-------------------------------------------------------------------------------------------------------------------------------------------------
  int assignedTS1; 
  int assignedTS2;
  int assignedTS3;
  int assignedTSC;
  int assignedUSS;
  bool on;

//for passcode
  int passcode[] = {1, 2, 3, 4}; // Default passcode sequence
  int enteredCode[4]; // Array to store entered code
  int CODE[4];
  int currentIndex = 0; // Current index in enteredCode array
  bool passcodeEnteredCorrectly = false;
  int uss;

//------------------------------------------------------------------------------------------------------------------------------------------SETUP--
void setup() {
  soundIndicator(2); //pupose: to indicate that this device is on
  servoMotor.attach(3); //temporary number
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(tiltSensor1, INPUT_PULLUP);
  pinMode(tiltSensor2, INPUT_PULLUP);
  pinMode(tiltSensor3, INPUT_PULLUP);
  pinMode(tiltSensorCenter, INPUT_PULLUP);
  pinMode(ussTrig, OUTPUT);
  pinMode(ussEcho, INPUT);

  delay(5000);
  on = true; //purpose: indicates that the device is on (used on leds and alarm)
  panelCloser(on); //purpose: opens the panel at start
                   // inig on, diri mo sulod ang initial values sa Tilt Sensor
  assignedTS1 = digitalRead(tiltSensor1); //tilt sensor 1's  assigned value
  assignedTS2 = digitalRead(tiltSensor2); //tilt sensor 2's  assigned value
  assignedTS3 = digitalRead(tiltSensor3); //tilt sensor 3's  assigned value
  assignedUSS = distanceCalculator(); //ultrasonic sensor assigned value

  Serial.begin(750);

  Serial.println("--------------------------------------");
  Serial.println("Touch Me Not (Shame Plant)    ver.1.0"); //project name and version
  Serial.println("an \"INOGAMI\" creation (May-21-2024)"); //creator
  Serial.println("--------------------------------------\n");
  Serial.println("Note: the default passcode is always  1 2 3 4\n");
}

//--------------------------------------------------------------------------------------------------------------------------------------------LOOP--
void loop() {
  int ts1 = digitalRead(tiltSensor1); //tilt sensor 1
  int ts2 = digitalRead(tiltSensor2); //tilt sensor 2
  int ts3 = digitalRead(tiltSensor3); //tilt sensor 3
  // uss = distanceCalculator();// 1003; //ultrasonic sensor in default cm, if inches 393 sya

  // int equivalent1 = uss-assignedUSS;
  // int equivalent2 = assignedUSS-uss;

  // Serial.print("\nUss - assignedUSS: ");
  // Serial.println(equivalent1);
  // Serial.print("AssignedUSS - uss : ");
  // Serial.println(equivalent2);

  // if(equivalent1 > 10){ //rechecker for miscalculation
  //   uss = distanceCalculator();
  // }
  // if(equivalent2 < 0){ //rechecker for miscalculation
  //   assignedUSS = distanceCalculator();
  // }

  // if(uss > 100){ //rechecker for miscalculation
  //     assignedUSS = distanceCalculator();
  //     uss = assignedUSS;
  // }

  // Serial.print("uss: ");
  // Serial.println(uss);
  // Serial.print("Assigned USS: ");
  // Serial.println(assignedUSS);
                                                                                                          // tolerance -7               tolerance +7
                                                                                                          //|| (uss < (assignedUSS -7)) || (uss > (assignedUSS +7)
  if((ts1 != assignedTS1) || (ts2 != assignedTS2)||(ts2 != assignedTS2) || (ts3 != assignedTS3) ){
    alarm();
    setRGBColor(255, 0, 0);
  }

  passcodeChecker();

}
//-------------------------------------------------------------------------------------------------------------------------------------------------


void alarm(){
  if(on){
    digitalWrite(alarmBuzzer, HIGH);
    setRGBColor(255, 0, 0);
  }else if(on == false){
    digitalWrite(alarmBuzzer, LOW);
    setRGBColor(0, 0, 0);
  }
}

void setRGBColor(int R, int G, int B){
  analogWrite(redLed, R);
  analogWrite(greenLed, G);
  analogWrite(blueLed, B);
}

int distanceCalculator(){
  long duration, cm;

  digitalWrite(ussTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(ussTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ussTrig, LOW);

  duration = pulseIn(ussEcho, HIGH);

  cm = duration * 0.034/ 2;

  return cm;
}

void panelCloser(bool turnedOn){
  if(!turnedOn){  //closes the panel when turned on
    servoMotor.write(180);
    // soundIndicator(1);
  }else if(!turnedOn == false){  //opens the panel when passCode is entered
    servoMotor.write(0);
    // soundIndicator(1);
  }
}

void passcodeChanger(){
  delay(500);
  bool allButtonsPressed  = false;
  if(digitalRead(button1) == HIGH && passcodeEnteredCorrectly){
    allButtonsPressed = true;
  }
  Serial.print("Button 1 pressed?: ");
  Serial.println(digitalRead(button1));
  Serial.print("All buttons pressed: ");
  Serial.println(allButtonsPressed);
  Serial.print("Passcode entered correctly: ");
  Serial.println(passcodeEnteredCorrectly);

  if (allButtonsPressed) { // changing passcode
    soundIndicator(3);
    delay(2000);
    Serial.println("\nPlease, enter new combination of passcode...");
    for (int index = 0; index < 4; index++) {
      Serial.print("Digit ");
      Serial.print(index+1);
      Serial.print(" : ");
      CODE[index] = readButton();
      Serial.println(CODE[index]);
      delay(500);
    }

    for(int rounds=0; rounds<2; rounds++){ 
        setRGBColor(0,255,0);
        delay(500);
        setRGBColor(0,255,0);
        delay(150);
      }

    if(currentIndex == 4){
      Serial.println("\nWait, changing the pass...");
      for(int index = 0; index < 4; index++){
        soundIndicator(1);
        passcode[index] = CODE[index];
        Serial.println(passcode[index]);
      }
      delay(1200);
      passcodeEnteredCorrectly = false;
      // soundIndicator(2);
      digitalWrite(alarmBuzzer, HIGH);
      delay(500);
      digitalWrite(alarmBuzzer, LOW);
      Serial.println("The passcode has been changed..");
      on = true;
      panelCloser(on);
    }

    currentIndex = 0;
  }
}

int readButton() {
  soundIndicator(1);
  while (true) {
    if (digitalRead(button1) == HIGH) return 1;
    if (digitalRead(button2) == HIGH) return 2;
    if (digitalRead(button3) == HIGH) return 3;
    if (digitalRead(button4) == HIGH) return 4;
  }
}

void passcodeChecker(){ //checks the passcode
  if (digitalRead(button1) == HIGH) {
    delay(50); // Debounce delay
    enteredCode[currentIndex] = 1; //new changes here from version 0.9 / not yet tested
    currentIndex++; // Move to next index
    Serial.println("1");
    delay(500);
  } else if (digitalRead(button2) == HIGH) {
    delay(50);
    enteredCode[currentIndex] = 2; //new changes here from version 0.9 / not yet tested
    currentIndex++;
    Serial.println("2");
    delay(500);
  } else if (digitalRead(button3) == HIGH) {
    delay(50);
    enteredCode[currentIndex] = 3; //new changes here from version 0.9 / not yet tested
    currentIndex++;
    Serial.println("3");
    delay(500);
  } else if (digitalRead(button4) == HIGH) {
    delay(50);
    enteredCode[currentIndex] = 4; //new changes here from version 0.9 / not yet tested
    currentIndex++;
    Serial.println("4");
    delay(500);
  }
  
  // Check if full passcode is entered
  if (currentIndex == 4) {
    // Compare entered code to passcode
    bool correct = true;
    for (int i=0; i<4; i++) {
      Serial.print(i+1);
      Serial.print(" Passcode: ");
      Serial.println(passcode[i]);
      if (enteredCode[i] != passcode[i]) {
        correct = false;
        passcodeEnteredCorrectly = false;
        break;
      }
    }
    
    // Output result
    if (correct) {
      int temporaryOff = on; //pupose: just for turning off the alarm
      Serial.println("Access granted!");
      on = false; // to turn off the alarm
      panelCloser(on);
      noTone(alarmBuzzer); //palong sa alarm
      on = temporaryOff;
      passcodeEnteredCorrectly = true;
      Serial.print("Passcode entered correctly: ");
      Serial.println(passcodeEnteredCorrectly);

      soundIndicator(1);
      setRGBColor(0, 255, 0);
      if(passcodeEnteredCorrectly){
        passcodeChanger();
      }
      restarter();
    } else {
      Serial.println("Access denied!");
      soundIndicator(3);
      alarm();
      // setRGBColor(255, 0, 0);
    }
    
    // Reset variables for next attempt
    currentIndex = 0;
    delay(1000); // Delay before resetting
    Serial.flush(); // Clear serial buffer
  }
}


void restarter(){
  // digitalRead(button)
  bool toOnAgain = false;
  bool wait = false;

  if(digitalRead(button1) == HIGH){
    toOnAgain = true;
  }

  // Serial.print(digitalRead(button1) == HIGH); 
  delay(1000);

  if(on && toOnAgain){
  
    delay(7000); //ample time for the user to set up the device again

    assignedTS1 = digitalRead(tiltSensor1); //tilt sensor 1's  assigned value
    assignedTS2 = digitalRead(tiltSensor2); //tilt sensor 2's  assigned value
    assignedTS3 = digitalRead(tiltSensor3); //tilt sensor 3's  assigned value
    // assignedUSS = distanceCalculator();

    // if(uss > 100){ //rechecker for misscalculation
    //   assignedUSS = distanceCalculator();
    //   uss = assignedUSS;
    //   Serial.print("Ran once");
    // }
    
    panelCloser(on);
  }else{
    wait = true;
  }

  while(wait){
    Serial.println("waiting to be restarted..."); //a pause

    if(digitalRead(button1) == HIGH){
      toOnAgain = true;
      wait = false;
    }

    // Serial.print(digitalRead(button1) == HIGH); 
    delay(1000);

    if(on && toOnAgain){
    
      //delay(7000); //ample time for the user to set up the device again

      assignedTS1 = digitalRead(tiltSensor1); //tilt sensor 1's  assigned value
      assignedTS2 = digitalRead(tiltSensor2); //tilt sensor 2's  assigned value
      assignedTS3 = digitalRead(tiltSensor3); //tilt sensor 3's  assigned value
      // assignedUSS = distanceCalculator();

      // if(uss > 100){ //rechecker for misscalculation
      //   assignedUSS = distanceCalculator();
      //   uss = assignedUSS;
      //   Serial.print("Ran once");
      // }
      
      panelCloser(on);
    }
  }
}

void soundIndicator(int nRepetitions){
  for(int i=0; i<nRepetitions; i++){
    digitalWrite(alarmBuzzer, HIGH);
    delay(100);
    digitalWrite(alarmBuzzer, LOW);
    delay(100);
  }
}