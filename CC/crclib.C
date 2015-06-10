#include "crclib.h"

unsigned int amscrc_update(unsigned int amscrc, unsigned int *buf, int buf_size)
{
    int beg;
    if (amscrc == 0) {
        amscrc =~ buf[0];
        beg = 1;
    } else {
        beg = 0;
    }
    int m, j;
    for(m=beg; m<buf_size/sizeof(buf[0]); m++){
        for(j=0; j<3; j++) {
            amscrc = CRC32_TABLE[amscrc>>24]^(amscrc<<8);
        }
        amscrc = amscrc^buf[m];
    }
    return amscrc;
}

unsigned int amscrc_finalize(unsigned int amscrc)
{
    return ~amscrc;
}

unsigned int crc_update(unsigned int crc, unsigned int *buf, int buf_size, int read_size, Long64_t file_size, Long64_t *pos)
{
    int m,n;
    for(m=0; m<=buf_size/sizeof(buf[0]) && m<read_size; m++) {
        for(n=0; n<sizeof(buf[0]); n++) {
            if ((*pos)++ >= file_size) break;
            unsigned char byte = *((char *)&buf[m] + n);
            COMPUTE(crc, byte);
        }
    }
    return crc;
}

unsigned int crc_finalize(unsigned int crc, long long len)
{
    for (; len != 0; len >>= 8) {
        COMPUTE(crc, len & 0xff);
    }
    return ~crc;
}
