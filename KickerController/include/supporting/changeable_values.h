#ifndef __changeable_values__
#define __changeable_values__

#include "../all_includes.h"

template <typename T>
class changeable_values{
    private:
        T* value;
        T min_value;
        T max_value;
        T change_by;
    public:
        changeable_values(T *value, T min_value, T max_value, T change_by);
        void increase_value();
        void decrease_value();
        T get_value();
};

template <typename T>
changeable_values<T>::changeable_values(T *value, T min_value, T max_value, T change_by){
    this->value = value;
    this->min_value = min_value;
    this->max_value = max_value;
    this->change_by = change_by;
}

template <typename T>
void changeable_values<T>::increase_value(){
    if(*value < max_value){
        *value = *value + change_by;
    }
    if(*value > max_value){
        *value = max_value;
    }
}

template <typename T>
void changeable_values<T>::decrease_value(){
    if(*value > min_value){
        *value = *value - change_by;
    }
    if(*value < min_value){
        *value = min_value;
    }
}

template <typename T>
T changeable_values<T>::get_value(){
    return *value;
}

#endif