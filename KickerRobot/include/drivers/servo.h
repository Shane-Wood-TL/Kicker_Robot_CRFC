#ifndef __servo__
#define __servo__

#include "../all_includes.h"
class servo{
  private:
    ledc_channel_t ledCTimer;
    int16_t Hlimit;
    int16_t Llimit;
    bool direction;
    int8_t offset;
  public:
  servo(ledc_channel_t ledCTimerV, uint16_t pwmHighLimitV, uint16_t pwmLowLimitV, bool directionV,int8_t offsetV);
  void setDegree(uint16_t nDegree);
};

#endif