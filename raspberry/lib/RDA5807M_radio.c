/* RDA5807M
 * Brief: Api to set the RDA5807M radio.
 *
 * Evolution 18-Mai-2018   Thibaut .Creation
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <RDA5807M_radio.h>
#include <RDA5807M_proxy.h>
#include "RDA5807M_REG.h"


/*
 * Brief: Turn on the radio
 * Return: 0 if ok
 */
int RDA5807M_turn_on(void) {
    int ret = 1;
    RDA5807M_REG_CTRL_t regCtrl = {.value = 0};
    RDA5807M_REG_R4_t reg4 = {.value=0};
    RDA5807M_REG_VOLUME_t regVolume = {.value=0};

    regCtrl.bits.ENABLE = 1;
    regCtrl.bits.RESET = 1;
    ret = RDA5807_proxy_write_register(RDA5807M_REG_CTRL, regCtrl.value);

    ret = RDA5807M_set_band(US_WEST_UE);

  
    reg4.bits.DE = 1;
    ret = RDA5807_proxy_write_register(RDA5807M_REG_R4, reg4.value);

    regVolume.bits.VOLUME = 1;
    regVolume.bits.SEEKTH = 1;
    regVolume.bits.INT_MODE = 1;
    ret = RDA5807_proxy_write_register(RDA5807M_REG_VOL, regVolume.value);

    regCtrl.bits.RESET = 0;
    regCtrl.bits.OUTPUT = 1;
    regCtrl.bits.UNMUTE = 1;
    ret = RDA5807_proxy_write_register(RDA5807M_REG_CTRL, regCtrl.value);

    return ret;
}

/*
 * Brief: Turn off the radio
 * Return: 0 if ok
 */
int RDA5807M_turn_off(void) {
    RDA5807M_REG_CTRL_t reg = {.value = 0};
    reg.bits.RESET = 1;
    return RDA5807_proxy_write_register(RDA5807M_REG_CTRL, reg.value);
}

/*
 * Brief: Set the volume
 * Param[in]: volume 0..15
 * Return: 0 if ok
 */
int RDA5807M_set_volume(uint8_t volume) {
    int ret = 1;
    RDA5807M_REG_VOLUME_t reg = {.value=0};

    if ( volume > 0xF )
        return 1;

    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_VOL, &reg.value)) ) {
        reg.bits.VOLUME = volume;
        ret = RDA5807_proxy_write_register(RDA5807M_REG_VOL, reg.value);
    }
    return ret;
}

/*
 * Brief: set the bass boost
 * Param[in]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_set_bass_boost(RDA5807M_state_t state) {
    int ret = 1;
    RDA5807M_REG_CTRL_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CTRL, &reg.value)) ) {  
        reg.bits.BASS = ( state == OFF )? 0 : 1;
        ret = RDA5807_proxy_write_register(RDA5807M_REG_CTRL, reg.value);
    }
    return ret;
}

/*
 * Brief: set mono
 * Param[in]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_set_mono(RDA5807M_state_t state) {
    int ret = 1;
    RDA5807M_REG_CTRL_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CTRL, &reg.value)) ) {
        reg.bits.MONO = ( state == OFF )? 0 : 1;
        ret = RDA5807_proxy_write_register(RDA5807M_REG_CTRL, reg.value);
    }
    return ret;
}

/*
 * Brief: set mute
 * Param[in]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_set_mute(RDA5807M_state_t state) {
    int ret = 1;
    RDA5807M_REG_CTRL_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CTRL, &reg.value)) ) {  
        reg.bits.UNMUTE = ( state == OFF )? 1 : 0;
        ret = RDA5807_proxy_write_register(RDA5807M_REG_CTRL, reg.value);
    }
    return ret;
}

/*
 * Brief: set the band
 * Param[in]: Band 
 *              US_WEST_UE
 *              JAPAN
 *              WORLD
 *              EST_EU
 * Return: 0 if ok
 */
int RDA5807M_set_band(RDA5807M_band_t band) {
    int ret = 1;
    RDA5807M_REG_CHAN_t reg;
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CHAN, &reg.value)) ) {        
        reg.bits.BAND = band;
        ret = RDA5807_proxy_write_register(RDA5807M_REG_CHAN, reg.value);
    }
    return ret;
}

/*
 * Brief: set the frequency
 * Param[in]: frequency
 * Return: 0 if ok
 */
int RDA5807M_set_frequency(float freq) {
    int ret = 1;
    RDA5807M_REG_CHAN_t reg;
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CHAN, &reg.value)) ) {
        float offset;
        switch (reg.bits.BAND) {
            case 0: offset = 87.0; break;
            case 1: 
            case 2: offset = 76.0; break;
            case 3: offset = 65.0; break;
        }

        int spacing_khz;
        switch (reg.bits.SPACE) {
            case RDA5807M_REG_CHAN_SPACE_100_KHZ: spacing_khz = 100; break;
            case RDA5807M_REG_CHAN_SPACE_200_KHZ: spacing_khz = 200; break;
            case RDA5807M_REG_CHAN_SPACE_50_KHZ: spacing_khz = 50; break;
            case RDA5807M_REG_CHAN_SPACE_25_KHZ: spacing_khz = 25; break;
        }

        uint16_t newChan = ((freq - offset)*1000) / spacing_khz;

        //printf("before 0x%04x\n",reg.value);

        reg.bits.CHAN = newChan;
        reg.bits.TUNE = 1;

        //printf("after 0x%04x\n",reg.value);

        ret = RDA5807_proxy_write_register(RDA5807M_REG_CHAN, reg.value);
    }
    return ret;
}

