/*
* File: MASTER.ino
* Author: Nguyen Huy
* Date: 15/05/2023
* Description: This is slave 1-byte transmission simulation program
*/

//Mode 1 : CPOL = 0,CPHASE = 0

//Cài đặt thời gian ngắt của mỗi xung trong SCK
#define TIME 50 //T = 50us

//Khai báo các chân của bên SLAVE
#define SCK_PIN   4
#define MOSI_PIN  5
#define MISO_PIN  6
#define SS_PIN    7

#define SCK_INPUT     DDRD &=~(1 << DDD4)  //pinMode(SCK_PIN, INPUT) 
#define MOSI_INPUT    DDRD &=~(1 << DDD5)
#define SS_INPUT      DDRD &=~(1 << DDD7)
#define MISO_OUTPUT   DDRD |= (1 << DDD6)  //pinMode(MISO_PIN, OUTPUT)

#define READ_SCK()   ( (PIND & (1 << PIND4)) ? HIGH:LOW)
#define READ_MOSI()  ( (PIND & (1 << PIND5)) ? HIGH:LOW)
#define READ_SS()    ( (PIND & (1 << PIND7)) ? HIGH:LOW)

#define WRITE_MISO(x)   PORTD = ((x) ? (PORTD | 1<<PD6) : (PORTD & (~(1<<PD6))) )

void setup() {
  SPI_setup();
}

void loop() {
  uint8_t res;
  res = SPI_transfer('A');
}

void SPI_setup()
{
  SCK_INPUT;
  MOSI_INPUT;
  SS_INPUT ;
  MISO_OUTPUT;
  delay(10);
}

uint8_t SPI_transfer(uint8_t byte_Out)
{
  uint8_t byte_In = 0; // 0000.0000
  uint8_t ibit,res;

  while(READ_SS() == HIGH); // Nếu chân SS còn ở mức cao thì chương trình sẽ ko chạy

  for(ibit=0x80;ibit>0;ibit>>1) //0x80 = 1000.0000
  {
    res = byte_Out & ibit;
    WRITE_MISO(res);

    while(READ_SCK() == LOW);

    if(READ_MOSI() == HIGH)
      byte_In = byte_In | ibit;  //Lấy giá trị từ bên MASTER

    while(READ_SCK() == HIGH);
  }

  return byte_In;
}
