#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 3, 2);
float FLOW_RATE = 0;  //Current flow rate, mL/Min
float MAX_FLOW_RATE = 5; //Max flow rate of syringe, mL/min
float VOLUME = 20; //Max readable volume of syringe, mL
float LENGTH = 70; //Length of syringe chamber when filled to max volume, mm

int reverse_button = 4;
int forward_button = 5;
int MS3_pin = 6;
int MS2_pin = 7;
int MS1_pin = 8;
int red_LED = 9;
int green_LED = 10;
int blue_LED = 11;
int limit = 12;
int power_button = 13;
int potPin = A3;

//Increment of flow rate per value of potentiometer,
//calculated by dividing the max flow rate by the max value of pot (1023)
float flow_rate_inc = MAX_FLOW_RATE/1023;
float lead = 2; //Lead of lead screw, mm
float steps_rev = 200; //Steps in full revolution of stepper motor, steps
int speed; //Var to hold calculated speed for desired flow rate
int const_speed = 4000; //Motor speed when forward and reverse buttons pressed
int potVal = 0; //Var to hold value output by potentiometer
int k = 5; //Band tolerance around pot value 
//(When pot vlaue changes by more than this, flow rate will be updated)

void setColor(int R, int G, int B) {
  analogWrite(red_LED, R);
  analogWrite(green_LED, G);
  analogWrite(blue_LED, B);
}

void setMicrostep(bool MS1, bool MS2, bool MS3) {
  if(MS1) {
    digitalWrite(MS1_pin, HIGH);
  }
  if(MS2) {
    digitalWrite(MS2_pin, HIGH);
  }
  if(MS3) {
    digitalWrite(MS3_pin, HIGH);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  stepper.setMaxSpeed(4000);
  stepper.setSpeed(100);
  setMicrostep(0, 1, 0);

  //Configure Pins
  pinMode(reverse_button, INPUT_PULLUP);
  pinMode(forward_button, INPUT_PULLUP);
  pinMode(MS1_pin, OUTPUT);
  pinMode(MS2_pin, OUTPUT);
  pinMode(MS3_pin, OUTPUT);
  pinMode(red_LED, OUTPUT);
  pinMode(green_LED, OUTPUT);
  pinMode(blue_LED, OUTPUT);
  pinMode(limit, INPUT_PULLUP);
  pinMode(power_button, INPUT_PULLUP);
}

void loop() {
  int newPotVal = analogRead(potPin);

  if(newPotVal < (potVal - k) || newPotVal > (potVal + k)) {
    potVal = newPotVal;
    FLOW_RATE = flow_rate_inc * potVal;
    Serial.print("Flow rate (mL/min): ");
    Serial.println(FLOW_RATE);
    speed = 16 * FLOW_RATE/((VOLUME/LENGTH)*(lead/steps_rev)*60);
  }
  
  //If reverse pressed, set color to blue and reverse
  if(digitalRead(reverse_button) == LOW) {
    //Reverse button pressed
    setColor(0, 0, 255);
    setMicrostep(0,1,0);
    stepper.setSpeed(-const_speed);
    stepper.runSpeed();
  } else if(digitalRead(limit) == HIGH) {
    //Limit reached, do not allow forwawrd motion, set LED to red
    setColor(255, 0 ,0);
  } else if(digitalRead(power_button) == LOW) {
    //Power on, run pump and set LED to green
    setColor(0, 255, 0);
    setMicrostep(1, 1, 1); //Microstep 1/16
    stepper.setSpeed(speed);
    stepper.runSpeed();
  } else {
    setColor(255, 80, 0);
    if(digitalRead(forward_button) == LOW) {
      //Forward button pressed
      setColor(255, 0, 255);
      setMicrostep(0, 1, 0); //Microstep 1/4
      stepper.setSpeed(const_speed);
      stepper.runSpeed();
    }
  }
}
