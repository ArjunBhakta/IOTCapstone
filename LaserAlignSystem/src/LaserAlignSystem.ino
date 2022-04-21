/*
 * Project LaserAlignSystem
 * Description:
 * Author: Arjun Bhakta
 * Date: 11 - April - 2022
 */

SYSTEM_MODE(SEMI_AUTOMATIC);

// library
#include <Stepper.h>
#include <math.h>

const int stepsPerRevolution = 2048;
const int RPM = 10;
Stepper tipStepper(stepsPerRevolution, D8, D6, D7, D5);
Stepper tiltStepper(stepsPerRevolution, A5, D12, D13, D11);

// These UUIDs were defined by Nordic Semiconductor and are now the defacto standard for
// UART-like services over BLE. Many apps support the UUIDs now, like the Adafruit Bluefruit app.
const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);
BleAdvertisingData data;

const int PHOTODIODE = A0;
int photoVal;
int largestPhotoVal;
bool largerReadingDetected;
unsigned int timer;
bool ScanToggle;

int currentX;
int currentY;
int n;
int pdX[4];
int pdY[4];
int pdDirection;

byte valD3;
byte valD4;

const byte pins[] = {valD3, valD4};
int signalPins = sizeof(pins);
int i;

int singleX;
int singleY;

byte recievedData;

// Lora Vars
// String password = "BA4204031968BA1114199009021994A7";
String reply;
int addr = 999;
int net = 5;
byte buf[40];
int dataLen;
int dataLora;
int db;
int SNR;

String parse0;
String parse1;
String parse2;
String parse3;
String parse4;

unsigned int sentTime;
unsigned int secondTime;
String sentTimeString;
int sentTimeLenght;
unsigned int offset;

unsigned int stepperArray[5000][3];
unsigned int startTimer1;
unsigned int endTimer1;
int totalScanTime;
int stepperCounter;

bool MoveToState;
bool button4State;

bool StartTimer;
unsigned int storedTimer;

int Spiral(String extra);

void setup() {
    //Particle.function("spiral",Spiral);
    Serial.begin(9600);
    delay(3000);
    Serial.printf("Beginning Commands \n");

    Serial1.begin(115200);
    Serial1.printf("AT\r\n");
    delay(200);
    if (Serial1.available() > 0) {
        Serial.printf("Awaiting Reply\n");
        reply = Serial1.readStringUntil('\n');
        Serial.printf("Reply: %s\n", reply.c_str());
    }

    // Serial1.printf("AT+CPIN=%s\r\n",password.c_str()); //correct
    // delay(200);
    // if(Serial1.available()>0) {
    //   Serial.printf("Awaiting Reply from password\n");
    //   reply = Serial1.readStringUntil('\n');
    //   Serial.printf("Reply: %s\n",reply.c_str());
    // }

    Serial1.printf("AT+ADDRESS=%i\r\n", addr); // correct
    delay(200);
    if (Serial1.available() > 0) {
        Serial.printf("Awaiting Reply from address\n");
        reply = Serial1.readStringUntil('\n');
        Serial.printf("Reply address: %s\n", reply.c_str());
    }

    Serial1.printf("AT+NETWORKID=%i\r\n", net); // correct
    delay(200);
    if (Serial1.available() > 0) {
        Serial.printf("Awaiting Reply from networkid\n");
        reply = Serial1.readStringUntil('\n');
        Serial.printf("Reply network: %s\n", reply.c_str());
    }

    Serial1.printf("AT+ADDRESS?\r\n"); // correct
    delay(200);
    if (Serial1.available() > 0) {
        Serial.printf("Awaiting Reply\n");
        reply = Serial1.readStringUntil('\n');
        Serial.printf("Reply get address: %s\n", reply.c_str());
    }

    Serial1.printf("AT+NETWORKID?\r\n"); // correct
    delay(200);
    if (Serial1.available() > 0) {
        Serial.printf("Awaiting Reply\n");
        reply = Serial1.readStringUntil('\n');
        Serial.printf("Reply get network: %s\n", reply.c_str());
    }

    Serial1.printf("AT+CPIN?\r\n"); // correct
    delay(500);
    if (Serial1.available() > 0) {
        Serial.printf("Awaiting Reply\n");
        reply = Serial1.readStringUntil('\n');
        reply.getBytes(buf, 40);
        Serial.printf("Reply get password: %s\n", (char *)buf);
    }
   

    // pinMode(D2, INPUT);
    // attachInterrupt(D2, getSingleVal, RISING);

    BLE.on();
    BLE.addCharacteristic(txCharacteristic);
    BLE.addCharacteristic(rxCharacteristic);
    data.appendServiceUUID(serviceUuid);
    BLE.advertise(&data);

    tipStepper.setSpeed(RPM);
    tiltStepper.setSpeed(RPM);

    pinMode(PHOTODIODE, INPUT); // delete this later only for quick testing purposes of photodioide pin on TX side

    pinMode(D3, INPUT); // bit 2
    pinMode(D4, INPUT); // bit 1
    pinMode(A1, OUTPUT); // reset pd values with a HIGH/LOW SIGNAL to an interrupt ( it will trigger an interrupt on the other side )

    ScanToggle = false;
    MoveToState= false;
    button4State= false;
}

