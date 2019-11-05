//SLAVE 1

#include <Wire.h>
#define SLAVE 1
#include <TM74HC595Display.h>



int SCLK2 = A2;
int RCLK2 = A1;
int DIO2 = A0;

byte count = 0;
char temp;
int stlight = 0;
int number = 20; // <--- Change it from 0 to 99


TM74HC595Display disp2(SCLK2, RCLK2, DIO2);
unsigned char LED_0F[29];



//신호등 모듈 무조건 왼쪽부터

//신호등왼쪽
int red1 = 2;
int green1 = 3;
int buzz1 = 4;
int motion1 = 5;

//신호등오른쪽
int red2 = 12;
int green2 = 11;
int buzz2 = 10;
int motion2 = 9;

//모션
int pirState1 = LOW; // PIR 초기상태
int val1 = 0; // Signal 입력값
int pirState2 = LOW; // PIR 초기상태
int val2 = 0; // Signal 입력값

void setup() {
  LED_0F[0] = 0xC0; //0
  LED_0F[1] = 0xF9; //1
  LED_0F[2] = 0xA4; //2
  LED_0F[3] = 0xB0; //3
  LED_0F[4] = 0x99; //4
  LED_0F[5] = 0x92; //5
  LED_0F[6] = 0x82; //6
  LED_0F[7] = 0xF8; //7
  LED_0F[8] = 0x80; //8
  LED_0F[9] = 0x90; //9
  LED_0F[10] = 0x88; //A
  LED_0F[11] = 0x83; //b
  LED_0F[12] = 0xC6; //C
  LED_0F[13] = 0xA1; //d
  LED_0F[14] = 0x86; //E
  LED_0F[15] = 0x8E; //F
  LED_0F[16] = 0xC2; //G
  LED_0F[17] = 0x89; //H
  LED_0F[18] = 0xF9; //I
  LED_0F[19] = 0xF1; //J
  LED_0F[20] = 0xC3; //L
  LED_0F[21] = 0xA9; //n
  LED_0F[22] = 0xC0; //O
  LED_0F[23] = 0x8C; //P
  LED_0F[24] = 0x98; //q
  LED_0F[25] = 0x92; //S
  LED_0F[26] = 0xC1; //U
  LED_0F[27] = 0x91; //Y
  LED_0F[28] = 0xFE; //hight -
  // Wire 라이브러리 초기화
  // 슬레이브로 참여하기 위해서는 주소를 지정해야 한다.
  Serial.begin(9600);
  ;
  Wire.begin(SLAVE);
  Wire.onReceive(receiveFromMaster);
  // 마스터의 데이터 전송 요구가 있을 때 처리할 함수 등록
  // Wire.onRequest(sendToMaster);

  //신호등
  pinMode(red1, OUTPUT);
  pinMode(red2, OUTPUT);
  pinMode(green1, OUTPUT);
  pinMode(green2, OUTPUT);

  //모션+피에조
  pinMode(motion1, INPUT); // 센서 Input 설정
  pinMode(motion2, INPUT); // 센서 Input 설정
  pinMode(buzz1, OUTPUT);
  pinMode(buzz2, OUTPUT);
  digitalWrite(buzz1, LOW);
  digitalWrite(buzz2, LOW);
  RedLight();

  disp2.send(LED_0F[0], 0b0001);
  stlight = 0;

}

void loop() {

  // 'O' 요청이 들어오면 녹색불 점등

  if (temp == 'A') {
    stlight = 1;
    number = 20;
    GreenLight();
    countDown();
    //Serial.println(temp);
  }
  else if (temp == 'B') {
    GreenLight();
    number = 15;
    countDown();
    // Serial.println(temp);
  }
  else if (temp == 'X') {
    RedLight();
    number = 20;
    stlight = 0;
    Serial.println(temp);
  }
  //조도센서 함수
  //인체감지 함수 빨간불일때만
  stlight = 0;
  motion();
}
void RedLight() {
  digitalWrite(green1, LOW);
  digitalWrite(green2, LOW);
  digitalWrite(red1, HIGH);
  digitalWrite(red2, HIGH);
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
  Wire.write("Main Traffic");
}
void motion() {
  val1 = digitalRead(motion1);
  val2 = digitalRead(motion2);
  if (stlight == 0) {
    if (val1 == HIGH) {
      digitalWrite(buzz1, HIGH);
      Serial.println("Motion detected1!");
      delay(2000);
      if (pirState1 == LOW) {
        // 시리얼모니터에 메시지 출력
        pirState1 = HIGH;
        digitalWrite(buzz1, LOW);
        delay(1000);
      }
    }
    else if (val2 == HIGH) {
      digitalWrite(buzz2, HIGH);
      Serial.println("Motion detected2!");
      delay(2000);
      if (pirState2 == LOW) {
        // 시리얼모니터에 메시지 출력
        pirState2 = HIGH;
        digitalWrite(buzz2, LOW);
        delay(2000);
      }
    }
    else {
      digitalWrite(buzz1, LOW);
      digitalWrite(buzz2, LOW);
      if (pirState1 == HIGH) {
        // 시리얼모니터에 메시지 출력
        Serial.println("Motion ended1!");
        pirState1 = LOW;
        delay(6000);
      }
      if (pirState2 == HIGH) {
        Serial.println("Motion ended2!");
        pirState2 = LOW;
        delay(6000);
      }
      delay(2000);
    }
  }
  else {
    digitalWrite(buzz1, LOW);
    digitalWrite(buzz2, LOW);
    if (pirState1 == HIGH || pirState2 == HIGH) {
      // 시리얼모니터에 메시지 출력
      Serial.println("Motion ended!");
      pirState1 = LOW;
      pirState2 = LOW;
      delay(6000);
    }
  }
}
void countDown() {
  for (number; number >= 0; number--) {
    disp2.digit2(number, 0b0100, 2300);
  }
  disp2.send(LED_0F[0], 0b0001);    //send digital "0" to 1st indicator
}

void GreenLight() {
  digitalWrite(red1, LOW);
  digitalWrite(red2, LOW);
  digitalWrite(green1, HIGH);
  digitalWrite(green2, HIGH);
}
