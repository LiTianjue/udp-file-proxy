#ifndef MHP_TIMER_H
#define MHP_TIMER_H

class MHP_TIMER
{
public:
    MHP_TIMER(){}

public:
    static void mhp_sleep(int s);
    static void mhp_msleep(int ms);
    static void mhp_usleep(int us);
    static void mhp_sm_sleep(int s,int ms);
};

#endif // MHP_TIMER_H

