/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Arjun/Documents/IOT/IOTCapstone/RxSide/src/RxSide.ino"
/*
 * Project RxSide
 * Description: recieve laser beam and send signal to other Lora
 * Author: Arjun Bhakta
 * Date: 12-April- 2022
 */

void setup();
void loop();
#line 8 "c:/Users/Arjun/Documents/IOT/IOTCapstone/RxSide/src/RxSide.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

const int PHOTODIODE = A0;
int photoVal;
int largestPhotoVal;
bool largerReadingDetected;
unsigned int timer;

void setup() {
pinMode(PHOTODIODE, INPUT);
largerReadingDetected=false;

}

void loop() {


photoVal = analogRead(PHOTODIODE);

if(millis()-timer>500){
//Serial.printf("%i\n", photoVal);
timer= millis();
}

if(photoVal > largestPhotoVal){
  largestPhotoVal= photoVal;
  largerReadingDetected= true;
}
if(largerReadingDetected){
  Serial.printf("%i\n", largestPhotoVal);
  largerReadingDetected= false;
}



}