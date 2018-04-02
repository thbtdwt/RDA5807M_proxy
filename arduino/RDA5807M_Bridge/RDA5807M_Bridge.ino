#include <Arduino.h>
#include <Wire.h>


/*****************************************************************************/
/*                               DEBUG PART                                  */
/*****************************************************************************/
bool debug_on=true;
char debug_buffer[256];
int debug_print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(debug_buffer,sizeof(debug_buffer), fmt, args);
    Serial.println(debug_buffer);
    va_end(args);
}

#define DEBUG_PRINT(trace...) do { \
  if ( debug_on )              \
    debug_print(trace);        \
} while (0)


/*****************************************************************************/
/*                                 SPI PART                                  */
/*****************************************************************************/
typedef enum { WAIT_DATA,
               RECEIVING_LENGH,
               RECEIVING_DATA,
               RECEIVING_STOP,
               TRANSMIT_START,
               TRANSMIT_LENGH,
               TRANSMIT_DATA,
               TRANSMIT_STOP,
               UNKNOWN_STATE }spi_state_t;

spi_state_t spi_state = WAIT_DATA;

#define SPI_BUFFER_SIZE 16
unsigned char spi_buffer[SPI_BUFFER_SIZE];
uint8_t spi_buffer_index;


uint8_t msg_lengh = 0;



#define CMD_WRITE_16 'w'
#define CMD_READ_16 'r'


/*
 * Brief: Interpret each Bytes received through SPI
 * Param[in]: Byte from SPI
 */
void spi_handler(unsigned char data)
{
  //Serial.println(data,HEX);
  SPDR = 0;
  switch ( spi_state )
  {
    case WAIT_DATA:
      if ( SPDR == 'S' )
      {
        SPDR = 'a';
        spi_buffer_index = 0;
        spi_state = RECEIVING_LENGH;
        DEBUG_PRINT("S received");
      }
      break;
      
    case RECEIVING_LENGH:
      msg_lengh = data; 
      spi_state = RECEIVING_DATA;  
      Serial.print("length: "); Serial.println(msg_lengh); 
      break;
      
    case RECEIVING_DATA:
      spi_buffer[spi_buffer_index++] = data;
      if ( spi_buffer_index == msg_lengh )
        spi_state = RECEIVING_STOP;    
      break;
      
    case RECEIVING_STOP:
      if ( data != 'E' )
      {
        DEBUG_PRINT("e not received");
        DEBUG_PRINT("Packet error");
        SPDR = 'n';
        spi_state = UNKNOWN_STATE;
      }
      else
      {
        DEBUG_PRINT("e received");
        SPDR = 'a';
        if ( process_buffer(spi_buffer) )
          spi_state = TRANSMIT_START;
        else
          spi_state = WAIT_DATA;
      }
      break;

    case TRANSMIT_START:
      SPDR = 's';
      spi_buffer_index = 0;
      spi_state = TRANSMIT_LENGH;
      break;

    case TRANSMIT_LENGH:
      SPDR = msg_lengh;
      spi_state = TRANSMIT_DATA;
      break;
    
    case TRANSMIT_DATA:
      SPDR = spi_buffer[spi_buffer_index++];
      //Serial.print("send"); Serial.println(spi_buffer[spi_buffer_index-1],HEX);
      DEBUG_PRINT("send 0x%x", spi_buffer[spi_buffer_index-1]);
      if ( spi_buffer_index == msg_lengh )
        spi_state = TRANSMIT_STOP;
      break;   
    
    case TRANSMIT_STOP:
      SPDR = 'e';
      spi_state = WAIT_DATA;
      break;    
      
    case UNKNOWN_STATE:
    default:
        if ( data == 0 )
          spi_state = WAIT_DATA; 
  }
}

/*****************************************************************************/
/*                                 I2C PART                                  */
/*****************************************************************************/

inline void uint16_to_uint8(uint16_t val_16, uint8_t* val_H, uint8_t* val_L)
{
  *val_H = val_16 >> 8;
  *val_L = val_16 & 0xFF;
}

#define I2C_INDX 0x11

uint16_t _read16(void)
{
  uint8_t hiByte = Wire.read();
  uint8_t loByte = Wire.read();
  return(256*hiByte + loByte);
}

void _write16(uint16_t val_H, uint16_t val_L)
{
  Wire.write(val_H);
  Wire.write(val_L);
}

uint16_t read_regiter(uint8_t add)
{
  Wire.beginTransmission(I2C_INDX); 
  Wire.write(add); 
  Wire.endTransmission(0); 
  Wire.requestFrom (I2C_INDX, 2, 1 );
  uint16_t res = _read16();
  Wire.endTransmission();
  return res;
}

void write_regiter(uint8_t add, uint16_t val_H, uint16_t val_L)
{
  Wire.beginTransmission(I2C_INDX);                
  Wire.write(add);
  _write16(val_H, val_L);
  Wire.endTransmission();
}


/*****************************************************************************/
/*                               BRIDGE PART                                 */
/*****************************************************************************/

/*
 * Brief: Interpret a message
 * Param[in]: message
 * Return: True if an answer has to be send.
 */
bool process_buffer(unsigned char* msg)
{
  uint8_t i2c_register_addr;
  uint8_t i2c_register_value_H;
  uint8_t i2c_register_value_L;
  switch ( msg[0] )
  {
    case CMD_WRITE_16:
      i2c_register_addr    = msg[1];
      i2c_register_value_H = msg[2];
      i2c_register_value_L = msg[3];
      DEBUG_PRINT("Write");
      Serial.println(i2c_register_addr,HEX);
      Serial.println(i2c_register_value_H,HEX);
      Serial.println(i2c_register_value_L,HEX);
      write_regiter(i2c_register_addr, i2c_register_value_H, i2c_register_value_L);
      break;

    case CMD_READ_16:
      i2c_register_addr    = msg[1];
      DEBUG_PRINT("Read");
      /*Serial.print(i2c_register_addr,HEX);
      spi_buffer[0]=6;
      spi_buffer[1]=9;*/
      uint16_to_uint8(read_regiter(i2c_register_addr), &spi_buffer[0], &spi_buffer[1]);
      msg_lengh = 2;
      return true;
      
    default:
      return false;
  }
  return false;
}



/*****************************************************************************/
/*                                 MAIN PART                                  */
/*****************************************************************************/
                    
void setup (void)
{
  Serial.begin(57600);

  /* Setup SPI in slave mode */
  pinMode(MISO, OUTPUT);
  SPCR |= _BV(SPE);

  /* I2c Init */
  Wire.begin();
}  


void loop (void)
{
  if((SPSR & (1 << SPIF)) != 0)
  {
    spi_handler(SPDR);
  }
}






