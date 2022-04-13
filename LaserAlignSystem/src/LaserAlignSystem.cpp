/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Arjun/Documents/IOT/IOTCapstone/LaserAlignSystem/src/LaserAlignSystem.ino"
/*
 * Project LaserAlignSystem
 * Description:
 * Author: Arjun Bhakta
 * Date: 11 - April - 2022
 */

void setup();
void loop();
void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
void ScanBeamHorizontal();
void ScanSpiral();
#line 8 "c:/Users/Arjun/Documents/IOT/IOTCapstone/LaserAlignSystem/src/LaserAlignSystem.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

// library
#include <Stepper.h>

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
int photodioideReading;
unsigned int timer;

void setup() {

    Serial.begin();
    waitFor(Serial.isConnected, 15000);

    BLE.on();
    BLE.addCharacteristic(txCharacteristic);
    BLE.addCharacteristic(rxCharacteristic);
    data.appendServiceUUID(serviceUuid);
    BLE.advertise(&data);

    tipStepper.setSpeed(RPM);
    tiltStepper.setSpeed(RPM);

    pinMode(PHOTODIODE, INPUT);
}

void loop() {
}

void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context) {

    uint8_t i;
    static int stepSize;

    Serial.printf("Recieved data from; %02X:%02X:%02X:%02X:%02X:%02X \n", peer.address()[0], peer.address()[1], peer.address()[2], peer.address()[3], peer.address()[4], peer.address()[5]);
    Serial.printf("Bytes:");

    for (i = 0; i < len; i++) {
        Serial.printf("%02X", data[i]);
    }

    // up arrow recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x35 && data[3] == 0x31) {
        Serial.printf("\nUp Arrow");
        tipStepper.step(stepSize);
    }

    // down arrow recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x36 && data[3] == 0x31) {
        Serial.printf("\nDown Arrow");
        tipStepper.step(-stepSize);
    }

    // right arrow  recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x38 && data[3] == 0x31) {
        Serial.printf("\nRight Arrow");
        tiltStepper.step(-stepSize);
    }

    // left arrow recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x37 && data[3] == 0x31) {
        Serial.printf("\nLeft Arrow");
        tiltStepper.step(stepSize);
    }

    // button1 recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x31 && data[3] == 0x31) {
        stepSize++;
        Serial.printf("stepsize = %i", stepSize);
    }

    // button2 recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x32 && data[3] == 0x31) {
        stepSize--;
        if (stepSize < 1) {
            stepSize = 1;
        }
        Serial.printf("stepsize = %i", stepSize);
    }

    // button3 recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x33 && data[3] == 0x31) {
        Serial.printf("\nbutton3");
        ScanBeamHorizontal();
    }

    // button4 recieved
    if (data[0] == 0x21 && data[1] == 0x42 && data[2] == 0x34 && data[3] == 0x31) {
        Serial.printf("\nbutton4");
        ScanSpiral();
    }

    Serial.printf("\n");
    Serial.printf("Message: %s \n", (char *)data);
}

void ScanBeamHorizontal() {
    int totalHorizontalSteps = 50; // maintain even step sizes
    int totalVerticalSteps = 50;
    int i;
    int n = 1;
    tiltStepper.step(totalHorizontalSteps / 2); // move to the left from current position
    tipStepper.step(totalVerticalSteps / 2);    // move upwards from current position
    for (i = 0; i < totalVerticalSteps; i++) {
        n = n * -1;
        tiltStepper.step(totalHorizontalSteps * n);
        tipStepper.step(-1);
    }
    tiltStepper.step(-totalHorizontalSteps / 2); // move to the right to reset to current position
    tipStepper.step(totalVerticalSteps / 2);     // move downwards to reset to current position
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
