#include <math/mosek.h>

#include <cassert>

namespace math {
  mosek::~mosek() { }

  // data chunk (index bounds for variables/constraints)
  mosek::chunk::chunk(natural start, natural end) 
    : start(start), end(end) { 
    assert( end >= start );
  }

  mosek::variable::variable(natural start, natural end) 
    : chunk(start, end) {
  }

  mosek::constraint::constraint(natural start, natural end) 
    : chunk(start, end) {
  }


  natural mosek::chunk::dim() const { return end - start; }
    

  static bool overlap(const mosek::chunk& x, const mosek::chunk& y) {
    
    return (x.start == y.start) ? true :
      (x.start < y.start) ? x.end > y.start : // x before
      y.end > x.start;			      // y before
    
  }

  static bool equal(const mosek::chunk& x, const mosek::chunk& y)  {
    return (x.start == y.start) && (x.end == y.end);
  }

  static bool comparable(const mosek::chunk& x, const mosek::chunk& y) {
    // std::cout << "compare: " << x.start << " " << x.end << " / " 
    // 	      << y.start << " " << y.end << std::endl;
    return equal(x, y) || !overlap(x, y);
  }

  bool mosek::variable::operator<(const variable& other) const {
    assert( comparable(*this, other) );
    return start < other.start;
  }

  bool mosek::variable::operator==(const variable& other) const {
    return equal(*this, other);
  }

  bool mosek::variable::operator<=(const variable& other) const {
    return (*this < other) || (*this == other);
  }

  bool mosek::constraint::operator==(const constraint& other) const {
    return equal(*this, other);
  }
  
  bool mosek::constraint::operator<(const constraint& other) const {
    assert( comparable(*this, other) );
    return start < other.start;
  }

  bool mosek::constraint::operator<=(const constraint& other) const {
    return (*this < other) || (*this == other);
  }


  mosek::variable mosek::variable::after( natural dim)  const {
    variable res;
    res.start = end;
    res.end = res.start + dim;
    return res;
  }


  mosek::constraint mosek::constraint::after(natural dim) const {
    constraint res;
    res.start = end;
    res.end = res.start + dim;
    return res;
  }
   
}



