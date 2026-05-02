/*
Lydia Chan
04/30/2026
ENME351 - Lab 9
*/

import processing.serial.*;
Serial myPort;

void setup() {
 size(1000, 600, P3D);
 printArray(Serial.list());
 myPort = new Serial(this, Serial.list()[1], 115200);
 myPort.clear();
}

void draw() {
 
 if (myPort.available() > 0) {
  String inString = myPort.readStringUntil('\n');
  if (inString != null) {
    inString = trim(inString);
    String[] rpyRaw = splitTokens(inString, "\t");
    if (rpyRaw.length == 3) {
     // Converting roll, pitch, and yaw angles to radians for rotate function
     float roll = float(rpyRaw[0]);
     float pitch = float(rpyRaw[1]);
     float yaw = float(rpyRaw[2]);
     
     background(200);
     
     // Creating a rectangular prism in center of screen to represent the current angular position of the IMU (NOT the platform)
     translate(300, 300, 0);
     // Rotating prism based on roll, pitch, and yaw angles obtained from serial monitors
     rotateX(radians(pitch));
     rotateY(radians(-1 * yaw));
     rotateZ(radians(-1 * roll));
     lights();
     
     // Making rectangle green if the IMU is angled within 10 degrees of its starting position
     if (abs(roll) <= 10.0 && abs(pitch) <= 10.0 && abs(yaw) <= 10.0){
       fill(0,255,0);
     }
     // Red if otherwise, to indicate IMU being severely tilted
     else {
       fill(255,0,0);
     }
     box(200, 40, 140);
    }
  }
 }
}

// rotateX = pitch, rotateY = yaw, rotateZ = roll (i think...)
