/* RDA5807M_REG
 * Brief: RDA5807M register description.
 *
 * Evolution 18-May-2018   Thibaut .Creation
 *
 */
#ifndef RDA5807M_REG_H
#define RDA5807M_REG_H

#include <stdint.h>

/* Main registers addresses */
#define RDA5807M_REG_CHIPID  0x00
#define RDA5807M_REG_CTRL    0x02
#define RDA5807M_REG_CHAN    0x03
#define RDA5807M_REG_R4      0x04
#define RDA5807M_REG_VOL     0x05

#define RDA5807M_REG_RA      0x0A
#define RDA5807M_REG_RB      0x0B

#define RDA5807M_REG_RDSA    0x0C
#define RDA5807M_REG_RDSB    0x0D
#define RDA5807M_REG_RDSC    0x0E
#define RDA5807M_REG_RDSD    0x0F


/* CHIPID Register */
typedef union {
	uint16_t value;
	struct {
		uint8_t unused:8;
		uint8_t CHIPID:8;
	} bits;
} RDA5807M_REG_CHIPID_t;


/* CTRL Register */
typedef union {
	uint16_t value;
	struct {
		unsigned ENABLE:1;
		unsigned RESET:1;
		unsigned NEW:1;
		unsigned RDS:1;
		unsigned CLK_MODE:3;
		unsigned SKMODE:1;
		unsigned SEEK:1;
		unsigned SEEKUP:1;
		unsigned RCLK_INPUT_MODE:1;
		unsigned RCLK_NON_CALIBRATE_MODE:1;
		unsigned BASS:1;
		unsigned MONO:1;
		unsigned UNMUTE:1;
		unsigned OUTPUT:1;
	} bits;
} RDA5807M_REG_CTRL_t;

#define RDA5807M_REG_CTRL_CLK_MODE_32_KHZ  0
#define RDA5807M_REG_CTRL_CLK_MODE_12_MHZ  1
#define RDA5807M_REG_CTRL_CLK_MODE_24_KHZ  5
#define RDA5807M_REG_CTRL_CLK_MODE_13_KHZ  2
#define RDA5807M_REG_CTRL_CLK_MODE_26_KHZ  6
#define RDA5807M_REG_CTRL_CLK_MODE_19_KHZ  3
#define RDA5807M_REG_CTRL_CLK_MODE_38_KHZ  7


/* CHAN Register */
typedef union {
	uint16_t value;
	struct {
		unsigned SPACE:2;
		unsigned BAND:2;
		unsigned TUNE:1;
		unsigned DIRECT_MODE:1;
		unsigned CHAN:10;
	} bits;
} RDA5807M_REG_CHAN_t;

#define RDA5807M_REG_CHAN_SPACE_25_KHZ  3
#define RDA5807M_REG_CHAN_SPACE_50_KHZ  2
#define RDA5807M_REG_CHAN_SPACE_100_KHZ 0
#define RDA5807M_REG_CHAN_SPACE_200_KHZ 1

#define RDA5807M_REG_CHAN_BAND_EU    0
#define RDA5807M_REG_CHAN_BAND_JAPAN 1
#define RDA5807M_REG_CHAN_BAND_WORLD 2
#define RDA5807M_REG_CHAN_BAND_EE    3


/* R4 Register */
typedef union {
	uint16_t value;
	struct {
		unsigned unused:8;
		unsigned AFCD:1;
		unsigned SOFTMUTE:1;
		unsigned unused1:1;
		unsigned DE:1;
		unsigned unused2:4;
	} bits;
} RDA5807M_REG_R4_t;

#define RDA5807M_REG_R4_DE_50_US  1
#define RDA5807M_REG_R4_DE_75_US  0


/* Volume Register */
typedef union {
	uint16_t value;
	struct {
		unsigned VOLUME:4;
		unsigned unused1:4;
		unsigned SEEKTH:4;
		unsigned unused2:3;
		unsigned INT_MODE:1;
	} bits;
} RDA5807M_REG_VOLUME_t;



/* RA Register */
typedef union {
	uint16_t value;
	struct {
		unsigned READCHAN:10;
		unsigned STEREO:1;
		unsigned BLK_E:1;
		unsigned RDSS_SYNC:1;
		unsigned SF_FAIL:1;
		unsigned STC_COMPLETE:1;
		unsigned RDS_READY:1;
	} bits;
} RDA5807M_REG_RA_t;


/* RB Register */
typedef union {
	uint16_t value;
	struct {
		unsigned BLERB:2;
		unsigned BLERA:2;
		unsigned ABCD_E:1;
		unsigned unused:2;
		unsigned FM_READY:1;
		unsigned FM_TRUE:1;
		unsigned RSSI:7;
	} bits;
} RDA5807M_REG_RB_t;


/* RDSx Register (ie RDSA, RDSB, RDSC, RDSD) */
typedef uint16_t RDA5807M_REG_RDSx_t;

#endif // RDA5807M_REG_H