/*
 * Brief: seek until a frequency is found
 * Param[in]: SEEK_UP or SEEK_DOWN
 * Return: 0 if ok
 */
int RDA5807M_seek(RDA5807M_seek_direction_t direction) {
    int ret = 1;
    int loop_counter = 10;
    RDA5807M_REG_CTRL_t regCtrl;

    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CTRL, &regCtrl.value)) ) {
        regCtrl.bits.SEEKUP = (direction == SEEK_UP) ? 1 : 0;
        regCtrl.bits.SEEK = 1;
        if ( 0 == (ret = RDA5807_proxy_write_register(RDA5807M_REG_CTRL, regCtrl.value)) ) {
            while (loop_counter) {
                RDA5807M_REG_RA_t regA;
                ret = RDA5807_proxy_read_register(RDA5807M_REG_RA, &regA.value);
                if ( 0 == ret && ( regA.bits.STC_COMPLETE || regA.bits.SF_FAIL ) )
                    break;
                loop_counter--;
                sleep(1);
            }
        }
    }
    return ret;
}

/*
 * Brief: get the chip id
 * Param[out]: chipid
 * Return: 0 if ok
 */
int RDA5807M_get_chipid(uint8_t* chipid) {
    int ret;
    RDA5807M_REG_CHIPID_t reg;
    ret = RDA5807_proxy_read_register(RDA5807M_REG_CHIPID, &reg.value);
    *chipid = (ret == 0 ) ? reg.bits.CHIPID : 0;
    return ret;
}

/*
 * Brief: get the volume
 * Param[out]: volume
 * Return: 0 if ok
 */
int RDA5807M_get_volume(uint8_t* volume) {
    int ret;
    RDA5807M_REG_VOLUME_t reg;
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_VOL, &reg.value)) ) {
        *volume = reg.bits.VOLUME;
    }
    return ret;
}

/*
 * Brief: get bass boost state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_bass_boost(RDA5807M_state_t* state) {
    int ret = 1;
    RDA5807M_REG_CTRL_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CTRL, &reg.value)) ) {  
        *state = ( reg.bits.BASS )? ON : OFF;
    }
    return ret;
}

/*
 * Brief: get mono state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_mono(RDA5807M_state_t* state) {
    int ret = 1;
    RDA5807M_REG_CTRL_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CTRL, &reg.value)) ) {  
        *state = ( reg.bits.MONO )? ON : OFF;
    }
    return ret;
}

/*
 * Brief: get the mute state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_mute(RDA5807M_state_t* state) {
    int ret = 1;
    RDA5807M_REG_CTRL_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CTRL, &reg.value)) ) {  
        *state = ( reg.bits.UNMUTE )? OFF : ON;
    }
    return ret;
}

/*
 * Brief: get the frequency
 * Param[out]: frequency
 * Return: 0 if ok
 */
int RDA5807M_get_frequency(float* freq) {
    int ret = 1;
    RDA5807M_REG_CHAN_t reg;
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_CHAN, &reg.value)) ) {
        float offset;
        switch (reg.bits.BAND) {
            case 0: offset = 87.0; break;
            case 1: 
            case 2: offset = 76.0; break;
            case 3: offset = 65.0; break;
        }

        int spacing_khz;
        switch (reg.bits.SPACE) {
            case RDA5807M_REG_CHAN_SPACE_100_KHZ: spacing_khz = 100; break;
            case RDA5807M_REG_CHAN_SPACE_200_KHZ: spacing_khz = 200; break;
            case RDA5807M_REG_CHAN_SPACE_50_KHZ: spacing_khz = 50; break;
            case RDA5807M_REG_CHAN_SPACE_25_KHZ: spacing_khz = 25; break;
        }

        RDA5807M_REG_RA_t regA;
        if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_RA, &regA.value)) ) {
            uint16_t current_chan = regA.bits.READCHAN;
            *freq = ( ((1.0)*current_chan) * spacing_khz)/1000 + offset;
        }
    }
    return ret;
}

/*
 * Brief: get the fm true state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_fm_true(RDA5807M_state_t* state) {
    int ret = 1;
    RDA5807M_REG_RB_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_RB, &reg.value)) ) {  
        *state = ( reg.bits.FM_TRUE )? ON : OFF;
    }
    return ret;
}

/*
 * Brief: get the fm ready state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_fm_ready(RDA5807M_state_t* state) {
    int ret = 1;
    RDA5807M_REG_RB_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_RB, &reg.value)) ) {  
        *state = ( reg.bits.FM_READY )? ON : OFF;
    }
    return ret;
}

/*
 * Brief: get the stero state
 * Param[out]: ON or OFF
 * Return: 0 if ok
 */
int RDA5807M_get_stereo(RDA5807M_state_t* state) {
    int ret = 1;
    RDA5807M_REG_RA_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_RA, &reg.value)) ) {  
        *state = ( reg.bits.STEREO )? ON : OFF;
    }
    return ret;
}

/*
 * Brief: get the rssi
 * Param[out]: rssi (log)
 * Return: 0 if ok
 */
int RDA5807M_get_rssi(uint8_t* rssi) {
    int ret = 1;
    RDA5807M_REG_RB_t reg = {.value = 0};
    if ( 0 == (ret = RDA5807_proxy_read_register(RDA5807M_REG_RB, &reg.value)) ) {  
        *rssi = reg.bits.RSSI;
    }
    return ret;
}