/* RDA5807M_RADIO
 * Brief: Api to set the RDA5807M radio.
 *
 * Evolution 18-Mai-2018   Thibaut .Creation
 *
 */
#ifndef RDA5807M_RADIO_H
#define RDA5807M_RADIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Brief: Seek direction
 */
typedef enum {
    SEEK_UP,
    SEEK_DOWN
} RDA5807M_seek_direction_t;

/*
 * Brief: Band 
 */
typedef enum {
    US_WEST_UE,
    JAPAN,
    WORLD,
    EST_EU
} RDA5807M_band_t;

/*
 * Brief: State
 */
typedef enum {
    ON,
    OFF
} RDA5807M_state_t;

/*
 * Brief: Turn on the radio
 * Return: 0 if ok
 */
int RDA5807M_turn_on(void);

/*
 * Brief: Turn off the radio
 * Return: 0 if ok
 */
int RDA5807M_turn_off(void);

/*
 * Brief: Set the volume
 * Param[in]: volume 0..15
 * Return: 0 if ok
 */
int RDA5807M_set_volume(uint8_t volume);

/*
 * Brief: set the bass boost
 * Param[in]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_set_bass_boost(RDA5807M_state_t state);

/*
 * Brief: set mono
 * Param[in]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_set_mono(RDA5807M_state_t state);

/*
 * Brief: set mute
 * Param[in]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_set_mute(RDA5807M_state_t state);

/*
 * Brief: set the band
 * Param[in]: Band 
 *              US_WEST_UE
 *              JAPAN
 *              WORLD
 *              EST_EU
 * Return: 0 if ok
 */
int RDA5807M_set_band(RDA5807M_band_t band);

/*
 * Brief: set the frequency
 * Param[in]: frequency
 * Return: 0 if ok
 */
int RDA5807M_set_frequency(float freq);

/*
 * Brief: seek until a frequency is found
 * Param[in]: SEEK_UP or SEEK_DOWN
 * Return: 0 if ok
 */
int RDA5807M_seek(RDA5807M_seek_direction_t direction);

/*
 * Brief: get the chip id
 * Param[out]: chipid
 * Return: 0 if ok
 */
int RDA5807M_get_chipid(uint8_t* chipid);

/*
 * Brief: get the volume
 * Param[out]: volume
 * Return: 0 if ok
 */
int RDA5807M_get_volume(uint8_t* volume);

/*
 * Brief: get bass boost state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_bass_boost(RDA5807M_state_t* state);

/*
 * Brief: get mono state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_mono(RDA5807M_state_t* state);

/*
 * Brief: get the mute state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_mute(RDA5807M_state_t* state);

/*
 * Brief: get the frequency
 * Param[out]: frequency
 * Return: 0 if ok
 */
int RDA5807M_get_frequency(float* freq);

/*
 * Brief: get the fm true state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_fm_true(RDA5807M_state_t* state);

/*
 * Brief: get the fm ready state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_fm_ready(RDA5807M_state_t* state);

/*
 * Brief: get the stero state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_stereo(RDA5807M_state_t* stereo);

/*
 * Brief: get the rssi
 * Param[out]: rssi (log)
 * Return: 0 if ok
 */
int RDA5807M_get_rssi(uint8_t* rssi);

#ifdef __cplusplus
}
#endif

#endif // RDA5807M_RADIO_H