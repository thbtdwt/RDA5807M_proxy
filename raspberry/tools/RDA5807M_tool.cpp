/* RDA5807M_tool
 * Brief: Control the RDA5807M.
 *
 * Evolution 18-Mai-2018   Thibaut .Creation
 *
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <libgen.h>
#include <RDA5807M.h>

static const char* progname;

static void usage() {
    std::cerr << "Usage: " << progname << " <command> [argument]" << std::endl;
    std::string text("Command:\n" 
        "\tturnOn\n"
        "\tturnOff\n"
        "\tsetVolume <volume 0..15>\n"
        "\tsetBassBoost <on|off>\n"
        "\tsetMono <on|off>\n"
        "\tsetMute <on|off>\n"
        "\tsetBand <US_WEST_UE|JAPAN|WORLD|EST_UE>\n"
        "\tsetFrequency <freq>\n"
        "\tseekUp\n"
        "\tseekDown\n"
        "\tgetChipId\n"
        "\tgetVolume\n"
        "\tgetBassBoost\n"
        "\tgetMono\n"
        "\tgetFrequency\n"
        "\tgetFmTrue\n"
        "\tgetFmReady\n"
        "\tgetStereo\n"
        "\tgetRSSI\n"
        );
    std::cerr << text << std::endl;
}

static inline void fatal_error(const char* description) {
    std::cerr << description << std::endl; \
    RDA5807_proxy_close();
    exit(1);
} 

#define checkArg \
do { \
    if ( argc < 3 ) \
        fatal_error("Need argument"); \
} while(0)


int main (int argc, char* argv[]) {
    int ret = 1;
    progname = basename(argv[0]);

    // Need at leat one argument;
    if ( argc < 2 ) {
        std::cerr << "Need argument" << std::endl;
        usage();
        exit(1);
    }

    if ( RDA5807_proxy_init(0,5000) )
    {
        std::cerr << "RDA5807_proxy failed" << std::endl;
        exit(1); 
    }

    std::string command = argv[1];

    if ( command == "turnOn" ) {
        ret = RDA5807M_turn_on();

    } else if ( command == "turnOff" ) {
        ret = RDA5807M_turn_off();

    } else if ( command == "setVolume" ) {
        checkArg;
        int volume = std::strtol(argv[2], NULL, 0);
        ret = RDA5807M_set_volume(volume);

    } else if ( command == "setBassBoost" ) {
        checkArg;
        std::string arg = argv[2];
        if ( arg == "on" )
            ret = RDA5807M_set_bass_boost(ON);
        else if ( arg == "off" )
            ret = RDA5807M_set_bass_boost(OFF);
        else
            fatal_error("Wrong argument");

    } else if ( command == "setMono" ) {
        checkArg;
        std::string arg = argv[2];
        if ( arg == "on" )
            ret = RDA5807M_set_mono(ON);
        else if ( arg == "off" )
            ret = RDA5807M_set_mono(OFF);
        else
            fatal_error("Wrong argument");

    } else if ( command == "setMute" ) {
        checkArg;
        std::string arg = argv[2];
        if ( arg == "on" )
            ret = RDA5807M_set_mute(ON);
        else if ( arg == "off" )
            ret = RDA5807M_set_mute(OFF);
        else
            fatal_error("Wrong argument");

    } else if ( command == "setBand" ) {
        checkArg;
        std::string arg = argv[2];
        if ( arg == "US_WEST_UE" )
            ret = RDA5807M_set_band(US_WEST_UE);
        else if ( arg == "JAPAN" )
            ret = RDA5807M_set_band(JAPAN);
        else if ( arg == "WORLD" )
            ret = RDA5807M_set_band(WORLD);
        else if ( arg == "EST_EU" )
            ret = RDA5807M_set_band(EST_EU);
        else
            fatal_error("Wrong argument");

    } else if ( command == "setFrequency" ) {
        checkArg;
        float frequency = std::stof(argv[2]);
        ret = RDA5807M_set_frequency(frequency);

    } else if ( command == "seekUp" ) {
        ret = RDA5807M_seek(SEEK_UP);

    } else if ( command == "seekDown" ) { 
        ret = RDA5807M_seek(SEEK_DOWN);  

    } else if ( command == "getChipId" ) {
        uint8_t chipId;
        if ( 0 == (ret = RDA5807M_get_chipid(&chipId)) )
            std::cout << "0x" << std::hex << (int)chipId << std::dec << std::endl;

    } else if ( command == "getVolume" ) {
        uint8_t volume;
        if ( 0 == (ret = RDA5807M_get_volume(&volume)) )
            std::cout << (int)volume << std::endl;

    } else if ( command == "getBassBoost" ) {
        RDA5807M_state_t state;
        std::string state_s("unknow");
        if ( 0 == (ret = RDA5807M_get_bass_boost(&state)) )
            state_s = ( state == ON ) ? "true" : "false";
        std::cout << state_s << std::endl;

    } else if ( command == "getMono" ) {
        RDA5807M_state_t state;
        std::string state_s("unknow");
        if ( 0 == (ret = RDA5807M_get_mono(&state)) )
            state_s = ( state == ON ) ? "true" : "false";
        std::cout << state_s << std::endl;

    } else if ( command == "getMute" ) {
        RDA5807M_state_t state;
        std::string state_s("unknow");
        if ( 0 == (ret = RDA5807M_get_mute(&state)) )
            state_s = ( state == ON ) ? "true" : "false";
        std::cout << state_s << std::endl;

    } else if ( command == "getFrequency" ) {
        float freq;
        if ( 0 == (ret = RDA5807M_get_frequency(&freq)) )
            std::cout << freq << std::endl;
    
    } else if ( command == "getFmTrue" ) {
        RDA5807M_state_t state;
        std::string state_s("unknow");
        if ( 0 == (ret = RDA5807M_get_fm_true(&state)) )
            state_s = ( state == ON ) ? "true" : "false";
        std::cout << state_s << std::endl;

    } else if ( command == "getFmReady" ) {
        RDA5807M_state_t state;
        std::string state_s("unknow");
        if ( 0 == (ret = RDA5807M_get_fm_ready(&state)) )
            state_s = ( state == ON ) ? "true" : "false";
        std::cout << state_s << std::endl;

    } else if ( command == "getStereo" ) {
        RDA5807M_state_t state;
        std::string state_s("unknow");
        if ( 0 == (ret = RDA5807M_get_stereo(&state)) )
            state_s = ( state == ON ) ? "true" : "false";
        std::cout << state_s << std::endl;

    } else if ( command == "getRSSI" ) {
        uint8_t rssi;
        if ( 0 == (ret = RDA5807M_get_rssi(&rssi)) )
            std::cout << (int)rssi << std::endl;

    } else if ( command == "checkRDS" ) {         
; // TODO
    } else {
        std::cerr << "Unknown command\n" << std::endl;
        usage();
        ret = 1;
    }

    RDA5807_proxy_close();
    return ret;
}
