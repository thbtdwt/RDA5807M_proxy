#include <Arduino.h>
#include <Wire.h>
#include "RDA5807M_msg_def.h"

/*****************************************************************************/
/*                               DEBUG PART                                  */
/*****************************************************************************/
bool debug_on=false;
static char debug_buffer[256];
static int debug_print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(debug_buffer,sizeof(debug_buffer), fmt, args);
    va_end(args);
    Serial.println(debug_buffer);
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
               PROCESSING_DATA,
               TRANSMIT_START,
               TRANSMIT_LENGH,
               TRANSMIT_DATA,
               TRANSMIT_STOP,
               UNKNOWN_STATE }spi_state_t;

/*
 * Brief: spi state
 */
spi_state_t spi_state = WAIT_DATA;

/*
 * Brief: spi buffer and index 
 */
static unsigned char spi_buffer[RDA5807M_BRIDGE_MAX_MSG_SIZE];
static uint8_t spi_buffer_index;

/*
 * Brief: lengh of the message
 */
uint8_t msg_lengh = 0;


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
      if ( SPDR == RDA5807M_PROXY_MSG_START )
      {
        SPDR = RDA5807M_BRIDGE_MSG_ACK;
        spi_buffer_index = 0;
        spi_state = RECEIVING_LENGH;
        DEBUG_PRINT("S received");
      }
      break;
      
    case RECEIVING_LENGH:
      msg_lengh = data; 
      spi_state = RECEIVING_DATA;
      DEBUG_PRINT("Msg length %u", msg_lengh);   
      break;
      
    case RECEIVING_DATA:
      spi_buffer[spi_buffer_index++] = data;
      if ( spi_buffer_index == msg_lengh + 1 )
        spi_state = RECEIVING_STOP;    
      break;
      
    case RECEIVING_STOP:
      if ( data != RDA5807M_PROXY_MSG_STOP )
      {
        DEBUG_PRINT("Packet error %c not received", RDA5807M_PROXY_MSG_STOP);
        SPDR = RDA5807M_BRIDGE_MSG_NACK;
        spi_state = UNKNOWN_STATE;
      }
      else
      {
        DEBUG_PRINT("%c received", RDA5807M_PROXY_MSG_STOP);
        SPDR = RDA5807M_BRIDGE_MSG_ACK;
        spi_state = PROCESSING_DATA;
      }
      break;

    case PROCESSING_DATA:
      if ( process_buffer(spi_buffer) )
        spi_state = TRANSMIT_START;
      else
        spi_state = WAIT_DATA;
      break;

    case TRANSMIT_START:
      SPDR = RDA5807M_BRIDGE_MSG_START;
      spi_buffer_index = 0;
      spi_state = TRANSMIT_LENGH;
      break;

    case TRANSMIT_LENGH:
      SPDR = msg_lengh;
      spi_state = TRANSMIT_DATA;
      break;
    
    case TRANSMIT_DATA:
      SPDR = spi_buffer[spi_buffer_index++];
      DEBUG_PRINT("SPI Send [0x%02x]", spi_buffer[spi_buffer_index-1]);
      if ( spi_buffer_index == msg_lengh )
        spi_state = TRANSMIT_STOP;
      break;   
    
    case TRANSMIT_STOP:
      SPDR = RDA5807M_BRIDGE_MSG_STOP;
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
#define I2C_INDX 0x11

/*
 * Brief: Read  16 bit value 
 * Return: 16 bit value
 */
uint16_t _read16(void)
{
  uint8_t hiByte = Wire.read();
  uint8_t loByte = Wire.read();
  return(256*hiByte + loByte);
}

/*
 * Brief: Write 16 bit value 
 * Param[in]: high value
 * Param[in]: low value
 */
void _write16(uint16_t val_H, uint16_t val_L)
{
  Wire.write(val_H);
  Wire.write(val_L);
}

/*
 * Brief: Read register
 * Param[in]: register address
 * Return: 16 bit value
 */
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

/*
 * Brief: Write 16 bit value in register 
 * Param[in]: register address
 * Param[in]: high value
 * Param[in]: low value
 */
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
 * Brief: Extract uint16_t to two uint8_t
 * Param[in]: uint16_t value
 * Param[out]: uint8_t high value
 * Param[out]: uint8_t low value
 */
inline void uint16_to_uint8(uint16_t val_16, uint8_t* val_H, uint8_t* val_L)
{
  *val_H = val_16 >> 8;
  *val_L = val_16 & 0xFF;
}

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
    case RDA5807M_PROXY_MSG_WRITE:
      i2c_register_addr    = msg[1];
      i2c_register_value_H = msg[2];
      i2c_register_value_L = msg[3];
      DEBUG_PRINT("I2C Write [0x%02x][0x%02x][0x%02x]", i2c_register_addr, i2c_register_value_H, i2c_register_value_L);
      write_regiter(i2c_register_addr, i2c_register_value_H, i2c_register_value_L);
      break;

    case RDA5807M_PROXY_MSG_READ:
      i2c_register_addr    = msg[1];
      DEBUG_PRINT("I2C Read regiter [0x%02x]", i2c_register_addr);
      uint16_to_uint8(read_regiter(i2c_register_addr), &spi_buffer[0], &spi_buffer[1]);
      msg_lengh = RDA5807M_BRIDGE_MSG_PAYLOAD_SIZE;
      return true;
      
    default:
      return false;
  }
  return false;
}


/*****************************************************************************/
/*                                 MAIN PART                                  */
/*****************************************************************************/

/*
 * Brief: Setup
 */
void setup (void)
{
  Serial.begin(57600);

  /* Setup SPI in slave mode */
  pinMode(MISO, OUTPUT);
  SPCR |= _BV(SPE);

  /* I2c Init */
  Wire.begin();
}  

/*
 * Brief: Main Loop
 */
void loop (void)
{
  if((SPSR & (1 << SPIF)) != 0)
  {
    spi_handler(SPDR);
  }
}






