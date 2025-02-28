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
 * @brief This class is for creating a value than can be changed by a set amount with limits
 * @details To be used with the menu class to ensure that values are modified within their bounds and modifed saftely
 * @tparam T the type of object that will be changed (needs to be numerical)
 */
template <typename T>
class changeable_values{
    private:
        T* value; ///< pointer to the value that can be changed
        T min_value; ///< minimum value that the value to change can be
        T max_value; ///< maximum value that the value to change can be
        T change_by; ///< value to change the value to change by
    public:
        /**
         * @brief Constructor for the changeable_values class, type needs to be set during construction
         * 
         * @tparam T type determined during construction
         * @param value : *T A pointer to the value to be changed
         * @param min_value : T The lowest that the value can be changed to
         * @param max_value : T The highest that the value can be changed to
         * @param change_by : T The amount that the value can be change by
         */
        changeable_values(T *value, T min_value, T max_value, T change_by);

        /**
         * @brief Increase value by change_by if value is within its set bounds
         */
        void increase_value();

        /**
         * @brief decrease value by change_by if value is within its set bounds
         */
        void decrease_value();

        /**
         * @brief Return the value of the variable value
         * 
         * @tparam T type determined during construction
         * @return : T The value of the variable value
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