void loop() {

    if (Serial1.available()) {
        Serial.printf("Awaiting Incoming Message");
        parse0 = Serial1.readStringUntil(',');
        parse1 = Serial1.readStringUntil(',');
        parse2 = Serial1.readStringUntil(',');
        parse3 = Serial1.readStringUntil(',');
        parse4 = Serial1.readStringUntil('\n');
        Serial.printf("Incoming Message; %s\n%s\n%s\n%s\n%s\n", parse0.c_str(), parse1.c_str(), parse2.c_str(), parse3.c_str(), parse4.c_str());
        dataLen = parse2.toInt();
        dataLora = parse3.toInt();
        db = parse4.toInt();
        //SNR = parse5.toInt();
        Serial.printf("Incoming Len %i, IncomingData %i\n", dataLen, dataLora);
          if(dataLora == 0){
          Serial.printf("Scan did not work\n");
        }
        if(dataLora >7){
            MoveToState= true;
        }

    }
    //ParabolaScan();

    // if (StartTimer) {
    //     sentTime = micros();
    //     sentTimeString = String(sentTime);
    //     sentTimeLenght = sentTimeString.length();
    //     Serial1.printf("AT+SEND=888,%i,%s\r\n", sentTimeLenght, sentTimeString.c_str());
    //     Serial.printf("Sent");
    //     StartTimer = false;
    // }

    if (ScanToggle) {
        Scan();
        printArray();
        // cyclePD(); // serial print photdiode position
        ScanToggle = false;
    }
    if(MoveToState){
        SearchArray();
        goTOsinglePD();
        MoveToState=false;
    }   
    if (button4State){
        //SpiralScan();
        ParabolaScan();
        ParabolaScan();
        ParabolaScan();
        button4State= false;
    }
    
  
}

void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context) {

    uint8_t i;
    static int stepSize;

    // Serial.printf("Recieved data from; %02X:%02X:%02X:%02X:%02X:%02X \n", peer.address()[0], peer.address()[1], peer.address()[2], peer.address()[3], peer.address()[4], peer.address()[5]);
    // Serial.printf("Bytes:");

    for (i = 0; i < len; i++) {
        // Serial.printf("%02X", data[i]);
    }

    // up arrow recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x35 && data[3] == 0x31) {
        // Serial.printf("\nUp Arrow");
        tipStepper.step(stepSize);
    }

    // down arrow recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x36 && data[3] == 0x31) {
        // Serial.printf("\nDown Arrow");
        tipStepper.step(-stepSize);
    }

    // right arrow  recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x38 && data[3] == 0x31) {
        // Serial.printf("\nRight Arrow");
        tiltStepper.step(-stepSize);
    }

    // left arrow recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x37 && data[3] == 0x31) {
        // Serial.printf("\nLeft Arrow");
        tiltStepper.step(stepSize);
    }

    // button1 recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x31 && data[3] == 0x31) {
        stepSize++;
        // Serial.printf("stepsize = %i", stepSize);
    }

    // button2 recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x32 && data[3] == 0x31) {
        stepSize--;
        if (stepSize < 1) {
            stepSize = 1;
        }
        StartTimer = true;

        // Serial.printf("stepsize = %i", stepSize);
    }

    // button3 recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x33 && data[3] == 0x31) {
        // Serial.printf("\nbutton3");
        ScanToggle = true;
        stepperCounter = 0;
    }

    // button4 recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x34 && data[3] == 0x31) {
        // Serial.printf("\nbutton4");
        
        button4State = true;
    }

    // Serial.printf("\n");
    // Serial.printf("Message: %s \n", (char *)data);
}

void ScanSpiral() {
    int totalSteps = 50;
    int i;
    int motorSpeed;

    for (i = 0; i < totalSteps; i++) {
        motorSpeed = map(i, 0, totalSteps, 3, 10);
        tiltStepper.setSpeed(motorSpeed);
        tipStepper.setSpeed(motorSpeed);
        if (i % 2 == 0) {
            tiltStepper.step(i);
            tipStepper.step(i);
        }
        if (i % 2 == 1) {
            tiltStepper.step(-i);
            tipStepper.step(-i);
        }
    }
    tiltStepper.setSpeed(RPM);
    tiltStepper.setSpeed(RPM);
    tiltStepper.step(totalSteps / 2);
    tipStepper.step(totalSteps / 2);
}

