/* RDA5807M_proxy
 * Brief: Api to communication with Arduino RDA5807M bridge.
 *
 * Evolution 18-Feb-2018   Thibaut .Creation
 */
#ifndef RDA5807M_PROXY_H
#define RDA5807M_PROXY_H

#include <stdint.h>
int init_RDA5807_proxy(void);
int read_register(unsigned char addr, uint16_t* value);
void close_RDA5807_proxy(void);
#endif //RDA5807M_PROXY_H
