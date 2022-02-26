#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
#define PICC_0 0x16 
#define PICC_1 0x35
#define PICC_2 0x8A
#define PICC_3 0xAB

const int xPin = 5;
const int yPin = 4;
const int zPin = 3;
int minVal = 265;
int maxVal = 402;
double x, y, z;

long randNumber;

int dataPin = 2;
int latchPin = 4;
int clockPin = 6;
byte row[8]={~B00000001,~B00000010,~B00000100,~B00001000,~B00010000,~B00100000,~B01000000,~B10000000};
byte col[8]={
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
  };

void right(){
  for(int i=0; i<8; i++){
    for(int j=0; j<7; j++){
      if(bitRead(col[i],7-j-1)==1 and bitRead(col[i],7-j)==0){
        bitClear(col[i],7-j-1);
        bitSet(col[i],7-j);
      }
    }
  }
}

void left(){
  for(int i=0; i<8; i++){
    for(int j=0; j<7; j++){
      if(bitRead(col[i],j+1)==1 and bitRead(col[i],j)==0){
        bitClear(col[i],j+1);
        bitSet(col[i],j);
      }
    }
  }
}

void down(){
  for(int i=0; i<7; i++){
    for(int j=0; j<8; j++){
      if(bitRead(col[7-i-1], j)==1 and bitRead(col[7-i], j)==0){
        bitClear(col[7-i-1], j);
        bitSet(col[7-i], j);
      }
    }
  }
}

void up(){
  for(int i=0; i<7; i++){
    for(int j=0; j<8; j++){
      if(bitRead(col[i+1], j)==1 and bitRead(col[i], j)==0){
        bitClear(col[i+1], j);
        bitSet(col[i], j);
      }
    }
  }
}

void arr_rand(){
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      bitClear(col[i], j);
    }
  }
  randomSeed(analogRead(0));
  for(int i=0; i<8; i++){
    randNumber = random(9);
    for(int j=0; j<randNumber; j++){
      randNumber = random(9);
      bitSet(col[i], randNumber);
    }
  }
}
void arr_heart(){
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      bitClear(col[i], j);
    }
  }
  bitSet(col[0], 1);
  bitSet(col[0], 2);
  bitSet(col[0], 5);
  bitSet(col[0], 6);
  for(int i=0; i<8; i++){
    bitSet(col[1], i);
    bitSet(col[2], i);
    bitSet(col[3], i);
    bitSet(col[7], i);
  }
  bitSet(col[4], 1);
  bitSet(col[4], 2);
  bitSet(col[4], 3);
  bitSet(col[4], 4);
  bitSet(col[4], 5);
  bitSet(col[4], 6);
  bitSet(col[5], 2);
  bitSet(col[5], 3);
  bitSet(col[5], 4);
  bitSet(col[5], 5);
  bitSet(col[6], 3);
  bitSet(col[6], 4);
}

void setup(){
  Serial.begin(9600); 
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(7, INPUT);
  SPI.begin();
  rfid.PCD_Init();
}

void dispoff(){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0x00);
  shiftOut(dataPin, clockPin, MSBFIRST, 0x00);
  digitalWrite(latchPin, HIGH);
  delay(1);
}

void main_game(){
  arr_rand();
  while(true){
    int readValue = digitalRead(7);
    if (readValue == 1){
      break;
    }
    int xRead = analogRead(xPin);
    int yRead = analogRead(yPin);
    int zRead = analogRead(zPin);
    int xAng = map(xRead, minVal, maxVal, -90, 90);
    int yAng = map(yRead, minVal, maxVal, -90, 90);
    int zAng = map(zRead, minVal, maxVal, -90, 90);
    x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
    y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  
    if(x<180){
      down();
    }
    else if(x>180){
      up();
    }
    if(y>180){
      right();
    }
    else if(y<180){
      left();
    }
  
    for(int i=0; i<8; i++){
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, LSBFIRST, row[i]);
      shiftOut(dataPin, clockPin, LSBFIRST, col[7-i]);
      digitalWrite(latchPin, HIGH);
    }
    dispoff();
    delay(17);
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void heart(){
  arr_heart();
  while (true){
    int readValue = digitalRead(7);
    if (readValue == 1){
      break;
    }
    for(int i=0; i<8; i++){
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, LSBFIRST, row[i]);
      shiftOut(dataPin, clockPin, LSBFIRST, col[7-i]);
      digitalWrite(latchPin, HIGH);
    }
    dispoff();
    delay(17);
  }
}

void loop(){
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != PICC_0 || 
      rfid.uid.uidByte[1] != PICC_1 || 
      rfid.uid.uidByte[2] != PICC_2 || 
      rfid.uid.uidByte[3] != PICC_3 ) {
        
    if (rfid.uid.uidByte[0]==163 &&
        rfid.uid.uidByte[1]==191 &&
        rfid.uid.uidByte[2]==82 &&
        rfid.uid.uidByte[3]==22){
          main_game();
      }
    if (rfid.uid.uidByte[0]==83 &&
        rfid.uid.uidByte[1]==91 &&
        rfid.uid.uidByte[2]==26 &&
        rfid.uid.uidByte[3]==21){
          heart(); 
        }
  }else{
    printHex(rfid.uid.uidByte, rfid.uid.size);    
  }
  
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
