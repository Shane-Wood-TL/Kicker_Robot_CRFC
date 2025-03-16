/**
 * @file changeable_values.h
 * @brief Contains the information for the changeable values class, this class allows a value
 * to be modifed by a set value with high and low limits setable
 * @author Shane Wood
 * @date 27/2/2025
 */
#ifndef __changeable_values__
#define __changeable_values__

#include "../all_includes.h"

/**
 * @class changeable_values
 * @brief A class to handle values that can be changed by a set amount
 * 
 * @tparam T The type of the value
 */
template <typename T>
class changeable_values{
    private:
        T* value; ///< the value that can be changed
        T min_value; ///< the minimum value that the value can be
        T max_value; ///< the maximum value that the value can be
        T change_by; ///< the amount that the value changes by
    public:
        /**
         * @brief Constructor for the changeable_values class
         * 
         * @param value : T* The value that can be changed
         * @param min_value : T The minimum value that the value can be
         * @param max_value : T The maximum value that the value can be
         * @param change_by : T The amount that the value changes by
         */
        changeable_values(T *value, T min_value, T max_value, T change_by);

        /**
         * @brief Increases the value by the change_by amount
         */
        void increase_value();

        /**
         * @brief Decreases the value by the change_by amount
         */
        void decrease_value();

        /**
         * @brief Gets the value
         * 
         * @return T The value
         */
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
        *value = *value + change_by; //increase value if in range
    }
    if(*value > max_value){
        *value = max_value; //else: do nothing
    }
}

template <typename T>
void changeable_values<T>::decrease_value(){ //decrease value if in range
    if(*value > min_value){
        *value = *value - change_by;
    }
    if(*value < min_value){
        *value = min_value; //else: do nothing
    }
}

template <typename T>
T changeable_values<T>::get_value(){ //return the value
    return *value;
}

#endif