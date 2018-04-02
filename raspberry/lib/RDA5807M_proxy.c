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
#include "RDA5807M_msg_def.h"

/*
 * Brief spi variables
 */
static int spi_fd;
static unsigned int spi_speed_hz = 2000000;
static unsigned int spi_delay_us = 1000;

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
    RDA5807_printf(ERROR, "%s failed '%m' (%d)'\n", __FUNCTION__, errno);
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
  unsigned char status[RDA5807M_BRIDGE_MAX_MSG_SIZE];
  unsigned int i;
  for (i = 0; i < length; i++)
  {
    if ( spi_transfer(data[i], status+i) )
    {
      return -1;
    }
    usleep (spi_delay_us);
  }

  // get the status
  if ( spi_transfer(0, status+i) )
  {
    return -1;
  }

  if ( ! (status[RDA5807M_BRIDGE_MSG_ACK_INDEX_1] == RDA5807M_BRIDGE_MSG_ACK 
    && status[length] == RDA5807M_BRIDGE_MSG_ACK) )
  {
    RDA5807_print_buffer(WARNING, status, sizeof(status),
      "Wrong status not 0x%02x at (%d) and (%d):\n", RDA5807M_BRIDGE_MSG_ACK,
      RDA5807M_BRIDGE_MSG_ACK_INDEX_1,length);
    return -1;
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
  unsigned int start_idx = 0xFFFFFFFF;
  unsigned int end_idx;
  uint8_t payload_size;
  int data_h_idx, data_l_idx;
  uint16_t data_h, data_l;


  // Find the beginning 
  for (i = 0; i < length; i++)
  {
    if (msg[i] == RDA5807M_BRIDGE_MSG_START)
    {
      start_idx = i;
      break;
    }
  } 

  if (start_idx == 0xFFFFFFFF)
  {
    *value = 0;
    return -1;
  }

  // Get the payload size
  if ( start_idx + 1 > (length - 1) )
  {
    RDA5807_printf(ERROR, "Unable to find payload size [%d] > [%d]\n",
      start_idx + 1, length - 1);
    *value = 0;
    return -1;    
  }
  payload_size = msg[start_idx + 1];
  if (payload_size != RDA5807M_BRIDGE_MSG_PAYLOAD_SIZE)
  {
    RDA5807_printf(ERROR, "Wrong payload size %d != %d\n", 
      start_idx + 1, RDA5807M_BRIDGE_MSG_PAYLOAD_SIZE);
    *value = 0;
    return -1;    
  }

  // Check msg endding
  end_idx = start_idx + 1 + payload_size + 1;
  if ( end_idx > (length - 1) )
  {
    RDA5807_printf(ERROR, "Unable to find end [%d] > [%d]\n",
      end_idx, length - 1);
    *value = 0;
    return -1;    
  }
  if (msg[end_idx] != RDA5807M_BRIDGE_MSG_STOP)
  {
    RDA5807_printf(ERROR, "End of msg not found msg[%d] = 0x%02x != 0x%02x\n",
      end_idx, msg[end_idx], RDA5807M_BRIDGE_MSG_STOP);
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
  unsigned char tx_msg[] = {RDA5807M_PROXY_MSG_START,
    2,RDA5807M_PROXY_MSG_READ, addr, RDA5807M_PROXY_MSG_STOP};
  unsigned char rx_msg[RDA5807M_BRIDGE_MAX_MSG_SIZE];

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
  unsigned char tx_msg[] = {RDA5807M_PROXY_MSG_START,
    4, RDA5807M_PROXY_MSG_WRITE,
    addr,
    (value >> 8),(0xFF & value),
    RDA5807M_PROXY_MSG_STOP};

  RDA5807_print_buffer(DEBUG, tx_msg, sizeof(tx_msg), "Write msg:\n");
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
