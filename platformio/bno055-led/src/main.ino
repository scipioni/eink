#include "BNO055_support.h"		//Contains the bridge code between the API and Arduino
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 12

//The device address is set to BNO055_I2C_ADDR2 in this example. You can change this in the BNO055.h file in the code segment shown below.
// /* BNO055 I2C Address */
//#define BNO055_I2C_ADDR1                0x28
//#define BNO055_I2C_ADDR2                0x29

#ifdef BOARD_DOIT
    #define LED 2
    #define BUTTON 0 
    #define NEOPIXEL_PIN 23
#endif

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

#ifdef PIXELS
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif

struct Calibration {
   float pitch;
   float roll;
   float yaw;
};

Calibration calibration = {.0, .0};

void setup() //This code is executed once
{
    pinMode(BUTTON, INPUT_PULLUP);  //Boot button

    pinMode(LED,OUTPUT);
#ifdef PIXELS
    pixels.begin();
#endif

	//Initialize I2C communication
	Wire.begin();

	//Initialization of the BNO055
	BNO_Init(&myBNO); //Assigning the structure to hold information about the device

	//Configuration to NDoF mode
	bno055_set_operation_mode(OPERATION_MODE_NDOF);
	Serial.begin(115200);

	delay(100);

    printf("-----------------------\n");
    printf("Chip ID: %d\n", myBNO.chip_id);
    printf("Software Revision ID: %d\n", myBNO.sw_revision_id);
    printf("Page ID: %d\n", myBNO.page_id);
    printf("Accelerometer Revision ID: %d\n", myBNO.accel_revision_id);
    printf("Gyroscope Revision ID: %d\n", myBNO.gyro_revision_id);
    printf("Magnetometer Revision ID: %d\n", myBNO.mag_revision_id);
    printf("Bootloader Revision ID: %d\n", myBNO.bootloader_revision_id);
    printf("Device Address: 0x%x\n", myBNO.dev_addr);
    printf("-----------------------\n");
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
    
    if (digitalRead(BUTTON) == 0) {
      calibration.pitch = pitch;
      calibration.roll = roll;
      calibration.yaw = yaw;
      printf("calibratation pitch=%.2f roll=%.2f\n", calibration.pitch);
      delay(300);
    }

    pitch = pitch - calibration.pitch;
    roll = roll - calibration.roll;
    yaw = yaw - calibration.yaw;

    float tot = sqrt(pow(pitch, 2) + pow(roll, 2));
    float fractional = tot - (int)tot;
    int decimal = (int)round(NUMPIXELS * fractional*10 / 10);
    
    printf("%09d yaw=%.2f roll=%.2f pitch:%.2f module:%.2f decimal=%d\n", lastTime, yaw, roll, pitch, tot, decimal);

#ifdef PIXELS
    for(int i=0; i<NUMPIXELS; i++) {
        if (i == decimal )
            pixels.setPixelColor(i, pixels.Color(i*25, 0, 255-i*25));
        else
            pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        pixels.show();
    }
#endif
  }
  delay(10);
}


