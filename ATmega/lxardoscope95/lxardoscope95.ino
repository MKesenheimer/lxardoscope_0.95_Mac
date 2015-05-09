/*
 *
 * This file is part of the LXARDOSCOPE package.
 *
 * LXARDOSCOPE is an Arduino based oscilloscope for Linux, using the Xforms library.
 *
 * Copyright (C) 2013 Oskar Leuthold
 * 
 * LXARDOSCOPE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LXARDOSCOPE is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with LXARDOSCOPE; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */
 
//   This program for Arduino Uno reads two channels and sends the data
//   out through the serial port in 4 bytes.
//   For synchronization purposes, the following scheme was chosen:
//   A0 data:   A09 (MSB) A08 A07 A06 A05 A04 A03 A02 A01 A00 (LSB)
//   A1 data:   A19 (MSB) A18 A17 A16 A15 A14 A13 A12 A11 A10 (LSB)
//   sent as byte 1:   1 1 1 A09 A08 A07 A06 A05
//       and byte 2:   0 1 1 A04 A03 A02 A01 A00
//       and byte 3:   0 1 1 A19 A18 A17 A16 A15
//       and byte 4:   0 1 1 A14 A13 A12 A11 A10
//
//    (This arrangement was chosen for hystorical reasons; there are
//     many other possibilities. 3 bytes would be enough, but this could
//     possibly create a nonsymmetry between the channels.
//
//
int sensorValue = 0;        // value read from the port
byte lb;
byte hb;
boolean ac1=false;        // Channel 1 AC amplifier off
boolean ac2=false;        // Channel 2 AC amplifier off

void setup() {
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  analogReference(EXTERNAL);
}

void loop() {
      //if(transmit) {
      //new, send bytes only when asked -> this prevents buffer overflows!
      if(Serial.available()) {
        //delayMicroseconds(10);
        digitalWrite(8,HIGH);
        if(ac1) sensorValue = analogRead(A2);
        else  sensorValue = analogRead(A0);           

        // shift sample by 3 bits, and select higher byte  
        hb=highByte(sensorValue<<3); 
        // set 3 most significant bits and send out
        Serial.write(hb|224); 
        // select lower byte and clear 3 most significant bits
        lb=(lowByte(sensorValue))&31;
        // set bits 5 and 6 and send out
        Serial.write(lb|96);
        //enable charge pump
        digitalWrite(8,LOW);
      
        if(ac2) sensorValue = analogRead(A3);
        else sensorValue = analogRead(A1);
        //      sensorValue = 200;
        // shift sample by 3 bits, and select higher byte 
        hb=highByte(sensorValue<<3); 
        // set bits 5 and 6 and send out
        Serial.write(hb|96); 
        // select lower byte and clear 3 most significant bits
        lb=(lowByte(sensorValue))&31;
        // set bits 5 and 6 and send out
        Serial.write(lb|96);
      }
      
      if(Serial.available()) {
        char inChar = (char)Serial.read();
        if (inChar == '1') {         // cal channel 1
          digitalWrite(9, HIGH);     // gnd1
          digitalWrite(11, LOW);     // AC1 off
          ac1=false;
        }
        else if (inChar == '2') {    // channel 1 DC & AC, low gain
          digitalWrite(9, LOW);      // gnd1
          digitalWrite(11, LOW);     // AC1 off
          ac1=false;
        }
        else if (inChar == '3') {    // channel 1 use AC amplifier
          digitalWrite(9, LOW);      // gnd1
          digitalWrite(11, HIGH);    // AC1 on
          ac1=true;
        }
        else if (inChar == '4') {    // cal channel 2
          digitalWrite(10, HIGH);    // gnd2
          digitalWrite(12, LOW);     // AC2 off
          ac2=false;
        }
        else if (inChar == '5') {    // channel 2 DC & AC, low gain
          digitalWrite(10, LOW);     // gnd2
          digitalWrite(12, LOW);     // AC2 off
          ac2=false;
        }
        else if (inChar == '6') {    // channel 2 use AC amplifier
          digitalWrite(10, LOW);     // gnd2
          digitalWrite(12, HIGH);    // AC2 on
          ac2=true;
        }
        else if (inChar == '*') {
          digitalWrite(13, HIGH);    // LED (on)
        }
        else if (inChar == '#') {
          digitalWrite(13, LOW);     // LED (off)
        }                
    }
}


