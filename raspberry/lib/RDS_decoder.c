/* RDS_decoder
 * Brief: API to decode RDS block.
 *
 * Evolution 26-May-2018   Thibaut .Creation
 *
 */

#include <RDA5807M_radio.h>
#include "RDS_decoder.h"

#include <stdio.h>
#include <string.h>

/*
 * Brief: Structure of block 1
 */
typedef union {
	uint16_t value;
	uint16_t PI_CODE;
} RDS_BLOCK_1_t;

/*
 * Brief: Structure of block 2
 */
typedef union {
	uint16_t value;
	struct {
		unsigned address:2;
		unsigned DI:1;
		unsigned MS:1;
		unsigned TA:1;
		unsigned TPY:5;
		unsigned TP:1;
        unsigned VERSION:1;
		unsigned TYPE:4;
	} bits;
} RDS_BLOCK_2_t;

/*
 * Brief: Structure of block 3 and 4
 */
typedef uint16_t RDS_BLOCK_34_t;

/*
 * Brief: Current RDS block value
 */
static struct {
    RDS_BLOCK_1_t   b1;
    RDS_BLOCK_2_t   b2;
    RDS_BLOCK_34_t  b3;
    RDS_BLOCK_34_t  b4;
} current_rds_blocks = {0};

/*
 * Brief: Global RDS info
 */
static RDS_info_t RDS_info = {0};

/*
 * Brief: Decode RDS block
 * Param[in]: rds block1
 * Param[in]: rds block2
 * Param[in]: rds block3
 * Param[in]: rds block4
 * Return: 0 if ok
 */
int RDS_decode(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
    int ret = 1;
    int new_values = 0;
    static char station_name_tmp[9];
    static char station_name_tmp2[9];

    if ( block1 == 0 ) {
        memset (&current_rds_blocks, 0, sizeof(current_rds_blocks));
    } else {
        if ( RDS_info.PI == block1 ) {
            RDS_info.valid = 1;    
        } else {
            RDS_info.PI = block1;
            RDS_info.valid = 0;
        }
    }

    if ( current_rds_blocks.b1.value != block1) { current_rds_blocks.b1.value = block1; new_values = 1; }
    if ( current_rds_blocks.b2.value != block2) { current_rds_blocks.b2.value = block2; new_values = 1; }
    if ( current_rds_blocks.b3 != block3) { current_rds_blocks.b3 = block3; new_values = 1; }
    if ( current_rds_blocks.b4 != block4) { current_rds_blocks.b4 = block4; new_values = 1; }

    if ( new_values ) {

        if ( current_rds_blocks.b2.bits.TYPE == 0) {
            /* 0A or 0B */
            int index = 2 * current_rds_blocks.b2.bits.address;

            char c1 = current_rds_blocks.b4 >> 8;
            char c2 = current_rds_blocks.b4 & 0x00FF;

            if ( (station_name_tmp[index] == c1)
                && (station_name_tmp[index + 1] == c2) ) {
                station_name_tmp2[index] = c1;
                station_name_tmp2[index + 1] = c2;
                station_name_tmp2[8] = '\0';

                if ( (index == 6)
                    && ( strcmp(station_name_tmp2, RDS_info.station_name) != 0) ) {
                    snprintf(RDS_info.station_name, sizeof(RDS_info.station_name),
                        "%s", station_name_tmp2);
                }
            }

            if ( (station_name_tmp[index] != c1)
                && (station_name_tmp[index + 1] != c2) ) {
                station_name_tmp[index] = c1;
                station_name_tmp[index + 1] = c2;
                station_name_tmp[8] = '\0';
            }

            if ( current_rds_blocks.b2.bits.VERSION == 0 ) {
                /* Version A */
                /*
                printf("AF 0x%d\n",  current_rds_blocks.b3 >> 8);
                printf("AF 0x%d\n",  current_rds_blocks.b3 & 0x00FF);
                */
            } else {
                /* Version B */
            }
        }

        return ret;

        if ( current_rds_blocks.b2.bits.TYPE == 2) {
            /* 2A or 2B */
            int textAB = (block2 & 0x0010);
            static int last_textAB = 0;
            int index = 4 * current_rds_blocks.b2.value & 0x0F ;
            static int last_index = 0;

            if ( index < last_index ) {
                ;
                //TODO printf("message %s\n", RDS_info.station_text);
            }
            last_index = index;

            if ( last_textAB != textAB ) {
                last_textAB = textAB;
                memset(RDS_info.station_text, 0, sizeof(RDS_info.station_text));
            }

            if ( current_rds_blocks.b2.bits.VERSION == 0 ) {
                /* Version A */
                RDS_info.station_text[index]     = current_rds_blocks.b3 >> 8;
                RDS_info.station_text[index + 1] = current_rds_blocks.b3 & 0x00FF;
                RDS_info.station_text[index + 2] = current_rds_blocks.b4 >> 8;
                RDS_info.station_text[index + 3] = current_rds_blocks.b4 & 0x00FF;
                RDS_info.station_text[64] = '\0';
            } else {
                /* Version B */
                RDS_info.station_text[index] = current_rds_blocks.b4 >> 8;
                RDS_info.station_text[index + 1] = current_rds_blocks.b4 & 0x00FF;
                RDS_info.station_text[32] = '\0';               
            }  
        }
    }

    return ret;
}

/*
 * Brief: Get RDS get info
 * Return: const pointer to RDS info
 */
const RDS_info_t* RDS_get_info(void) {
    return &RDS_info;
}