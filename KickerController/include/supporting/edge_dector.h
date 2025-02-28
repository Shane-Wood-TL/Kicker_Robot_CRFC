/**
 * @file edge_dector.h
 * @brief This is the file that contains the class for edge detection
 * @author Shane Wood
 * @date 9/2/2025
*/

#ifndef __edge_dector__
#define __edge_dector__
/**
 * @class edge_dector
 * @brief This class is for edge detection (rising edge)
 */
class edge_dector{
    private:
        bool old_state; ///< The last state of the edge detector
    public:
        /**
         * @brief Construct a new edge dector object
         * 
         * @param current_state : bool The current state of the edge detector
         */
        edge_dector(bool current_state);

        /**
         * @brief This function updates the edge detector
         * 
         * @param new_state bool : The new state of the edge detector
         * 
         * @return bool returns true if the edge detector has detected a rising edge
         */
        bool update(bool new_state);
};

#endif