/*
 * Project RxSide
 * Description: recieve laser beam and send signal to other Lora
 * Author: Arjun Bhakta
 * Date: 12-April- 2022
 */

SYSTEM_MODE(SEMI_AUTOMATIC);

const int PHOTODIODE = A0;

const int topPD = A1;
const int leftPD = A2;
const int rightPD = A3;
const int bottomPD = A4;

int photoVal;
int largestPhotoVal;
bool largerReadingDetected;
unsigned int timer;

int topPDval;
int leftPDval;
int rightPDval;
int bottomPDval;

int largestTopVal;
int largestLeftVal;
int largestRightVal;
int largestBottomVal;

int largerTop;
int largerLeft;
int largerRight;
int largerBottom;

bool topFlag;
bool leftFlag;
bool rightFlag;
bool bottomFlag;

int dataArray[5000][3];
unsigned int pdTimer;
int rowCounter;
bool printArray;
int lastRow;
unsigned int printTimer;
int r;

int pdArray[5000][2];
unsigned int startTimer2;
int largestValCounter;




// Lora Vars
// String password = "BA4204031968BA1114199009021994A7";
String reply;
int addr = 888;
int net = 5;
byte buf[40];

int dataLen;
int dataResetLora;
int dataLora;
int db;
int SNR;

unsigned int recievedTime;
String recievedTimeString;
int recievedTimeLenght;
int recievedTimeState;

String parse0;
String parse1;
String parse2;
String parse3;
String parse4;



