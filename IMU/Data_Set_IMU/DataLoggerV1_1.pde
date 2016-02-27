// I2C device class (I2Cdev) demonstration Processing sketch for MPU6050 DMP output
// 6/20/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-06-20 - initial release

/* ============================================
 I2Cdev device library code is placed under the MIT license
 Copyright (c) 2012 Jeff Rowberg
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ===============================================
 */

import processing.serial.*;
import processing.opengl.*;
import toxi.geom.*;
import toxi.processing.*;

// NOTE: requires ToxicLibs to be installed in order to run properly.
// 1. Download from http://toxiclibs.org/downloads
// 2. Extract into [userdir]/Processing/libraries
//    (location may be different on Mac/Linux)
// 3. Run and bask in awesomeness

ToxiclibsSupport gfx;
PrintWriter write_csv;
Serial port;                         // The serial port
char[] teapotPacket = new char[22];  // InvenSense Teapot packet
char a1 = 0x00;
int serialCount = 0;                 // current packet byte position
int aligned = 0;
int interval = 0;
float[] q = new float[4];
Quaternion quat = new Quaternion(1, 0, 0, 0);
byte[] real_acc = new byte[6];
float a, b, c, d, e, f;

void setup()
{
  // 300px square viewport using OpenGL rendering
  size(300, 300, OPENGL);
  gfx = new ToxiclibsSupport(this);

  // se tup lights and antialiasing
  lights();
  smooth();

  String portName = "/dev/tty.usbserial-A9S7ZL5T";
  // open the serial port
  port = new Serial(this, portName, 115200);
  // send single character to trigger DMP init/startq
  // (expected by MPU6050_DMP6 example Arduino sketch)
  port.write('r');
  // file write 
  write_csv = createWriter("Acc-elevation-20cm.csv");
}


void draw() 
{
  if (millis() - interval > 100) 
  {
    // resend single character to trigger DMP init/start
    // in case the MPU is halted/reset while applet is running
    port.write('r'); 
    interval = millis();
  }
  // black background
  background(0);
  // translate everything to the middle of the viewport
  pushMatrix();
  translate(width / 2, height / 2);

  // 3-step rotation from yaw/pitch/roll angles (gimbal lock!)
  // ...and other weirdness I haven't figured out yet
  //rotateY(-ypr[0]);
  //rotateZ(-ypr[1]);
  //rotateX(-ypr[2]);

  // toxiclibs direct angle/axis rotation from quaternion (NO gimbal lock!)
  // (axis order [1, 3, 2] and inversion [-1, +1, +1] is a consequence of
  // different coordinate system orientation assumptions between Processing
  // and InvenSense DMP)
  float[] axis = quat.toAxisAngle();
  rotate(axis[0], -axis[1], axis[3], axis[2]);

  // draw main body in red
  fill(255, 0, 0, 200);
  box(10, 10, 200);
  // draw front-facing tip in blue
  fill(0, 0, 255, 200);
  
  pushMatrix();
  translate(0, 0, -120);
  rotateX(PI/2);
  drawCylinder(0, 20, 20, 8);
  popMatrix();

  // draw wings and tail fin in green
  fill(0, 255, 0, 200);
  beginShape(TRIANGLES);
  vertex(-100, 2, 30); 
  vertex(0, 2, -80); 
  vertex(100, 2, 30);  // wing top layer
  vertex(-100, -2, 30); 
  vertex(0, -2, -80); 
  vertex(100, -2, 30);  // wing bottom layer
  vertex(-2, 0, 98); 
  vertex(-2, -30, 98); 
  vertex(-2, 0, 70);  // tail left layer
  vertex( 2, 0, 98); 
  vertex( 2, -30, 98); 
  vertex( 2, 0, 70);  // tail right layer
  endShape();
  beginShape(QUADS);
  vertex(-100, 2, 30); 
  vertex(-100, -2, 30); 
  vertex(  0, -2, -80); 
  vertex(  0, 2, -80);
  vertex( 100, 2, 30); 
  vertex( 100, -2, 30); 
  vertex(  0, -2, -80); 
  vertex(  0, 2, -80);
  vertex(-100, 2, 30); 
  vertex(-100, -2, 30); 
  vertex(100, -2, 30); 
  vertex(100, 2, 30);
  vertex(-2, 0, 98); 
  vertex(2, 0, 98); 
  vertex(2, -30, 98); 
  vertex(-2, -30, 98);
  vertex(-2, 0, 98); 
  vertex(2, 0, 98); 
  vertex(2, 0, 70); 
  vertex(-2, 0, 70);
  vertex(-2, -30, 98); 
  vertex(2, -30, 98); 
  vertex(2, 0, 70); 
  vertex(-2, 0, 70);
  endShape();
  popMatrix();
}

