#ifndef MATH_TYPES_H
#define MATH_TYPES_H

namespace math {
  
  typedef unsigned int natural;
  typedef double real;
  
  struct size {
    // TODO check Eigen/src/Core/util/Constants.h
    static const int dynamic = -1;
  };
  
  // vectors
  template<class = real, int = size::dynamic> struct vector;
  
  typedef vector<real> vec;

  typedef vector<real, 1> vec1;
  typedef vector<real, 2> vec2;
  typedef vector<real, 3> vec3;
  typedef vector<real, 4> vec4;
  typedef vector<real, 5> vec5;
  typedef vector<real, 6> vec6;
  typedef vector<real, 8> vec8;
  typedef vector<real, 12> vec12;

  // covectors 
  template<class = real, int = size::dynamic> struct covector;
  
  typedef covector<real> form;
  
  typedef covector<real, 1> form1;
  typedef covector<real, 3> form3;
  typedef covector<real, 4> form4;
  typedef covector<real, 6> form6;
  
  // matrices
  template<class = real, int = size::dynamic, int = size::dynamic> struct matrix;
  
  typedef matrix<real> mat;
  
  typedef matrix<real, 2, 2> mat22;
  typedef matrix<real, 3, 3> mat33;
  typedef matrix<real, 3, 2> mat32;
  typedef matrix<real, 2, 3> mat23;
  typedef matrix<real, 4, 4> mat44;
  typedef matrix<real, 4, 3> mat43;
  typedef matrix<real, 6, 6> mat66;
  typedef matrix<real, 3, 6> mat36;


  // machine precision
  template<class U> struct precision;

  // euclidean space
  namespace euclid {
    template<class E> struct traits;
    template<class E> class space;
  }

  // lie groups
  namespace lie {
    template<class G> struct traits;
    template<class G> class group;
  }

  // functions
  namespace func {
    template<class F, class = void > struct traits;

    template<class F, int M = size::dynamic> struct vector; 
    template<class F, int M = size::dynamic> struct covector;

    template<class F> struct base;

    template<class LHS, class RHS> struct compose;
  }

  // matrix groups 
  template<class U = real> class rigid;
  typedef rigid<> SE3;

  template<class U = real> class rotation;
  typedef rotation<> SO3;
  
  // quaternions
  template<class U = real> struct quaternion;
  typedef quaternion<> quat;

  template<class U = real> class dual_quaternion;
  typedef dual_quaternion<> dual_quat;
  
  
  // kinematics
  typedef vec6 twist;
  typedef form6 wrench;
  
  typedef form3 force;
  typedef form3 torque;

}



#endif
