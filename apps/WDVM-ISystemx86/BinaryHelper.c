#include "BinaryHelper.h"
#include <ilib/ilib.h>

void bh_saferead(unsigned char * buf, int block, int count, ILFILE * fd)
{
    int read = 0, requ = count;
    do
    {
        int r = ILReadFile(fd, buf, block * count);
        read+=r;
    } while(read<requ);
}

unsigned char bh_readbyte(ILFILE * fd)
{
    unsigned char buf;
    bh_saferead(&buf, 1, 1, fd);
    return buf;
}

unsigned short bh_readuint16(ILFILE * fd)
{
    unsigned short a = 0;
    unsigned char buf[2];
    bh_saferead(buf, 2, 1, fd);
    a |= ((unsigned short)buf[0]);
    a |= ((unsigned short)buf[1]) << 8;
    return a;
}

int bh_readint32(ILFILE * fd)
{
    int a = 0;
    unsigned char buf[4];
    bh_saferead(buf, 4, 1, fd);
    a |= ((int)buf[0]);
    a |= ((int)buf[1]) << 8;
    a |= ((int)buf[2]) << 16;
    a |= ((int)buf[3]) << 24;
    return a;
}

long long bh_readint64(ILFILE *fd)
{
    long long a = 0;
    unsigned char buf[8];
    bh_saferead(buf, 8, 1, fd);
    a |= ((long long)buf[0]);
    a |= ((long long)buf[1]) << 8;
    a |= ((long long)buf[2]) << 16;
    a |= ((long long)buf[3]) << 24;
    a |= ((long long)buf[4]) << 32;
    a |= ((long long)buf[5]) << 40;
    a |= ((long long)buf[6]) << 48;
    a |= ((long long)buf[7]) << 56;
    return a;
}

double bh_readdouble(ILFILE *fd)
{
    long long a = bh_readint64(fd);
    return *((double *)(&a));
}