void ScanHorizontal() {
    int totalHorizontalSteps = 50; // maintain even step sizes
    int totalVerticalSteps = 50;
    int i;
    int h;
    int n = 1;
    int currentX;
    int currentY;

    tiltStepper.step(totalHorizontalSteps / 2); // move to the left from current position
    tipStepper.step(totalVerticalSteps / 2);    // move upwards from current position
    currentX = totalHorizontalSteps / 2;
    currentY = totalVerticalSteps / 2;
    Serial.printf("currentX = %i , currentY = %i\n", currentX, currentY);
    for (i = 0; i < totalVerticalSteps; i++) {
        n = n * -1;
        for (h = 0; h < totalHorizontalSteps; h++) {
            tiltStepper.step(1 * n);
            currentX = currentX + (1 * n);
            Serial.printf("current x = %i , currentY = %i\n", currentX, currentY);
        }
        tipStepper.step(-1);
        currentY = currentY - 1;
    }

    tiltStepper.step(-totalHorizontalSteps / 2); // move to the right to reset to current position
    tipStepper.step(totalVerticalSteps / 2);     // move downwards to reset to current position
    currentX = currentX + (-totalHorizontalSteps / 2);
    currentY = currentY + (totalVerticalSteps / 2);
    Serial.printf("current x = %i , currentY = %i\n", currentX, currentY);
}

void Scan() {
    int totalHorizontalSteps = 50; // maintain even step sizes
    int totalVerticalSteps = 50;

    unsigned int stepTimer;
    int horizontalCounter;
    int verticalCounter;
    bool ScanComplete;
    n = 1;
    horizontalCounter = 0;
    verticalCounter = 0;
    currentX = 0;
    currentY = 0;
    stepperCounter= 0;
    n = n * -1;
  
    tiltStepper.step(totalHorizontalSteps / 2); // move to the left from current position
    tipStepper.step(totalVerticalSteps / 2);    // move upwards from current position
    digitalWrite(A1, HIGH);                     // trigger an interrupt to clear PD val
    digitalWrite(A1, LOW);                      // reset pin for next interrupt
    currentX = totalHorizontalSteps / 2;
    currentY = totalVerticalSteps / 2;
    
    Serial1.printf("AT+SEND=888,1,5\r\n");
     
    delayMicroseconds(1065273 *.827); 
    startTimer1 = micros();                // delay time between Tx and Rx
    //Serial.printf("current x = %i , currentY = %i\n", currentX, currentY);
    ScanComplete = false;
    // startTimer
    while (ScanComplete == false) {

        if (micros() - stepTimer > 1000) {

            if (horizontalCounter <= totalHorizontalSteps) {
                stepTimer = micros();
                storedTimer= stepTimer-startTimer1;
                tiltStepper.step(1 * n);
                currentX = currentX + (1 * n);
                stepperCounter++;
                stepperArray[stepperCounter][0] = storedTimer;
                stepperArray[stepperCounter][1] = currentX;
                stepperArray[stepperCounter][2] = currentY;
            }

            horizontalCounter++;
        }
        if (horizontalCounter == totalHorizontalSteps) {
            stepTimer = micros();
            storedTimer= stepTimer-startTimer1;
            tipStepper.step(-1);
            currentY = currentY - 1;
            stepperCounter++;
            stepperArray[stepperCounter][0] = storedTimer;
            stepperArray[stepperCounter][1] = currentX;
            stepperArray[stepperCounter][2] = currentY;
            horizontalCounter = 0;
            n = n * -1;
            verticalCounter++;
        }

        if (verticalCounter == totalVerticalSteps) {
            ScanComplete = true;
        }
    }

    tiltStepper.step(-totalHorizontalSteps / 2); // move to the right to reset to current position
    tipStepper.step(totalVerticalSteps / 2);     // move downwards to reset to current position
    storedTimer= stepTimer-startTimer1;
    currentX = currentX + (-totalHorizontalSteps / 2);
    currentY = currentY + (totalVerticalSteps / 2);
    
    Serial.printf(" movedStepper");
    endTimer1 = micros();
    stepperCounter++;
    stepperArray[stepperCounter][0] = storedTimer;
    stepperArray[stepperCounter][1] = currentX;
    stepperArray[stepperCounter][2] = currentY;

    Serial1.printf("AT+SEND=888,1,9\r\n");
    Serial.printf(" sending 9");
    delay(200);
    if( Serial1.available()>0){
        reply = Serial1.readStringUntil('\n');
        Serial.printf("\n Reply to send : %s\n", reply.c_str());
    }
    // Serial.printf("current x = %i , currentY = %i\n", currentX, currentY);
}

