#ifndef __changeable_values__
#define __changeable_values__

#include "all_includes.h"

template <typename T>
struct changeable_values{
    T* value;
    T min_value;
    T max_value;
    T change_by;
};
#endif