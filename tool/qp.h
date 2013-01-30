#ifndef TOOL_QP_H
#define TOOL_QP_H

#include <math/qp.h>

namespace tool {

  // a somewhat more usable QP adaptor class
  class qp {
    math::vec _b, _c;

    // quadratic terms: vout += Q * vin
    typedef std::function< void(math::vec& out, math::vec::rchunk in) > linear_type;
    std::vector<linear_type> metric;
    
    // constraint terms: cout = A * vin
    std::vector<linear_type> constraint;
    
    // constraintT terms: vout += A^T * cin
    std::vector<linear_type> constraintT;
    
  public:
    
    qp();

    void clear();

    void add_var(math::natural dim = 1);
    void add_con(math::natural dim = 1);
    
    math::natural vars() const;
    math::natural cons() const;
    
    // adds an objective term
    template<class Q>
    void add_q( const Q& q ) { 
      metric.push_back( q ); 
    }

    // adds a constraint term
    template<class A, class AT>
    void add_a( const A& a, const AT& aT ) { 
      constraint.push_back( a ); 
      constraintT.push_back( aT ); 
    }
    
    math::vec::lchunk b();
    math::vec::rchunk b() const;

    math::vec::lchunk c();
    math::vec::rchunk c() const;
    
    math::real eps;
    math::iter iter;
    
    void solve(math::vec& x) const;
    
  };

}



#endif
