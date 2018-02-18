/* RDA5807M_ALL_REG
 * Brief: Display all register of RDA5807M.
 *
 * Evolution 18-Feb-2018   Thibaut .Creation
 */
#include <stdio.h>
#include <stdlib.h>
#include "RDA5807M_proxy.h"

int main (int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    int i;
    uint16_t value;
    uint8_t list_of_register[] = { 0x0, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 
                                 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

    if ( init_RDA5807_proxy() )
    {
        fprintf(stderr,"RDA5807_proxy failed");
        exit(1); 
    }

    for (i = 0; i < sizeof(list_of_register); i++)
    {
        read_register(list_of_register[i], &value);
        printf("Reg 0x%02x = 0x%04x \n", list_of_register[i], value);
    }

    close_RDA5807_proxy();

    return 0;
}
