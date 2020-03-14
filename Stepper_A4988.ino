/* Demo of A4988 micro-stepping motor driver
   Arduino Uno
   Arduino IDE 1.8.10

   Uses button debounce library  https://github.com/thomasfredericks/Bounce2

   Gadget Reboot
*/

#include <Bounce2.h>

// stepper control outputs
#define motStepPin     2
#define motDirPin      3
#define motMS1Pin      4
#define motMS2Pin      5
#define motMS3Pin      6

// button inputs
#define buttonDir      7
#define buttonRun      8
#define buttonMS       9

// speed control pot
#define speedPot       A0

// create debounce instances for 3 buttons
Bounce dirIn   = Bounce();  // motor direction toggle
Bounce runIn   = Bounce();  // motor run for 200 steps
Bounce msIn    = Bounce();  // cycle through microstep options

// list of microstep config pin outputs in binary notation B[MS1][MS2][MS3]
byte microSteps[5] = {
  B000,  // full step
  B100,  // 1/2  step
  B010,  // 1/4  step
  B110,  // 1/8  step
  B111,  // 1/16 step
};

// start with full stepping:  microSteps[0] gives MS1=0 MS2=0 MS3=0
byte msMode = 0;

void setup() {

  // assign actual input pins to debounce instances with a certain debounce time
  dirIn.attach(buttonDir, INPUT_PULLUP);
  dirIn.interval(10);
  runIn.attach(buttonRun, INPUT_PULLUP);
  runIn.interval(10);
  msIn.attach(buttonMS, INPUT_PULLUP);
  msIn.interval(10);

  // configure initial motor settings
  digitalWrite(motStepPin, LOW);
  digitalWrite(motDirPin, HIGH);
  digitalWrite(motMS1Pin, LOW);
  digitalWrite(motMS2Pin, LOW);
  digitalWrite(motMS3Pin, LOW);

  pinMode(motStepPin, OUTPUT);
  pinMode(motDirPin, OUTPUT);
  pinMode(motMS1Pin, OUTPUT);
  pinMode(motMS2Pin, OUTPUT);
  pinMode(motMS3Pin, OUTPUT);

}
void loop() {
  // update button debounce status
  dirIn.update();
  runIn.update();
  msIn.update();

  // if any buttons are pressed, perform actions
  // toggle motor direction output pin
  if (dirIn.fell()) {
    digitalWrite(motDirPin, !digitalRead(motDirPin));
  }

  // run motor for 200 steps
  if (runIn.fell()) {
    for (int i = 0; i < 200; i++) {
      // read pot and set a delay to control speed
      int motSpeed = map(analogRead(speedPot), 0, 1023, 100, 10000);  

      // advance motor one step at the pot delay setting/speed
      digitalWrite(motStepPin, HIGH);
      delayMicroseconds(motSpeed);
      digitalWrite(motStepPin, LOW);
      delayMicroseconds(motSpeed);
    }
  }

  // cycle through micro step configuration options
  if (msIn.fell()) {
    msMode++;
    if (msMode >= 4) msMode = 0;
    digitalWrite(motMS1Pin, bitRead(microSteps[msMode], 2));
    digitalWrite(motMS2Pin, bitRead(microSteps[msMode], 1));
    digitalWrite(motMS3Pin, bitRead(microSteps[msMode], 0));
  }
}