void setup() {

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
    delay(200);
    if (Serial1.available() > 0) {
        Serial.printf("Awaiting Reply\n");
        reply = Serial1.readStringUntil('\n');
        reply.getBytes(buf, 40);
        Serial.printf("Reply get password: %s\n", (char *)buf);
    }
    delay(500);

    // Initialize Pins
    // pinMode(PHOTODIODE, INPUT);
    pinMode(topPD, INPUT);
    pinMode(leftPD, INPUT);
    pinMode(rightPD, INPUT);
    pinMode(bottomPD, INPUT);

    largerReadingDetected = false;

    pinMode(D8, OUTPUT); // interupt sending PIN
    pinMode(D6, OUTPUT); // HIGH LOW to send single byte for interrupt
    pinMode(D7, OUTPUT); // HIGH LOW to Send Single byte for interrupt

    pinMode(D5, INPUT);                      // Pin to trigger resetPDval
    attachInterrupt(D5, resetPDval, RISING); // Interrupt that triggers to reset PhotoDioide Values (Example: in the begin of a scanning procedure)

    printArray = false;
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
        dataLen = parse1.toInt();
        dataLora = parse2.toInt();
        //db = parse3.toInt();
        //SNR = parse4.toInt();
        if(dataLora == 0){
          Serial.printf("Scan did not work\n");
        }
        if(dataLora == 5){
          startTimer2 = micros();
          Serial.printf("Starting Timer\n");
          largestPhotoVal = 0;
        }
        if(dataLora == 9){
            recievedTimeString = String(recievedTime);
            recievedTimeLenght = recievedTimeString.length();
            Serial1.printf("AT+SEND=999,%i,%s\r\n",recievedTimeLenght,recievedTimeString.c_str());
            Serial.printf("Sent\n");
            Serial.printf("Incoming Len %i, IncomingData %i\n", dataLen, dataLora);
        }
        
    }


    
        photoVal = analogRead(PHOTODIODE);
        // topPDval = analogRead(topPD);
        // leftPDval = analogRead(leftPD);
        // rightPDval = analogRead(rightPDval);
        // bottomPDval = analogRead(bottomPDval);

        // if( millis()-timer> 2000){
        // Serial.printf("topPd = %i, leftPd = %i, rightPd= %i, bottomPd = %i \n", topPDval,leftPDval,rightPDval,bottomPDval);
        // timer = millis();
        // }

        if (photoVal > largestPhotoVal) {
            largestPhotoVal = photoVal;
            largerReadingDetected = true;
        }

        if (largerReadingDetected) {
            // Serial.printf("%i\n", largestPhotoVal);
            recievedTime = micros()- startTimer2;
            largerReadingDetected = false;
            Serial.printf("NewVal %i\n", recievedTime);
        }


        //top photoDiode
        if (topPDval > largestTopVal + 100) {
            largestTopVal = topPDval;
            topFlag = true;
            // Serial.printf("topPd = %i, leftPd = %i, rightPd= %i, bottmPd = %i \n", largestTopVal,largestLeftVal,largestRightVal,largestBottomVal);
        }

        if (topFlag) {
            topFlag = false;
            // create (0,0) reading
            digitalWrite(D6, LOW);
            digitalWrite(D7, LOW);
            // trigger interrupt
            digitalWrite(D8, HIGH);
            digitalWrite(D8, LOW);
            // Serial.printf("%i\n", largestPhotoVal);
            // Serial1.printf("AT+SEND=999,1,1\r\n");
            pdTimer = millis();
            rowCounter++;
            dataArray[rowCounter][0] = 0;
            dataArray[rowCounter][1] = pdTimer;
            dataArray[rowCounter][2] = topPDval;
        }

        // Left photoDiode
        if (leftPDval > largestLeftVal + 100) {
            largestLeftVal = leftPDval;
            leftFlag = true;
            // Serial.printf("topPd = %i, leftPd = %i, rightPd= %i, bottmPd = %i \n", largestTopVal,largestLeftVal,largestRightVal,largestBottomVal);
        }

        if (leftFlag) {
            // Serial.printf("%i\n", largestPhotoVal);
            // Serial1.printf("AT+SEND=999,1,1\r\n");
            leftFlag = false;
            // create (0,1) reading
            digitalWrite(D6, LOW);
            digitalWrite(D7, HIGH);
            // trigger interrupt
            digitalWrite(D8, HIGH);
            digitalWrite(D8, LOW);

            pdTimer = millis();
            rowCounter++;
            dataArray[rowCounter][0] = 1;
            dataArray[rowCounter][1] = pdTimer;
            dataArray[rowCounter][2] = leftPDval;
        }

        // Right photoDiode
        if (rightPDval > largestRightVal + 100) {
            largestRightVal = rightPDval;
            rightFlag = true;
            // Serial.printf("topPd = %i, leftPd = %i, rightPd= %i, bottmPd = %i \n", largestTopVal,largestLeftVal,largestRightVal,largestBottomVal);
        }

        if (rightFlag) {
            // Serial.printf("%i\n", largestPhotoVal);
            // Serial1.printf("AT+SEND=999,1,1\r\n");
            rightFlag = false;
            // create (1,0) reading
            digitalWrite(D6, HIGH);
            digitalWrite(D7, LOW);
            // trigger interrupt
            digitalWrite(D8, HIGH);
            digitalWrite(D8, LOW);
            // store into data Array
            pdTimer = millis();
            rowCounter++;
            dataArray[rowCounter][0] = 2;
            dataArray[rowCounter][1] = pdTimer;
            dataArray[rowCounter][2] = rightPDval;
        }

        // bottom photoDiode
        if (bottomPDval > largestBottomVal + 100) {
            largestBottomVal = bottomPDval;
            bottomFlag = true;
            // Serial.printf("topPd = %i, leftPd = %i, rightPd= %i, bottomPd = %i \n", largestTopVal,largestLeftVal,largestRightVal,largestBottomVal);
        }

        if (bottomFlag) {
            // Serial.printf("%i\n", largestPhotoVal);
            // Serial1.printf("AT+SEND=999,1,1\r\n");
            bottomFlag = false;
            // create (1,1) reading
            digitalWrite(D6, HIGH);
            digitalWrite(D7, HIGH);
            // trigger interrupt
            digitalWrite(D8, HIGH);
            digitalWrite(D8, LOW);

            // store into data Array
            pdTimer = millis();
            rowCounter++;
            dataArray[rowCounter][0] = 3;
            dataArray[rowCounter][1] = pdTimer;
            dataArray[rowCounter][2] = bottomPDval;
        }

        // if (printArray && millis()-printTimer > 10000 ){
        //   Serial.printf("printStart\n");
        //   for (r=0; r < lastRow; r++){

        //     Serial.printf("%i,%i,%i\n",dataArray[r][0],dataArray[r][1],dataArray[r][2]);
        //   }
        //    printArray = false;
        // }

        // Serial.printf("%i,%i,%i\n",dataArray[r][0],dataArray[r][1],dataArray[r][2]);
    }

void resetPDval() {
        largestBottomVal = 0;
        largestTopVal = 0;
        largestLeftVal = 0;
        largestRightVal = 0;
        printArray = true;
        lastRow = rowCounter;
        rowCounter = 0;
        printTimer = millis();
    }

