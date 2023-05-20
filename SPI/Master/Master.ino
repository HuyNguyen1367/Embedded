/*
* File: MASTER.ino
* Author: Nguyen Huy
* Date: 15/05/2023
* Description: This is master's 1-byte transmission simulation program
*/

//Mode 1 : CPOL = 0,CPHASE = 0

//Cài đặt thời gian ngắt của mỗi xung trong SCK
#define TIME 50 //T = 50us

//Khai báo các chân của bên MASTER
#define SCK_PIN   4
#define MOSI_PIN  5
#define MISO_PIN  6
#define SS_PIN    7

#define SCK_OUTPUT   DDRD |= (1 << DDD4)  //pinMode(SCK_PIN, OUTPUT) 
#define MOSI_OUTPUT  DDRD |= (1 << DDD5)
#define SS_OUTPUT    DDRD |= (1 << DDD7)
#define MISO_INPUT   DDRD &=~(1 << DDD6)  //pinMode(MISO_PIN, INPUT)

//if   x = HIGH <=> digtalWrite(MOSI_PIN,HIGH)
//else x = LOW  <=> digtalWrite(MOSI_PIN,LOW)
#define WRITE_MOSI(x)   PORTD = ((x) ? (PORTD | 1<<PD5) : (PORTD & (~(1<<PD5))) )
#define WRITE_SS(x)     PORTD = ((x) ? (PORTD | 1<<PD7) : (PORTD & (~(1<<PD7))) )
#define WRITE_SCK(x)    PORTD = ((x) ? (PORTD | 1<<PD4) : (PORTD & (~(1<<PD4))) )

//digitalRead(MISO_PIN)
// Nếu tín hiệu tại chân D6 = 1 thì giá trị trả về là mức HIGH ,D6 = 0 ==> giá trị trả về là LOW
#define READ_MISO()  ( (PIND & (1 << PIND6)) ? HIGH:LOW)

void setup() {
  MOSI_OUTPUT;
  MISO_INPUT;
  WRITE_SCK(LOW);
  SCK_OUTPUT ;
  WRITE_SS(HIGH);
  SS_OUTPUT;
  delay(10);
}

void loop() {
  uint8_t res;
  SPI_start();
  res = SPI_transfer('a'); //a = 0x61
  SPI_end();
  delay(1000);
}

void SPI_setup()
{
  MOSI_OUTPUT;
  MISO_INPUT;
  WRITE_SCK(LOW);
  SCK_OUTPUT ;
  WRITE_SS(HIGH);
  SS_OUTPUT;
  delay(10);
}

void SPI_start()
{
  WRITE_SS(LOW); // Cho tín hiệu của chân SS xuống mức LOW để bắt đầu chương trình
}

void SPI_end()
{
  WRITE_SCK(LOW);
  WRITE_SS(HIGH); // Cho tín hiệu của chân SS xuống mứcHIGH để kết thúc chương trình
}

uint8_t SPI_transfer(uint8_t byte_Out)
{
  uint8_t byte_In = 0; // 0000.0000
  uint8_t ibit,res;

  for(ibit=0x80;ibit>0;ibit = ibit>>1) //0x80 = 1000.0000
  {
    res = byte_Out & ibit;
    WRITE_MOSI(res); // truyền dữ liệu 1 bit qua chân MOSI ,dữ liệu được gửi đi và giữ trong nguyên 1 chu kì SCK 
    delayMicroseconds(TIME);
    WRITE_SCK(HIGH);
    if(READ_MISO() == HIGH)
      byte_In = byte_In | ibit;  //Lấy giá trị từ bên SLAVE
    delayMicroseconds(TIME);
    WRITE_SCK(LOW);
  }

  return byte_In;
}
