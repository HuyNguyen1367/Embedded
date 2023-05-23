//#include <Wire.h>

#define TIME 50

#define SCL_PIN 3
#define SDA_PIN 4

#define SDA_OUTPUT DDRD |= (1 << DDB4)
#define SCL_OUTPUT DDRD |= (1 << DDB3)

#define SDA_INPUT  DDRD &= ~(1 << DDB4)
#define SCL_INPUT  DDRD &= ~(1 << DDB3)

#define SDA_HIGH   PORTD |=  (1 << PD4)  //pinMode(SDA_PIN, OUTPUT) 
#define SCL_HIGH   PORTD |=  (1 << PD3)  //pinMode(SCL_PIN, OUTPUT)
#define SDA_LOW    PORTD &= ~(1 << PD4)  //pinMode(SDA_PIN, INPUT)
#define SCL_LOW    PORTD &= ~(1 << PD3)  //pinMode(SCL_PIN, INPUT)

#define SDA_READ   (PIND & (1 << PIND4)) >> PIND4
#define SCL_READ   (PIND & (1 << PIND3)) >> PIND3


void setup() {
  Serial.begin(9600);
  delay(1000);
}

void loop() {

  // int8_t res;
  // res = I2C_WriteData(0x55,"Hello.");
  // Serial.println(res);
  
  byte rev[10]={0};
  int8_t res;
  res = I2C_ReadData(0x55 , rev, 1);
  if(res == 1)
  {
    Serial.println("Read done");
    Serial.println(String((char*)rev));
  }
  else if(res == -1)
  {
    Serial.println("Read fail");
  }
  
  delay(100);
}

////////////////////////////////////////////////////////////////

int8_t I2C_ReadData(byte address , byte *data , byte count)
{
  int8_t ACK;
  I2C_StratCondition();
  address =(address<<1) + 1;
  I2C_Write8bit(address);
  //check ACK------------
  ACK = I2C_ReadACK();
  if(ACK == 1)
  {
    I2C_StopCondition();
    return -1;
  }

  //read count byte data
  for (int i=0;i<count;i++)
  {
    *data = I2C_Read8bit();
    data++;
    I2C_SendACK;
  }

  //stop
  I2C_StopCondition();
  return 1;
}

int8_t I2C_WriteData(byte address, char *data)
{
  int8_t ACK;
  I2C_StratCondition();
  //send address(7bit) + W(=0) --7bitA+0--
  address = address<<1;
  I2C_Write8bit(address);
  //check ACK----------------
  ACK = I2C_ReadACK();
  if(ACK == 1)
  {
    I2C_StopCondition();
    return -1;
  }
  //send data ----------
  while(*data != '\0')
  {
    I2C_Write8bit(*data);
    ACK = I2C_ReadACK(); //check ACK
    if(ACK == 1)
    {
      I2C_StopCondition();
      return -1;
    }
    data++; //data+1 = data[1]
  }
  //stop---------
  I2C_StopCondition();
  return 1;
}

void I2C_StratCondition(void)
{
  SDA_HIGH;
  SCL_HIGH;
  SDA_OUTPUT;
  SCL_OUTPUT;
  delayMicroseconds(TIME*2);
  //START--------------------------
  SDA_LOW;
  delayMicroseconds(TIME);
  SCL_LOW;
  delayMicroseconds(TIME);
}

void I2C_StopCondition(void)
{
  SCL_LOW;
  SDA_OUTPUT;
  SDA_LOW;
  delayMicroseconds(TIME);
  SCL_HIGH;
  delayMicroseconds(TIME);
  SDA_HIGH;
}

void I2C_Write8bit(byte data)
{
  SDA_OUTPUT;
  for(int i=0;i<8;i++)
  {
    if( (data & 0x80) != 0)
       SDA_HIGH;
    else
       SDA_LOW;
    data = data << 1;
    delayMicroseconds(TIME);
    SCL_HIGH;
    delayMicroseconds(TIME);
    SCL_LOW;
  }
}

byte I2C_Read8bit(void)
{
  byte data = 0;
  SDA_INPUT;
  SCL_OUTPUT;
  SCL_LOW;
  //Read 8bbit data----------------
  for(int i=0;i<8;i++)
  {
    delayMicroseconds(TIME);
    SCL_HIGH;
    data = (data<<1) | SDA_READ;
    delayMicroseconds(TIME);
    SCL_LOW;
  }
  return data;
}

uint8_t I2C_ReadACK(void)
{
  uint8_t ACK;
  //Read ACK/NACK
  SDA_INPUT;
  delayMicroseconds(TIME);
  SCL_HIGH;
  ACK = SDA_READ;
  delayMicroseconds(TIME);
  SCL_LOW;
  delayMicroseconds(TIME);
  return ACK;
}

void I2C_SendACK(void)
{
  SCL_LOW;
  SDA_OUTPUT;
  SDA_LOW; //send ACK
  delayMicroseconds(TIME);
  SCL_HIGH;
  delayMicroseconds(TIME);
  SCL_LOW;
  SDA_INPUT;
  delayMicroseconds(TIME);
}

uint8_t I2C_WriteByte(byte data)
{
  SDA_HIGH;
  SCL_HIGH;
  SDA_OUTPUT;
  SCL_OUTPUT;
  delayMicroseconds(TIME*2);
  //START--------------------------
  SDA_LOW;
  delayMicroseconds(TIME);
  SCL_LOW;
  delayMicroseconds(TIME);

  for(int i=0;i<8;i++)
  {
    if((data & 0x80) != 0)
       SDA_HIGH;
    else
       SDA_LOW;
    data = data << 1;
    delayMicroseconds(TIME);
    SCL_HIGH;
    delayMicroseconds(TIME);
    SCL_LOW;
  }

  //Read ACK/NACK
  uint8_t ACK;
  SDA_INPUT;
  delayMicroseconds(TIME);
  SCL_HIGH;
  delayMicroseconds(TIME);
  ACK = SDA_READ ;
  SDA_OUTPUT;
  SCL_LOW;
  //delayMicroseconds(TIME);

  //Stop
  SDA_LOW;
  delayMicroseconds(TIME);
  SCL_HIGH;
  delayMicroseconds(TIME);
  SDA_HIGH;
  
  return ACK;
}



