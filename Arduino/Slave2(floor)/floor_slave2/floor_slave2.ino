//slave2
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>


#define SLAVE 2

SoftwareSerial BTSerial(2, 3);
byte buffer[1024];
int bufferPosition;

// 자신의 슬레이브 주소를 설정해 줍니다.(슬레이브 주소에 맞게 수정해야 합니다.)
byte count = 0;
char temp;


//신호등 모듈 무조건 왼쪽부터

//LED1
int latchPin1 = 5;
int clockPin1 = 4;
int dataPin1 = 6;
byte leds1 = 0;

//LED2
int latchPin2 = 12;
int clockPin2 = 11;
int dataPin2 = 13;
byte leds2 = 0;

//차단기
int servoPin11 = 7;


int servoPin22 = 10;

Servo servo11;


Servo servo22;
int angle = 0; // servo position in degrees

void setup() {
  BTSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Hello!");

  bufferPosition = 0;
  // Wire 라이브러리 초기화
  // 슬레이브로 참여하기 위해서는 주소를 지정해야 한다.
  Wire.begin(SLAVE);
  Wire.onReceive(receiveFromMaster);
  // 마스터의 데이터 전송 요구가 있을 때 처리할 함수 등록
  Wire.onRequest(sendToMaster);

  //LED를 위한 IC 세팅
  pinMode(latchPin1, OUTPUT);
  pinMode(dataPin1, OUTPUT);
  pinMode(clockPin1, OUTPUT);
  pinMode(latchPin2, OUTPUT);
  pinMode(dataPin2, OUTPUT);
  pinMode(clockPin2, OUTPUT);

  //차단기
  servo11.attach(servoPin11);
  servo22.attach(servoPin22);
  redLight();
  angle = 0;
  servo11.write(angle);


  servo22.write(angle);

}

void loop() {


  // 'O' 요청이 들어오면 녹색불 점등
  if (temp == 'A') {
    BTSerial.println("AT+MINO0x00C8");
    motorup();
    greenLight();
    delay(23000);
  }
  else if (temp == 'B') {
    BTSerial.println("AT+MINO0x0190");
    greenLight();
    delay(18000);
  }
  else if (temp == 'X') {
    BTSerial.println("AT+MINO0x0064");
    redLight();
  }
 // BTSerial.println("AT+MINO0x0064");

  while (BTSerial.available()) {
    byte data = BTSerial.read();
    Serial.write(data);
  }

  while (Serial.available()) {
    byte data = Serial.read();
    BTSerial.write(data);
  }
}
//I2C 통신
void receiveFromMaster(int bytes) {
  char ch[2];
  for (int i = 0 ; i < bytes ; i++) {
    // 수신 버퍼 읽기
    ch[i] = Wire.read();
  }
  temp = ch[0];
}
void sendToMaster() {
  // 자신의 슬레이브 주소를 담은 메세지를 마스터에게 보냅니다. 슬레이브 주소에 맞게 수정해야 합니다.
  Wire.write("FLOOR Traffic");
}

//IC
void updateShiftRegister1() {
  //왼쪽 IC
  digitalWrite(latchPin1, LOW);
  shiftOut(dataPin1, clockPin1, LSBFIRST, leds1);
  digitalWrite(latchPin1, HIGH);
}
void updateShiftRegister2() {
  //오른쪽 IC
  digitalWrite(latchPin2, LOW);
  shiftOut(dataPin2, clockPin2, LSBFIRST, leds2);
  digitalWrite(latchPin2, HIGH);
}
void initShiftRegister1() {
  leds1 = 0;
  updateShiftRegister1();
  delay(50);
}
void initShiftRegister2() {
  leds2 = 0;
  updateShiftRegister2();
  delay(50);
}

void greenLight() {
  initShiftRegister1();
  initShiftRegister2();
  for (int i = 3; i < 6; i++)  {
    bitSet(leds1, i);
    bitSet(leds2, i);
    updateShiftRegister1();
    updateShiftRegister2();
  }
}

void redLight() {
  initShiftRegister1();
  initShiftRegister2();
  for (int i = 0; i < 3; i++) {
    bitSet(leds1, i);
    bitSet(leds2, i);
    updateShiftRegister1();
    updateShiftRegister2();
  }
  motordown();
  delay(4000);
}
//차단기 올라가는 함수
void motorup() {
  if (angle == 0) {
    for (angle = 0; angle < 101; angle++)
    {
      servo11.write(angle);
      servo22.write(angle);
      delay(20);
    }

    angle == 101;
    servo11.write(angle);
    servo22.write(angle);
   
  }

}
//차단기 내려가는 함수
void motordown() {
  if (angle == 101) {
    for (angle = 100; angle > 0; angle--)
    {
      servo11.write(angle);
      servo22.write(angle);
      delay(20);
    }
    angle == 0;
    servo11.write(angle);
    servo22.write(angle);
   
  }
}
