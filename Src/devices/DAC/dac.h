#ifndef DAC_H
#define DAC_H

/**
    dac info:
    uses i2s3
    i2s3_mclk gpio: PC7
    i2c3_sck gpio: PC10
    i2c3_sd gpio: PC12
    i2s3_ws: PA4

    initialization sequence:
    PD4 in output to high
    Power clt. 1 (0x02): 0x01 
    (also the default value so don't need to change but good to
    do if dac finds itself in a weird state)
    write to address: 0x00 val: 0x99
    write to address: 0x47 val: 0x80
    
    Note: the next 2 steps would require rwm but because they are back to
    back we can do it with 1 read and 2 writes. 
    write to address: 0x32 val: 1b to bit 7
    write to address: 0x32 val: 0b to bit 7
    write to address: 0x00 val: 0x00

    do section 4.6
    read address 0x06
    modify bit 7: M/S to 0
    write the val to address 0x06

    turn on the i2s peripheral
    write to address: 0x02 val: 0x9E

    done

*/

void dac_init()

#endif