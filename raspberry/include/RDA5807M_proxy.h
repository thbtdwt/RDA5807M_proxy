/* RDA5807M_proxy
 * Brief: Api to communication with Arduino RDA5807M bridge.
 *
 * Evolution 18-Feb-2018   Thibaut .Creation
             25 Mar-2018   Thibaut .Rename all functions
 */
#ifndef RDA5807M_PROXY_H
#define RDA5807M_PROXY_H

#include <stdint.h>

/*
 * Brief: initialize the proxy
 * Param[in]: spi bus speed
 * Param[in]: spi delay between two transfer
 * Return: 0 if ok
 */
int RDA5807_proxy_init(unsigned int spi_speed, unsigned int spi_delay);

/*
 * Brief: Read register
 * Param[in]: address of the register
 * Param[out]: value of the register
 * Return: 0 if ok
 */
int RDA5807_proxy_read_register(unsigned char addr, uint16_t* value);

/*
 * Brief: Write register
 * Param[in]: address of the register
 * Param[in]: value of the register
 * Return: 0 if ok
 */
int RDA5807_proxy_write_register(unsigned char addr, uint16_t value);

/*
 * Brief: close the proxy
 * Return: 0 if ok
 */
void RDA5807_proxy_close(void);

#endif //RDA5807M_PROXY_H
