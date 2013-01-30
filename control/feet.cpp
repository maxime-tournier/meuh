#include "feet.h"

#include <iostream>

namespace control {

  feet::result_type feet::operator()(const math::T<math::vec3>& left,
				     const math::T<math::vec3>& right )
  {
    bool left_landed =  (left.at() - target.left ).norm() < dist;
    bool right_landed =  (right.at() - target.right ).norm() < dist;

    std::cout << "feet control: ";
    if( left_landed ) std::cout << "L";
    if( right_landed ) std::cout << "R";
    std::cout << std::endl;
    
    
    if( left_landed && right_landed ) {
      *next += step_length * direction;

      if( next == &target.left ) next = &target.right;
      else if( next == &target.right ) next = &target.left; 
      else throw core::exception();
    } 

    result_type res;
    res.left = pd(left, target.left);
    res.right = pd(right, target.right);


    return res;
  }








}
