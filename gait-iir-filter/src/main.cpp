#include <Arduino.h>
#include <Wire.h>
#include "mpu6050.h"
#include "sdrw.h"
#include "imu_filter.h"
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

/* ===== NEOPIXEL LED ===== */
#define LED_PIN 12

Adafruit_NeoPixel led(1, LED_PIN, NEO_GRB + NEO_KHZ800);
/* ===== END NEOPIXEL LED ===== */



/* ===== MPU-6050 I2C ===== */
#define SDA 3
#define SCL 1
/* ===== END MPU-6050 I2C ===== */



/* ===== DSP ===== */
#define X_BUFFER_SIZE 5
#define Y_BUFFER_SIZE 4

float b_coeff[X_BUFFER_SIZE] = {9.500782e-05, 3.800313e-04, 5.700469e-04, 3.800313e-04, 9.500782e-05};
float a_coeff[Y_BUFFER_SIZE] = {-3.449698, 4.495498, -2.620084, 0.575804};

imu_buffer_t* inputs;
imu_buffer_t* outputs;

// Anteroposterior acceleration
int16_t ap_accel = 0;
// Filtered signal
int16_t filtered_accel = 0;
int16_t prev_filtered_accel = 0;

bool dataAvailable = false;

// Difference
int16_t diff_accel = 0;
int16_t prev_diff_accel = 0;

int LED_timer = 0;


/* ===== END DSP ===== */


void setup() {
  // MPU-6050 I2C
  Wire.begin(SDA, SCL);
  MPU6050_write(MPU6050_RA_PWR_MGMT_1, 0x00);

  led.begin();
  led.clear();

  // Setup SD card
  if(!SD_MMC.begin()){
    while(1)
    {
      led.setPixelColor(0, led.Color(200, 0, 0));
      led.show();
    }
  }
  uint8_t cardType = SD_MMC.cardType();

  if(cardType == CARD_NONE){
    while(1)
    {
      led.setPixelColor(0, led.Color(200, 0, 0));
      led.show();
    }
  }

  // Empty file
  writeFile(SD_MMC, "/raw.txt", "");
  writeFile(SD_MMC, "/filtered.txt", "");
  writeFile(SD_MMC, "/trigger.txt", "");

  inputs = imu_buffer_init(Y_BUFFER_SIZE);
  outputs = imu_buffer_init(X_BUFFER_SIZE);
}

uint8_t red = 200;
uint8_t green = 0;
uint8_t blue = 200;

void loop() 
{
  MPU6050_getAccelAxis(AXIS_Y, &ap_accel);
  filtered_accel = imu_filter(ap_accel, inputs, b_coeff, outputs, a_coeff);
  diff_accel = filtered_accel - prev_filtered_accel;

  appendFile(SD_MMC, "/raw.txt", (String(micros()) + ',' + String(ap_accel) + "\n").c_str());
  appendFile(SD_MMC, "/filtered.txt", (String(micros()) + ',' + String(filtered_accel) + "\n").c_str());
  //appendFile(SD_MMC, "/diff.txt", (String(micros()) + ',' + String(diff_accel) + "\n").c_str());

  red = 0;
  green = 200;
  blue = 0;

  if (diff_accel >= 0 && prev_diff_accel < 0)
  {
    LED_timer = 100;
    //appendFile(SD_MMC, "/trigger.txt", (String(micros()) + ',' + String(diff_accel) + "-" + String(prev_diff_accel) + "\n").c_str());
  }

  led.setPixelColor(0, led.Color(red, green, blue));
  led.show();

  prev_filtered_accel = filtered_accel;
  prev_diff_accel = diff_accel;
}