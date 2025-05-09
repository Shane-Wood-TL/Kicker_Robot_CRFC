/**
 * @file edge_dector.cpp
 * @brief This is the file that contains the edge (rising) dector class
 * @author Shane Wood
 */
#include "../../include/supporting/edge_dector.h"



edge_dector::edge_dector(bool current_state) : old_state(current_state) {
}


//detects rising edge
bool edge_dector::update(bool new_state) {
  if (old_state == false && new_state == true) {
    old_state = new_state;
    return true;
  } else {
    old_state = new_state;
    return false;
  }
}