#ifndef MATH_MOSEK_H
#define MATH_MOSEK_H

#include <math/types.h>
#include <math/vec.h>
#include <math/error.h>
#include <map>

namespace math {

  struct mosek {
      
    virtual ~mosek();

    enum struct bound {
      lower,
	upper,
	fixed,
	};

    struct chunk {
      chunk( natural start = 0,
	     natural end = 0);
	
      natural start, end;
      natural dim() const;
      
    };

    struct variable : chunk { 
      
      variable( natural start = 0,
		natural end = 0);

      bool operator<(const variable& ) const;
      bool operator<=(const variable& ) const;
      bool operator==(const variable& ) const;

      variable after(natural dim) const;
    };

    struct constraint : chunk { 
      
      constraint( natural start = 0,
		  natural end = 0);

      bool operator<(const constraint& ) const;
      bool operator<=(const constraint& ) const;
      bool operator==(const constraint& ) const;
      
      constraint after(natural dim) const;
    };
    

    typedef std::map< variable, std::map< variable, math::mat > > sparse_qobj;

    struct exception : core::exception { };

    struct infeasible : exception { vec result; };
    struct unknown_status : exception { vec result; };


    // declare variables/constraints
    virtual mosek& init(math::natural variables = 0, math::natural constraints = 0) = 0; 
    
    // append (free) variables/constraints
    virtual mosek& add(const variable& ) = 0;
    virtual mosek& add(const constraint& ) = 0;
    
    virtual mosek& clear() = 0;

    // linear term in objective
    virtual void c(const variable& v, const math::vec& ) = 0;
    
    // quadratic matrix in objective
    virtual void Q(const variable& v, const math::mat& ) = 0; // diagonal
    virtual void Q(const variable& i, const variable& j, const math::mat& ) = 0; // (lower) off-diagonal

    virtual void add_Q(const variable& v, const math::mat& ) = 0; // diagonal
    virtual void add_Q(const variable& i, const variable& j, const math::mat& ) = 0; // (lower) off-diagonal
      
    // quadratic constraint
    virtual void K(const constraint& c, const variable& v, const math::mat& ) = 0;
    virtual void K(const constraint& c, const variable& vi, const variable& vj, const math::mat& ) = 0;


    // linear constraint matrix
    virtual void A(const constraint& i, const variable& j, const math::mat& ) = 0;
      
    // rhs for constraints
    virtual void b(const constraint& i, const math::vec& , bound ) = 0;
    virtual void b(const constraint& i, const math::vec& lower, const math::vec& upper) = 0;
    virtual void b(const constraint& i ) = 0; // disables constraint
    
      
    // bounds on variables
    virtual void bounds(const variable& v, const math::vec& , bound  ) = 0;
    virtual void bounds(const variable& v, const math::vec& low, const math::vec& up  ) = 0;
    virtual void bounds(const variable& v ) = 0;

    
    // solves and returns part of the solution
    virtual math::vec solve(const variable& v) throw( math::mosek::exception ) = 0;
      
    virtual void debug( bool ) = 0;
    virtual void write(const std::string& filename) const = 0;
    
    virtual variable variables() const = 0;
    virtual constraint constraints() const = 0;

    virtual mosek& relax() = 0;

    virtual mosek& read(const std::string& filename) = 0;
    
    virtual void Q(const sparse_qobj& ) = 0;

      
  };
  
  

}



#endif
