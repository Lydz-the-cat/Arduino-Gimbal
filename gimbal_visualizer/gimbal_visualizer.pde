/*
Lydia Chan
04/30/2026
ENME351 - Lab 9
*/

import processing.serial.*;
Serial myPort;

void setup() {
 size(800, 600, P3D);  // Setting up window with 3D rendering mode
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
     // Saving roll, pitch, and yaw angles from serial monitor
     // General convention I made: up & right = positive, down & left = negative
     float roll = -1 * float(rpyRaw[0]);
     float pitch = float(rpyRaw[1]);
     float yaw = -1 * float(rpyRaw[2]);
     
     float[] pos = {roll, pitch, yaw};
     
     background(200);
     
     pushMatrix(); // Saves current coordinate system, since I will translate and rotate it to move a rectangular prism around
     // (Source: https://processing.org/tutorials/transform2d)
     
     // Creating a rectangular prism in center of screen to represent the current angular position of the IMU (NOT the platform)
     translate(400, 250, 0);
     // Rotating prism based on roll, pitch, and yaw angles obtained from serial monitors
     // I determined which rotate function corresponds to which direction from experimentation
     rotateX(radians(pitch));
     rotateY(radians(yaw));
     rotateZ(radians(roll));
     lights();
     // Making rectangle green if the IMU is angled within 10° of its starting position
     if (abs(roll) <= 10.0 && abs(pitch) <= 10.0 && abs(yaw) <= 10.0){
       fill(0,255,0);
     }
     // Red if otherwise, to indicate IMU being severely tilted
     else {
       fill(255,0,0);
     }
     box(300, 40, 200);
     popMatrix(); // Restores original coordinate system so that my text isn't moving with the rectangle!
     
     // Displaying IMU position angles below the rectangle
     textSize(35);
     fill(0,0,0);
     text("Roll:", 150, 450);
     text("Pitch:", 350, 450);
     text("Yaw:", 550, 450);
     
     // Making text green for each position angle within 10° from starting position, red otherwise
     for (int i=0; i<3; i++) {
       if (abs(pos[i]) <= 10.0) {
         fill(68, 160, 72);
       } else {
         fill(255, 0, 0);
       }
       text(str(pos[i]) + "°", 150 + 200*i, 500);
     }
    }
  }
 }
}
