
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

/*
Use this funtion------------
- I2C_begin()
- I2C_write()
*/
void setup() {
  Serial.begin(9600);
  delay(100);

}

void loop() {
  byte rev[10]={0};
  int8_t s,c;

  s = I2C_begin(0x55);
  if(s == 0) //Master write - Slave read
  {
    I2C_read(rev , 8);
    Serial.println("Master wants to write data");
    Serial.println(String((char*)rev));
  }
  else if(s == 1) //Master read - Slave write
  {
    c = I2C_write("a");
    Serial.println("Master wants to read data");
    if(c == 1)       Serial.println("Done to Write");
    else if(c == -1) Serial.println("Fail to Write");
    else             Serial.println("Error");
  }
     
  else if(s == 2)
     Serial.println("ERROR ADDRESS");
  else
     Serial.println("Other uncontrol case");
  //delay(100);

}

uint8_t I2C_begin(byte address)
{
  uint8_t data = 0,rw = 0,i;
  while(true)
  {
    SDA_INPUT;
    SCL_INPUT;
    while( (SDA_READ != 1) || (SCL_READ != 1) );
    while(SDA_READ == 1);
    delayMicroseconds(TIME/2);
    if(SCL_READ == 1)
    {
      while( SCL_READ == 1 );
//read address
      for(i=0;i<8;i++)
      {
        while(SCL_READ == 0);
        data = (data<<1) | SDA_READ;
        while(SCL_READ == 1);
      }

      rw = data & 1;
      data = data>>1;
      if(data == address)
      {
        I2C_SendACK();
        return rw;
      }
      else
        return 2;

    }
    else
      {}
  }
}

byte I2C_ReadByte(void)
{
  byte data,i;
  SDA_INPUT;
  SCL_INPUT;

  for(i=0;i<8;i++)
  {
    while(SCL_READ == 0); // Waaiting util SCL = 1
    data = (data<<1) | SDA_READ;
    while(SCL_READ == 1); // Waaiting util SCL = 0
  }
  SDA_OUTPUT;
  SDA_LOW; //send ACK to Master
  while(SCL_READ == 0);
  while(SCL_READ == 1);
  SDA_INPUT;
  return data;
}

void I2C_SendACK(void)
{
  SDA_OUTPUT;
  SDA_LOW;
  while(SCL_READ == 0);
  while(SCL_READ == 1);
  SDA_INPUT;
}

uint8_t I2C_ReadACK(void)
{
  uint8_t ack;
  SDA_INPUT;
  while(SCL_READ == 0);
  ack = SDA_READ;
  while(SCL_READ == 1);
  return ack;
}

//slave send 8 bit after master receives
void I2C_WriteByte(byte data)
{
  SDA_OUTPUT;
  for(int i=0;i<8;i++)
  {
    if( (data & 0x80) != 0)
    {
      SDA_HIGH;
    }
    else
    {
      SDA_LOW;
    }
    data = data << 1;
    while(SCL_READ == 0);
    while(SCL_READ == 1);
  }
}

void I2C_read(byte *data, uint8_t count)
{
  for(int i=0;i<count;i++)
  {
    *data = I2C_ReadByte(); //data[0]
    data++;
  }
}

int8_t I2C_write(char *data) //"abc" = 'a' + 'b' + 'c'
{
  uint8_t ack; //0:ack, 1:nack

  while(*data != '\0')
  {
    I2C_WriteByte(*data);
    ack = I2C_ReadACK();
    if(ack == 1)
    {
      SDA_INPUT;
      return -1; //fail
    }
    data++;
  }
  SDA_INPUT;
  return 1; //Succeces
}




