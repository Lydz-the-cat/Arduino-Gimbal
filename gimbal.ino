/*
Lydia Chan
04/30/26
ENME351 - Lab 9 (Gimbal)
*/

// Necessary libraries for controlling BNO055 sensor
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

unsigned long tperiod = 20000; // PWM period for servos
unsigned long t = 0; // Timer variable for non-blocking delay (servo PWM)
int readings[3]; // Array for containing IMU readings
int control[3]; // Array for containing duty cycle periods for roll, pitch, and yaw

Adafruit_BNO055 bno = Adafruit_BNO055(55); // Initializing BNO055 sensor (from BNO055 position example)

void setup() {
  Serial.begin(115200); // Initializing serial output

  // Checking to see if BNO055 is actually connected
  if(!bno.begin()) {
    Serial.println("No BNO055 detected. Check wiring!");
    while(1);
  }

  pinMode(5, OUTPUT); // Roll servo pin
  pinMode(6, OUTPUT); // Pitch servo pin
  pinMode(7, OUTPUT); // Yaw servo pin

  /*
  In the following line, I'm changing the BNO055's mode from NDOF to IMUPLUS. By default, the sensor calculates its angular position using the accelerometer, gyroscope, and magnetometer.
  However, the magnetometer takes a while after powering on to find magnetic North, and once it does, the BNO055 changes its reference origin to align with magnetic North. This was messing up
  my yaw readings, because I only want my sensor readings to be relative to its position when I power it on. In IMUPLUS mode, it only calculates its angular position from accelerometer
  and gyroscope data and ignores the magnetometer.
  */
  bno.setMode(OPERATION_MODE_IMUPLUS);

  bno.setExtCrystalUse(true); // Using the Arduino's crystal for timing instead of the BNO055's - probably more stable/precise for its euler angle calculations
}

void loop() {
  // "Manual" servo PWM control code
  if (micros() - t >= tperiod){
    t = micros();

    readIMU(readings); // See readIMU function below

    for (int i=0; i<3; i++) {
      // Mapping IMU readings to PWM duty cycle periods
      control[i] = map(readings[i], 0, 1800, 600, 2600);
      
      noInterrupts(); // Disables background tasks; I found this trick in an online forum because without it, my servos would constantly shudder
      digitalWrite(i+5, HIGH);
      delayMicroseconds(control[i]); // Keep servo pin on for mapped duty cycle period
      digitalWrite(i+5, LOW);
      interrupts(); // Re-enables background tasks
    }
  }
}

void readIMU(int pos[3]) {
  sensors_event_t event;
  bno.getEvent(&event); // Taking a reading from the sensor in the form of variable type "sensors_event_t"
  // Also found from BNO055 position example

  /*
  The default orientation data for x, y, and z is outputted in Euler angles.
  For the orientation of my sensor on the gimbal device: y = roll, z = pitch, and x = yaw. 
  According to the BNO055 datasheet (from Bosch): roll is outputted from -90° to 90°, pitch is from -180° to 180°, and yaw is from 0° to 360°.
  The servos I'm using only have a range of 180 degrees. Thus, I need to limit the range of the pitch and yaw outputs to 180 degrees.
  For the sake of less code in my loop function, I'm also bringing all of the output values between 0° and 180°, where 90° represents
  the "origin", or the position the sensor is in when the device is powered on.
  */
  // As you might notice, I'm multiplying each sensor output by 10 to get 10ths of degrees when mapping since map() only works for ints

  int roll = (int)(event.orientation.y * 10.0);
  // Values already limited from -90 to 90, no need to limit
  Serial.print(String(roll/10.0));   // Printing to serial monitor for Processing
  // Bringing values between 0 and 180 for servo
  roll -= 900;
  roll *= -1;

  int pitch = (int)(event.orientation.z * 10.0);
  // Limiting values to -90 to 90 degrees for the servo
  if (pitch < -900) {
    pitch = -900;
  } else if (pitch > 900){
    pitch = 900;
  }
  Serial.print("\t" + String(pitch/10.0));   // Printing to serial monitor for Processing
  // Bringing values between 0 and 180 for servo
  pitch += 900;

  int yaw = (int)(event.orientation.x * 10.0);
  // Adjusting yaw values to be between -90 and 180 degrees
  if (yaw > 1800) {
    yaw -= 3600;
  }
  // Limiting values to -90 to 90, then 0-180 degrees for the servo
  if (yaw < -900) {
    yaw = -900;
  } else if (yaw > 900){
    yaw = 900;
  }
  Serial.print("\t" + String(yaw/10.0));   // Printing to serial monitor for Processing
  // Bringing values betewen 0 and 180 for servo
  yaw += 900;

  pos[0] = roll;
  pos[1] = pitch;
  pos[2] = yaw;

  Serial.println("");
}
