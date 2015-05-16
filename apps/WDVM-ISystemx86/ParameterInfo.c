#include "ParameterInfo.h"

ParameterInfo ParameterInfo_Read(ILFILE * fd)
{
	ParameterInfo pi;
    pi.type=bh_readbyte(fd);
    if (pi.type==Param_Float)
    {
        pi.value_float=bh_readdouble(fd);
    }
    else
    {
        pi.value=bh_readint64(fd);
    }
    return pi;
}
