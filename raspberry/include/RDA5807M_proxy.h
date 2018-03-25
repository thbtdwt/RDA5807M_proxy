/* RDA5807M_proxy
 * Brief: Api to communication with Arduino RDA5807M bridge.
 *
 * Evolution 18-Feb-2018   Thibaut .Creation
 */
#ifndef RDA5807M_PROXY_H
#define RDA5807M_PROXY_H

#include <stdint.h>
int RDA5807_proxy_init(void);
int RDA5807_proxy_read_register(unsigned char addr, uint16_t* value);
int RDA5807_proxy_write_register(unsigned char addr, uint16_t value);
void RDA5807_proxy_close(void);
#endif //RDA5807M_PROXY_H
