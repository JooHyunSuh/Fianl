//I2C 3번째 테스트
#include <Wire.h> // using Wire Library
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(11, 12);
int SLAVE[3] = {1, 2, 3};


//BLE SETTING

byte buffer[1024];
int bufferPosition;
//차량용 신호등
int red1 = 2;
int yellow1 = 3;
int green1 = 4;

int red2 = 5;
int yellow2 = 6;
int green2 = 7;

int ju = 0;
//white
int latchPin3 = 9;
int clockPin3 = 8;
int dataPin3 = 10;
byte leds3 = 0;


int sw = A3;


void setup() {
  // Wire 라이브러리 초기화

  Wire.begin();
  // 직렬 통신 초기화
  Serial.begin(9600);
  Serial.println("I2C");
  BTSerial.begin(9600);
  pinMode(red1, OUTPUT);
  pinMode(yellow1, OUTPUT);
  pinMode(green1, OUTPUT);
  pinMode(red2, OUTPUT);
  pinMode(yellow2, OUTPUT);
  pinMode(green2, OUTPUT);
  pinMode(A3, INPUT);
  pinMode(latchPin3, OUTPUT);
  pinMode(dataPin3, OUTPUT);
  pinMode(clockPin3, OUTPUT);
  redLight();
carLightGreen();
  bufferPosition = 0;
}


void loop()
{
  int readValue = digitalRead(17);
  if (readValue == HIGH) {
    offLight();
  }
  else if (readValue == LOW) {
    onLight();
  }
  while (BTSerial.available()) {
    byte readData = BTSerial.read();
    Serial.write(readData);
    buffer[bufferPosition++] = readData;

    if (readData == 'Q') {
      delay(100);
      carLightRed();
      add();
      delay(20000);
    //  digitalWrite(green1, HIGH);
    //  digitalWrite(green2, HIGH);
    }
    else if (readData == 'F') {
      add2();
      carLightRed();
      delay(15000);
      digitalWrite(green1, LOW);
      digitalWrite(green2, LOW);
      Serial.write(readData); 
      delay(500);
    }
    else if (readData == '\n') {
      buffer[bufferPosition] = '\0';
      // BufferPosition을 초기화 합니다.
      bufferPosition = 0;
    }
  }
  // Write data to Slave

  delay(500);
  redLight();
  carLightGreen();
}

/*
  // Read data from Slave
  Wire.requestFrom(3, 6);  // Slave address is 3, Data length is 6
  while(Wire.available())  // Wait data until completed
  {
       Wire.read();    // Get data from buffer
  }

*/
void add() {
  Wire.beginTransmission(1); // Slave address is 1
  Wire.write('A');
  Wire.write('\n');
  Wire.endTransmission();
  delay(500);
  Wire.beginTransmission(2); // Slave address is 1
  Wire.write('A');
  Wire.write('\n');
  Wire.endTransmission();
  delay(500);
  Wire.beginTransmission(3); // Slave address is 1
  Wire.write('A');
  Wire.write('\n');
  Wire.endTransmission();
  delay(500);

}

void add2() {
  Wire.beginTransmission(1); // Slave address is 1
  Wire.write('B');
  Wire.write('\n');
  Wire.endTransmission();
  delay(500);
  Wire.beginTransmission(2); // Slave address is 1
  Wire.write('B');
  Wire.write('\n');
  delay(500);
  Wire.endTransmission();
  Wire.beginTransmission(3); // Slave address is 1
  Wire.write('B');
  Wire.write('\n');
  Wire.endTransmission();


}
void add3() {

  Wire.beginTransmission(1); // Slave address is 1
  Wire.write('C');
  Wire.write('\n');
  Wire.endTransmission();
  delay(500);
  Wire.beginTransmission(2); // Slave address is 1
  Wire.write('C');
  Wire.write('\n');
  Wire.endTransmission();
  delay(500);
  Wire.beginTransmission(3); // Slave address is 1
  Wire.write('C');
  Wire.write('\n');
  Wire.endTransmission();


}
void redLight() {
  Wire.beginTransmission(1); // Slave address is 1
  Wire.write('X');
  Wire.write('\n');
  Wire.endTransmission();
  delay(500);
  Wire.beginTransmission(2); // Slave address is 3
  Wire.write('X');
  Wire.write('\n');
  Wire.endTransmission();
  delay(500);
  // Slave address is 3
  Wire.beginTransmission(3);
  Wire.write('X');
  Wire.write('\n');
  Wire.endTransmission();

}



void carLightRed() {
  carGreenIntoRed();
  digitalWrite(red1, HIGH);
  digitalWrite(red2, HIGH);
  digitalWrite(yellow1, LOW);
  digitalWrite(yellow2, LOW);
  digitalWrite(green1, LOW);
  digitalWrite(green2, LOW);
}

void carGreenIntoRed() {
  delay(800);
  digitalWrite(red1, LOW);
  digitalWrite(red2, LOW);
  digitalWrite(green1, LOW);
  digitalWrite(green2, LOW);
  digitalWrite(yellow1, HIGH);
  digitalWrite(yellow2, HIGH);
  delay(2000);
}
void carLightGreen() {
  delay(1000);
  digitalWrite(red1, LOW);
  digitalWrite(red2, LOW);
  digitalWrite(yellow1, LOW);
  digitalWrite(yellow2, LOW);
  digitalWrite(green1, HIGH);
  digitalWrite(green2, HIGH);
}
void initShiftRegister3() {
  leds3 = 0;
  updateShiftRegister3();
  delay(500);

}
void updateShiftRegister3() {
  //왼쪽 IC
  digitalWrite(latchPin3, LOW);
  shiftOut(dataPin3, clockPin3, LSBFIRST, leds3);
  digitalWrite(latchPin3, HIGH);
}


void onLight() {
  initShiftRegister3();

  for (int i = 0; i < 8; i++)  {
    bitSet(leds3, i);
    updateShiftRegister3();

  }
  delay(5000);

}
void offLight() {
  initShiftRegister3();

}
int i2c_communication(int i) {
  // 12 바이트 크기의 데이터 요청
  Wire.requestFrom(SLAVE[i], 12);
  // 12 바이트 모두 출력할 때까지 반복
  for (int j = 0 ; j < 12 ; j++) {
    // 수신 데이터 읽기
    char c = Wire.read();
    // 수신 데이터 출력
    Serial.print(c);
  }
  Serial.println();

  return 0;
}
