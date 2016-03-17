#include "mhp_timer.h"
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>

void MHP_TIMER::mhp_sleep(int s)
{
    struct timeval timeout;
    timeout.tv_sec=s;
    timeout.tv_usec=0;
    int ret =select(0,NULL,NULL,NULL,&timeout);
}


void MHP_TIMER::mhp_msleep(int ms)
{
    struct timeval timeout;
    timeout.tv_sec=0;
    timeout.tv_usec=ms*1000;
    int ret =select(0,NULL,NULL,NULL,&timeout);
}


void MHP_TIMER::mhp_usleep(int us)
{
    struct timeval timeout;
    timeout.tv_sec=0;
    timeout.tv_usec=us;
    int ret =select(0,NULL,NULL,NULL,&timeout);
}


void MHP_TIMER::mhp_sm_sleep(int s, int ms)
{
    struct timeval timeout;
    timeout.tv_sec=s;
    timeout.tv_usec=ms*1000;
    int ret =select(0,NULL,NULL,NULL,&timeout);
}
