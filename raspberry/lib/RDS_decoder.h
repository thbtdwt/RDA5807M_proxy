/* RDS_decoder
 * Brief: API to decode RDS block.
 *
 * Evolution 26-May-2018   Thibaut .Creation
 *
 */

#ifndef RDS_DECODER_H
#define RDS_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <RDA5807M_radio.h>
#include <stdint.h>

int RDS_decode(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4);

const RDS_info_t* RDS_get_info(void);


#ifdef __cplusplus
}
#endif

#endif // RDS_DECODER_H