void serialEvent(Serial port) 
{
  interval = millis();
  while (port.available () > 0)
  {
    int ch = port.read();
    //print((char)ch);
    if (ch == '$') 
    {
      serialCount = 0;
    } // this will help with alignment

    if (aligned < 4) 
    {
      // make sure we are properly aligned on a 22-byte packet
      if (serialCount == 0) 
      {
        if (ch == '$') aligned++; 
        else aligned = 0;
      } 
      else if (serialCount == 1) 
      {
        if (ch == 2) aligned++; 
        else aligned = 0;
      } 
      else if (serialCount == 20) 
      {
        if (ch == '\r') aligned++; 
        else aligned = 0;
      }
      else if (serialCount == 21)
      {
        if (ch == '\n') aligned++; 
        else aligned = 0;
      }
      //println(ch + " " + aligned + " " + serialCount);
      serialCount++;
      if (serialCount == 22) serialCount = 0;
    } 
    else 
    {
      if (serialCount > 0 || ch == '$') 
      {
        teapotPacket[serialCount++] = (char)ch;
        if (serialCount == 22) 
        {
          serialCount = 0; // restart packet byte position

          // get quaternion from data packet
          q[0] = ((teapotPacket[2] << 8) | teapotPacket[3]) / 16384.0f;
          q[1] = ((teapotPacket[4] << 8) | teapotPacket[5]) / 16384.0f;
          q[2] = ((teapotPacket[6] << 8) | teapotPacket[7]) / 16384.0f;
          q[3] = ((teapotPacket[8] << 8) | teapotPacket[9]) / 16384.0f;    
          // set our toxilibs quaternion to new data
          for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i]; 
          quat.set(q[0], q[1], q[2], q[3]);

          real_acc[0] = byte(teapotPacket[10]);
          real_acc[1] = byte(teapotPacket[11]);
          real_acc[2] = byte(teapotPacket[12]);
          real_acc[3] = byte(teapotPacket[13]);
          real_acc[4] = byte(teapotPacket[14]);
          real_acc[5] = byte(teapotPacket[15]);

		  long intervalDur = (long)((teapotPacket[19] << 24) | (teapotPacket[18] << 16) | (teapotPacket[17] << 8) | teapotPacket[16]);
          write_csv.println(intervalDur + "," + q[0] + "," + q[1] + "," + q[2] + "," + q[3] + "," + real_acc[0] + "," + real_acc[1] + "," + real_acc[2]+ "," + real_acc[3] + "," + real_acc[4] + "," + real_acc[5]);          

        }
      }
    }
  }
}


void drawCylinder(float topRadius, float bottomRadius, float tall, int sides) 
{
  float angle = 0;
  float angleIncrement = TWO_PI / sides;
  beginShape(QUAD_STRIP);
  for (int i = 0; i < sides + 1; ++i) {
    vertex(topRadius*cos(angle), 0, topRadius*sin(angle));
    vertex(bottomRadius*cos(angle), tall, bottomRadius*sin(angle));
    angle += angleIncrement;
  }
  endShape();

  // If it is not a cone, draw the circular top cap
  if (topRadius != 0) {
    angle = 0;
    beginShape(TRIANGLE_FAN);

    // Center point
    vertex(0, 0, 0);
    for (int i = 0; i < sides + 1; i++) 
    {
      vertex(topRadius * cos(angle), 0, topRadius * sin(angle));
      angle += angleIncrement;
    }
    endShape();
  }
  
  // If it is not a cone, draw the circular bottom cap
  if (bottomRadius != 0) 
  {
    angle = 0;
    beginShape(TRIANGLE_FAN);
    // Center point
    vertex(0, tall, 0);
    for (int i = 0; i < sides + 1; i++) {
      vertex(bottomRadius * cos(angle), tall, bottomRadius * sin(angle));
      angle += angleIncrement;
    }
    endShape();
  }
}

void keyPressed() 
{
  if(key == 'q' || key == 'Q')
  {
    write_csv.flush(); // Writes the remaining data to the file
    write_csv.close(); // Finishes the file
    exit(); // Stops the program
  }
}

