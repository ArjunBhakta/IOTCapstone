/*
 * Project RxSide
 * Description: recieve laser beam and send signal to other Lora
 * Author: Arjun Bhakta
 * Date: 12-April- 2022
 */

SYSTEM_MODE(SEMI_AUTOMATIC);

const int PHOTODIODE = A0;
int photoVal;
int largestPhotoVal;
bool largerReadingDetected;
unsigned int timer;

void setup() {

Serial1.begin(115200);                               // defualt rate  of  is 1152200 
delay(500);

Serial1.printf( "AT\r\n"); 
delay(50);

Serial1.printf("AT+PARAMETER = 10,7,1,7 \r\n");      // use for shorter distances (less than 3km)
//Serial.1printf("AT+PARAMETER = 12,4,1,7 \r\n");    // use for longer distances (more than 3km)
delay(50);

Serial1.printf("AT+BAND=868500000\r\n");             // Bandwidth set to 868.5MHz
delay(50);

Serial1.printf("AT+ADDRESS = 123");                  // unique address that ranges from 0-65535(default 0) | stored in EEPROM
delay(50);

Serial1.printf("AT+NETWORKID =5\r\n");               // network id 0-16  (defualt is 0 but also the pblic id) | avoid using 0 | setting stored in EEPROM
delay(50);

Serial1.printf("AT+PARAMETER? \r\n");                // check AT+PARAMETER
delay(50);

Serial1.printf("AT+BAND?\r\n");                      // check AT+BAND
delay(50);

Serial1.printf("AT+NETWORKID?\r\n");                 // check AT+NETWORKID
delay(50);

Serial.printf("AT+ADDRESS?\r\n");                    // check AT+ADDRESS
delay(50);



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