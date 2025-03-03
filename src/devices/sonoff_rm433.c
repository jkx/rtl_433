/* 
 */
#include "decoder.h"

#define GDB()  asm("int $0x3")


static int rm433_callback(r_device *decoder, bitbuffer_t *bitbuffer)
{
    data_t *data;

    int addr; 
    int key;
    int tmp;

    for (int i=0;i<bitbuffer->num_rows;i++) {

        if (bitbuffer->bits_per_row[i]!=25){
          continue;
        }
        
        // check last byte
        if (bitbuffer->bb[i][3]!=0x80) {
            //printf("===0x%x \n",bitbuffer->bb[i][3]);
            continue;
        }

        // addresse = 2 first bytes encoded to int
        addr = (bitbuffer->bb[i][0] << 8) | bitbuffer->bb[i][1];

        // btn is the lower part of the byte, so mask it w/ a 0xf
        // usually this is byte start w/ a 0 or a F
        tmp = bitbuffer->bb[i][2] & 0x0f;
        switch (tmp) {
            case 7:  key=1; break;
            case 3:  key=2; break;
            case 11: key=3; break;
            case 6:  key=4; break;
            case 13: key=5; break;
            case 10: key=6; break;
            case 14: key=7; break;
            case 12: key=8; break;
            default:
                key = 0;
        }
        if (key==0)
            continue;

        data = data_make(
                "brand",    "",                 DATA_STRING, "SONOFF",
                "model",    "",                 DATA_STRING, _X("RM433","RM433"),
                "id",       "id",               DATA_FORMAT, "0x%04X", DATA_INT, addr,
                "button",   "button",           DATA_FORMAT, "0x%02X", DATA_INT, key,
                NULL);
        decoder_output_data(decoder, data);
        return 1;
    }
    return 0;
}

static char *output_fields[] = {
    "brand",
    "model",
    "id",
    "button",
    NULL
};

r_device sonoff_rm433 = {
    .name           = "Sonoff RM433 remote",
    .modulation     = OOK_PULSE_PWM,
    .short_width    = 248,
    .long_width     = 728,
    .reset_limit    = 7552,
    .gap_limit      = 728,
    //.reset_limit    = 15000,
    .sync_width     = 0,    // No sync bit used
    .tolerance      = 200, // us
    .decode_fn      = &rm433_callback,
    .disabled       = 0,
    .fields         = output_fields,
};
