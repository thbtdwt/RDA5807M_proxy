/* RDA5807M_proxy
 * Brief: Api to communication with Arduino RDA5807M bridge.
 *
 * Evolution 18-Feb-2018   Thibaut .Creation
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

#define MAX_SPI_MESSAGE_SIZE 10

/*
 * Brief spi fiel descriptor
 */
static int spi_fd;


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
  int i;
  for (i = 0; i < length; i++)
  {
    if ( spi_transfer(0, data+i) )
    {
      return -1;
    }
    usleep (10000);
  }
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
  int i;
  for (i = 0; i < length; i++)
  {
    if ( spi_transfer(data[i], result+i) )
    {
      return -1;
    }
    usleep (10000);
  }
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
  int i = 0;
  int start_idx = -1;
  int data_h_idx, data_l_idx;

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
    fprintf(stderr, "msg[1] != 2\n");
    *value = 0;
    return -1;    
  }
  if (msg[start_idx + 1 + data_length + 1] != 'e')
  {
    fprintf(stderr, "msg[%d] != e\n",1+data_length+1);
    *value = 0;
    return -1;
  }

  data_h_idx = start_idx + 1 + 1;
  data_l_idx = data_h_idx + 1;

  *value=((uint16_t)msg[data_h_idx])<<4 + msg[data_l_idx];
  return 0;
}

/*
 * Brief: initialize the proxy
 * Param[in]: address of the register
 * Param[out]: value of the register
 * Return: 0 if ok
 */
int read_register(unsigned char addr, uint16_t* value)
{
  unsigned char tx_msg[] = {'S',2,'r','?','E'};
  unsigned char rx_msg[MAX_SPI_MESSAGE_SIZE];
  tx_msg[3] = addr;
  write_bridge_msg(tx_msg, sizeof(tx_msg));
  read_bridge_msg(rx_msg, sizeof(rx_msg));

  return decode_bridge_message(rx_msg, sizeof(rx_msg), value);
}


/*
 * Brief: initialize the proxy
 * Return: 0 if ok
 */
int init_RDA5807_proxy(void)
{
    spi_fd = open("/dev/spidev0.0", O_RDWR);

    unsigned int speed = 1000000;
    if ( 0 > ioctl (spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) )
    {
        return errno;
    }
    return 0;
}

/*
 * Brief: close the proxy
 * Return: 0 if ok
 */
void close_RDA5807_proxy(void)
{
    close(spi_fd);
}
