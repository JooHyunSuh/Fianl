//slave3
#include <Wire.h>


#define SLAVE 3

byte count = 0;
char temp;
//신호등 모듈 무조건 왼쪽부터


//LED1
int latchPin1 = 3;
int clockPin1 = 2;
int dataPin1 = 4;
byte leds1 = 0;

//LED2
int latchPin2 = 11;
int clockPin2 = 10;
int dataPin2 = 12;
byte leds2 = 0;



void setup() {

  Serial.begin(9600);
  Serial.println("Hello!");

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

  redLight();
  
}

void loop() {

  // 'O' 요청이 들어오면 녹색불 점등
  if (temp == 'A') {
    greenLight();
    delay(25000);
  }
  else if (temp == 'B') {
    greenLight();
    delay(18000);
  }
   else if (temp == 'X') {
   redLight();
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
  Wire.write("Street Traffic");
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
  for (int i = 0; i < 8; i++)  {
    bitSet(leds1, i);
    updateShiftRegister1();
    bitSet(leds2, i);
    updateShiftRegister2();
    delay(500);
  }
}


void redLight() {
  initShiftRegister1();
  initShiftRegister2();
}
