/*
 ***************************************************************************
 *
 *  Basic.ino - part of sample SW for using BNO055 with Arduino
 * 
 * (C) All rights reserved by ROBERT BOSCH GMBH
 *
 * Copyright (C) 2014 Bosch Sensortec GmbH
 *
 *	This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **************************************************************************/
/*	Date: 2014/01/07
 *	 Revision: 1.2
 *
 */

#include "BNO055_support.h"		//Contains the bridge code between the API and Arduino
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXEL_PIN 23
#define NUMPIXELS 12

//The device address is set to BNO055_I2C_ADDR2 in this example. You can change this in the BNO055.h file in the code segment shown below.
// /* BNO055 I2C Address */
//#define BNO055_I2C_ADDR1                0x28
//#define BNO055_I2C_ADDR2                0x29


#define LED 2

//Pin assignments as tested on the Arduino Due.
//Vdd,Vddio : 3.3V
//GND : GND
//SDA/SCL : SDA/SCL
//PSO/PS1 : GND/GND (I2C mode)

//This structure contains the details of the BNO055 device that is connected. (Updated after initialization)
struct bno055_t myBNO;
struct bno055_euler myEulerData; //Structure to hold the Euler data

unsigned long lastTime = 0;
unsigned long lastTimeLed = 0;
float pitch = .0;
float roll = .0;
float yaw = .0;

Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() //This code is executed once
{
    pinMode(LED,OUTPUT);
    pixels.begin();

	//Initialize I2C communication
	Wire.begin();

	//Initialization of the BNO055
	BNO_Init(&myBNO); //Assigning the structure to hold information about the device

	//Configuration to NDoF mode
	bno055_set_operation_mode(OPERATION_MODE_NDOF);

	delay(1);

	//Initialize the Serial Port to view information on the Serial Monitor
	Serial.begin(115200);
    printf("Chip ID: %d\n", myBNO.chip_id);
    printf("Software Revision ID: %d\n", myBNO.sw_revision_id);
    printf("Page ID: %d\n", myBNO.page_id);
    printf("Accelerometer Revision ID: %d\n", myBNO.accel_revision_id);
    printf("Gyroscope Revision ID: %d\n", myBNO.gyro_revision_id);
    printf("Magnetometer Revision ID: %d\n", myBNO.mag_revision_id);
    printf("Bootloader Revision ID: %d\n", myBNO.bootloader_revision_id);
    printf("Device Address: %d\n", myBNO.dev_addr);
    printf("-----\n");
}

void loop() //This code is looped forever
{

  if ((millis() - lastTimeLed) >= 1000) {
    lastTimeLed = millis();
    digitalWrite(LED, !digitalRead(LED));
  }

  if ((millis() - lastTime) >= 100) //To stream at 10 Hz without using additional timers
  {
    lastTime = millis();

    bno055_read_euler_hrp(&myEulerData);			//Update Euler data into the structure

    pitch = float(myEulerData.p) / 16.00; // convert to degrees
    roll = float(myEulerData.r) / 16.00;
    yaw = float(myEulerData.h) / 16.00;


    float tot = abs(pitch);
    float fractional = tot - (int)tot;
    int decimal = (int)round(fractional*10);
    
    printf("%09d yaw=%03.2f roll=%03.2f pitch:%03.2f fractional=%f decimal=%d\n", lastTime, yaw, roll, pitch, fractional, decimal);

    for(int i=0; i<NUMPIXELS; i++) {
        if (i == decimal )
            pixels.setPixelColor(i, pixels.Color(0, 150, 0));
        else
            pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        pixels.show();
    }
  }
}