void getPDval() {
    valD4 = digitalRead(D4);
    valD3 = digitalRead(D3);
    i = valD4 << 1 | valD3;
    pdX[i] = currentX;
    pdY[i] = currentY;
    // Serial.printf("interrupted %i \n",i);
    //  pdDirection = n;
}

void getSingleVal() {
    singleX = currentX;
    singleY = currentY;
    Serial.printf("singleX %i, singleY %i\n", singleX, singleY);
    // pdDirection = n;
}


void goTOsinglePD() {
    tiltStepper.step(singleX);
    tipStepper.step(singleY);
    Serial.printf("moved to %i, %i\n", singleX, singleY);
}


void goTOpd() {
    int x;
    int tipAverage;
    int tiltAverage;

    tiltAverage = (pdX[0] + pdX[3]) / 2;
    tipAverage = (pdY[1] + pdY[2]) / 2;

    tiltStepper.step(pdX[3]);
    tipStepper.step(pdY[3]);
    Serial.printf("tiltAverage %i, tipAverage = %i\n", tiltAverage, tipAverage);

    // tiltStepper.step((-1*pdDirection));
}

void cyclePD() {
    int c;
    for (c = 0; c < 4; c++) {
        Serial.printf("pdX[%i]= %i, pdY[%i] =%i\n", c, pdX[c], c, pdY[c]);
    }
}

void printArray() {
int r; 
    Serial.printf("printStart\n");
    
    for (r = 0; r < stepperCounter; r++) {
    Serial.printf("%i,%i,%i,%i\n", r,stepperArray[r][0], stepperArray[r][1], stepperArray[r][2]);
    }
    
}

void SearchArray(){
int x; 
int row;
int offset;
int comparedVal;
int prevOffset = 7330673;


for(x = 0; x < 5000; x++){

    comparedVal = stepperArray[x][0];
    offset = dataLora - comparedVal;
    if (abs(offset) < prevOffset){
        prevOffset = offset;
        row = x;
       // Serial.printf("offset %i  row = %i\n ", offset, x);
    }
 } 
 singleX = stepperArray[row][1];
 singleY = stepperArray[row][2];
 stepperCounter=0;
 Serial.printf("singleX= %i, singleY=%i\n", singleX, singleY);
}

void ParabolaScan(){
int amp =100;
static int x1;
static int y1;
static int x2;
static int y2;

int p = 200 ;

for (x1=0;x1<=50; x1++){
    y1= (x1*x1)/p;
    Serial.printf ("x = %i, y= %i \n",x1, y1);
    Serial.printf ("x2 = %i, y2= %i \n",x2, y2);
    tiltStepper.step(x2-x1); // move to the right to reset to current position
    tipStepper.step(y2-y1);  // move downwards to reset to current position
    x2= x1;
    y2=y1;
    
}
x1=0;
x2=0;
y1 =0;
y2=0;

for(x1=0; x1<=50;x1++){
    y1= (x1*x1)/p;
    Serial.printf ("x = %i, y= %i \n",x1, y1);
    Serial.printf ("x2 = %i, y2= %i \n",x2, y2);
    tiltStepper.step(x1-x2); // move to the right to reset to current position
    tipStepper.step(y1-y2);  // move downwards to reset to current position
    x2= x1;
    y2=y1;
   
}
x1=0;
x2=0;
y1=0;
y2= 0;


for(x1=0; x1>=-50;x1--){
    y1= (x1*x1)/p;
    Serial.printf ("x = %i, y= %i \n",x1, y1);
    Serial.printf ("x2 = %i, y2= %i \n",x2, y2);
    tiltStepper.step(x2-x1); // move to the right to reset to current position
    tipStepper.step(y2-y1);  // move downwards to reset to current position
    x2= x1;
    y2=y1;
  
}
x1=0;
x2=0;
y1 =0;
y2=0;
for(x1=0; x1>=-50;x1--){
    y1= (x1*x1)/p;
    Serial.printf ("x = %i, y= %i \n",x1, y1);
    Serial.printf ("x2 = %i, y2= %i \n",x2, y2);
    tiltStepper.step(x1-x2); // move to the right to reset to current position
    tipStepper.step(y1-y2);  // move downwards to reset to current position
    x2= x1;
    y2=y1;

}


x1=0;
x2=0;
y1=0;
y2=0;


}


