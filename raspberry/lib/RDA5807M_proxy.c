/* RDA5807M_proxy
 * Brief: Api to communication with Arduino RDA5807M bridge.
 *
 * Evolution 18-Feb-2018   Thibaut .Creation
             25 Mar-2018   Thibaut .Add write support
 */

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <RDA5807M_proxy.h>
#include "RDA5807M_trace.h"

#define MAX_SPI_MESSAGE_SIZE 10

/*
 * Brief spi variables
 */
static int spi_fd;
static unsigned int spi_speed_hz = 1000000;
static unsigned int spi_delay_us = 10000;

/*
 * Brief: decode the bridge message ant extract the register value
 * Param[in]: Bytes to send
 * Param[out]: Received Bytes 
 * Return: 0 if ok
 */
static int spi_transfer(unsigned char tx_Bytes, unsigned char* rx_Bytes)
{
  struct spi_ioc_transfer spi;
  memset (&spi, 0, sizeof (spi));

  spi.tx_buf        = (unsigned long)&tx_Bytes;
  spi.rx_buf        = (unsigned long)rx_Bytes;
  spi.len           = 1;

  if ( 0 > ioctl (spi_fd, SPI_IOC_MESSAGE(1), &spi) )
  {
    fprintf(stderr, "'%m' (%d)'\n", errno);
    return errno;
  }
  return 0;
}

/*
 * Brief: read message from the bridge
 * Param[out]: data
 * Param[in]: data length
 * Return: 0 if ok
 */
static int read_bridge_msg(unsigned char* data, unsigned int length)
{
  unsigned int i;
  for (i = 0; i < length; i++)
  {
    if ( spi_transfer(0, data+i) )
    {
      return -1;
    }
    usleep (spi_delay_us);
  }
  return 0;
}

/*
 * Brief: write message to the bridge 
 * Param[in]: data
 * Param[in]: data length
 * Return: 0 if ok
 */
static int write_bridge_msg(unsigned char* data, unsigned int length)
{
  unsigned char result[MAX_SPI_MESSAGE_SIZE];
  unsigned int i;
  for (i = 0; i < length; i++)
  {
    if ( spi_transfer(data[i], result+i) )
    {
      return -1;
    }
    usleep (spi_delay_us);
  }
  return 0;
}

/*
 * Brief: decode the bridge message ant extract the register value
 * Param[in]: bridge message
 * Param[in]: message length
 * Param[out]: value of the register
 * Return: 0 if ok
 */
static int decode_bridge_message(unsigned char* msg, unsigned int length,
    uint16_t* value)
{
  unsigned int i = 0;
  int start_idx = -1;
  int data_h_idx, data_l_idx;
  uint16_t data_h, data_l;

  // Find the beginning 
  for (i = 0; i < length; i++)
  {
    if (msg[i] == 's')
    {
      start_idx = i;
      break;
    }
  } 

  if (start_idx == -1)
  {
    *value = 0;
    return -1;
  }

  if (length - (start_idx + 1) < 5)
  {
    *value = 0;
    return -1;
  }

  uint8_t data_length;
  data_length = msg[start_idx + 1];
  if (data_length != 2)
  {
    RDA5807_printf(ERROR, "msg[1] != 2\n");
    *value = 0;
    return -1;    
  }
  // Check msg endding
  if (msg[start_idx + 1 + data_length + 1] != 'e')
  {
    RDA5807_printf(ERROR, "msg[%d] != e\n",1+data_length+1);
    *value = 0;
    return -1;
  }

  data_h_idx = start_idx + 1 + 1;
  data_l_idx = data_h_idx + 1;
  data_h=(msg[data_h_idx])<<8;
  data_l=msg[data_l_idx];
  
  *value=data_h+data_l;
  return 0;
}

/*
 * Brief: Read register
 * Param[in]: address of the register
 * Param[out]: value of the register
 * Return: 0 if ok
 */
int RDA5807_proxy_read_register(unsigned char addr, uint16_t* value)
{
  int rc;
  unsigned char tx_msg[] = {'S',2,'r','?','E'};
  unsigned char rx_msg[MAX_SPI_MESSAGE_SIZE];
  tx_msg[3] = addr;

  RDA5807_printf(DEBUG, "Write msg [0x%02x]\n", tx_msg[3]);
  if ( 0 != (rc = write_bridge_msg(tx_msg, sizeof(tx_msg))) )
  {
    RDA5807_printf(ERROR, "Fail to write msg [0x%02x]\n", tx_msg[3]);
    return rc;
  }

  if ( 0 != (rc = read_bridge_msg(rx_msg, sizeof(rx_msg))) )
  {
    RDA5807_printf(ERROR, "Fail to read from [0x%02x]\n", tx_msg[3]);
    return rc;
  }

  return decode_bridge_message(rx_msg, sizeof(rx_msg), value);
}

/*
 * Brief: Write register
 * Param[in]: address of the register
 * Param[in]: value of the register
 * Return: 0 if ok
 */
int RDA5807_proxy_write_register(unsigned char addr, uint16_t value)
{
  unsigned char tx_msg[] = {'S',4,'w','?','?','?','E'};
  tx_msg[3] = addr;
  tx_msg[4] = (value >> 8);
  tx_msg[5] = (0xFF & value);
  RDA5807_printf(DEBUG, "Write msg [0x%02x][0x%02x][0x%02x]\n",
    tx_msg[3], tx_msg[4], tx_msg[5]);
  return write_bridge_msg(tx_msg, sizeof(tx_msg));
}


/*
 * Brief: initialize the proxy
 * Param[in]: spi bus speed
 * Param[in]: spi delay between two transfer
 * Return: 0 if ok
 */
int RDA5807_proxy_init(unsigned int spi_speed, unsigned int spi_delay)
{

    if ( spi_speed > 0 )
      spi_speed_hz = spi_speed;

    if ( spi_delay > 0 )
      spi_delay_us = spi_delay;

    if ( 0 > (spi_fd = open("/dev/spidev0.0", O_RDWR)) )
    {
      RDA5807_printf(ERROR, "Fail to open '/dev/spidev0.0' '%m' (%d)", errno);
      return errno;
    }

    if ( 0 > ioctl (spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed_hz) )
    {
        RDA5807_printf(ERROR, "ioctl failed '%m' (%d)", errno);
        return errno;
    }

    RDA5807_printf(INFO, "%s\nspi bus speed %u Hz\nspi delay %u us\n",
      __FUNCTION__, spi_speed_hz, spi_delay_us);

    return 0;
}

/*
 * Brief: close the proxy
 * Return: 0 if ok
 */
void RDA5807_proxy_close(void)
{
    close(spi_fd);
}
