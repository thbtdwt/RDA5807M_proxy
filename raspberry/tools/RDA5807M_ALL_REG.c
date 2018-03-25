/* RDA5807M_ALL_REG
 * Brief: Display all register of RDA5807M.
 *
 * Evolution 18-Feb-2018   Thibaut .Creation
             25 Mar-2018   Thibaut .Add write function
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 #include <libgen.h>
#include "RDA5807M_proxy.h"


static char* progname;

static void usage(void)
{
    fprintf(stderr, "Usage: %s <r> <address|'all'>\n"
        "       %s <w> <address> <value>\n", progname, progname);
}

static void read_all_register(void)
{
    int i;
    uint16_t value;
    uint8_t list_of_register[] = { 0x0, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 
                                 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

    for (i = 0; i < sizeof(list_of_register); i++)
    {
        RDA5807_proxy_read_register(list_of_register[i], &value);
        printf("Reg 0x%02x = 0x%04x \n", list_of_register[i], value);
    }
}

int main (int argc, char* argv[])
{
    uint16_t reg_address;
    uint16_t reg_value;

    progname=basename(argv[0]);

    if (( argc < 2 ) 
        || (( *argv[1] == 'r' ) && ( argc < 3 ))
        || (( *argv[1] == 'w' ) && ( argc < 4 )) )
    {
        fprintf(stderr,"Need args\n");
        usage();
        exit(1);        
    }

    if (( *argv[1] != 'r' ) && ( *argv[1] != 'w' ))
    {
        fprintf(stderr,"Wrong command\n");
        usage();
        exit(1);  
    } 

    if ( 0 == strncmp(argv[2],"all", 3) )
    {
        reg_address=0xff;
    } else {
        reg_address = (uint16_t) ( 0xffff & strtoul(argv[2], NULL, 0) );
    }

    if ( RDA5807_proxy_init() )
    {
        fprintf(stderr,"RDA5807_proxy failed");
        exit(1); 
    }

    if ( *argv[1] == 'r' ) 
    {
        if ( reg_address == 0xff )
        {
            read_all_register();
        } else {
            RDA5807_proxy_read_register(reg_address, &reg_value);
            printf("Reg 0x%02x = 0x%04x \n", reg_address, reg_value);
        }
    } else {
        reg_value = (uint16_t) ( 0xffff & strtoul(argv[3], NULL, 0) );
        RDA5807_proxy_write_register(reg_address, reg_value);
        printf("Reg 0x%02x = 0x%04x \n", reg_address, reg_value); 
    }

    RDA5807_proxy_close();

    return 0;
}
