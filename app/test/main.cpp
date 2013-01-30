#include <core/range/iterator.h>
#include <core/range/map.h>

#include <phys/dof.h>
#include <phys/constraint/bilateral.h>

#include <sparse/vector.h>
#include <sparse/matrix.h>
#include <sparse/sum.h>
#include <sparse/diff.h>
#include <sparse/scalar.h>
#include <sparse/dot.h>
#include <sparse/prod.h>


#include <core/timer.h>

template<class F>
struct make_pair {
  const F fun;
  

  template<class First, class Second>
  auto operator()(const std::pair<First, Second>& p) const 
    -> decltype( fun(p.first, p.second) ) {
    return fun(p.first, p.second);
  }

  template<class First, class Second>
  auto operator()(std::pair<First, Second>& p) const 
    -> decltype( fun(p.first, p.second) ) {
    return fun(p.first, p.second);
  }
  
};

template<class F>
make_pair<F> pair(const F& fun) { return {fun}; }
 
int main(int, char** ) {
  using namespace phys;
  using namespace math;
  
  std::vector< dof* > dofs(1000000);
  core::each(dofs, [](dof*& d) { d = new dof(3); } ); 
  

  dof* d0 = dofs[0];
  
  sparse::vector< dof::key > v1(dofs.size()), u1(1);
  sparse::matrix< dof::key, dof::key > M1(dofs.size());
  
  
  auto fast = [&] {
    for(int i = 0; i < dofs.size(); ++i) {
      auto d = dofs[dofs.size() -1 - i];
      v1(d).setZero();
      M1(d, d).setIdentity();
      M1(d, d0).setIdentity();
    }
  };

 
  
  u1(dofs[dofs.size() -1]).setOnes();

  
  auto sum_fast = [&] {
    using sparse::operator+;
    using sparse::operator-;
    v1 = v1 - u1 + v1;
  };



  auto dot_fast = [&] {
    math::real d = sparse::dot(v1, v1);
  };


  auto prod_fast = [&] {
    using sparse::operator*;
    using sparse::operator+;
    v1 = sparse::transp(M1) * (M1 * v1);
  };

 


  core::log("fast:", core::time(fast) );
  core::log("sum_fast", core::time(sum_fast));
  core::log("dot_fast", core::time(dot_fast));
  core::log("prod_fast", core::time(prod_fast));
  

}

 
