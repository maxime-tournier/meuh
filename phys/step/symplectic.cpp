#include "symplectic.h"

#include <math/mat.h>
#include <math/qp.h>


#include <math/bilevel.h>
#include <math/opt/bokhoven.h>


namespace phys {
  namespace step {


    void unconstrained::operator()(math::vec& x, 
				   const math::vec& f) const {
	
      x = math::minres(M).solve(f, iter, x);
	
    }

    
    void constrained::operator()(math::vec& x, 
				 const math::vec& f) const {
	
      if( ! constraints.k ) {
	unconstrained{M, iter}(x, f);
	return;
      }
	
      auto A = [&](math::vec::rchunk x) { return constraints.apply(x); };
      auto AT = [&](math::vec::rchunk x) { return constraints.applyT(x); };
	

      script::require("plot");

      std::string name = "bilevel";
      script::exec("%% = plot.new(); %%.title = { 'residual (uncontrolled)' }; %%.set.logscale = 'y'",
		   name, name, name);
      math::iter it = iter;
      it.cb = [&](math::natural k, math::real eps){
	script::exec("plot.push(%%, %%, %%)", name, k, eps);
      };
      

      math::vec slack = math::vec::Zero(constraints.m + constraints.n);

      // x = math::qp(M, A, AT, constraints.m).stable(-f, constraints.rhs(), it, 1e-5, x, &slack );      

      auto qp = math::opt::qp::make_bokhoven(M, A, AT);

      qp.eps = 1e-1;
      qp.slack = &slack;
      qp.bilateral = constraints.m;
      
            // TODO make warm start configureable
      x = qp.solve(-f, constraints.rhs(), it); //, 0, x, &slack );
      
      // TODO make warm start configureable
      lambda = sparse::dense( constraints.bilateral.matrix.rows(), slack.head( constraints.m) );
      mu = sparse::dense( constraints.unilateral.matrix.rows(), slack.tail( constraints.n) );
      
    }	    


    void fake_control::operator()(math::vec& x, 
				  const math::vec& f) const {
       
      // if( ! kinematic.k ) {
      // 	unconstrained{M, iter}(x, f);
      // 	return;
      // }
       
      auto A = [&](math::vec::rchunk x) { return kinematic.apply(x); };
      auto AT = [&](math::vec::rchunk x) { return kinematic.applyT(x); };
      
      script::require("plot");

      std::string name = "bilevel";
      script::exec("%% = plot.new(); %%.title = { 'residual (fake control)' }; %%.set.logscale = 'y'",
		   name, name, name);
      math::iter it = iter;
      it.cb = [&](math::natural k, math::real eps){
	script::exec("plot.push(%%, %%, %%)", name, k, eps);
      };
      
      math::vec slack = math::vec::Zero(kinematic.m + kinematic.n);
      
      math::vec tmp1, tmp2;
      auto QQ = [&](math::vec::rchunk x) -> const math::vec& {
	tmp1.noalias() = control.obj.matrix * x;
	tmp2.noalias() = control.obj.matrix.transpose() * tmp1;
	return tmp2;
      };
      
      math::vec tmp3;
      auto Q = [&](math::vec::rchunk x) -> const math::vec& {
	tmp3.noalias() = M * x;
	if( !control.obj.matrix.empty() ) tmp3 += QQ(x);
	return tmp3;
      };
      
      // auto qp = math::opt::qp::make_bokhoven(Q, A, AT);
      auto qp = math::qp(Q, A, AT);
      
      // auto damping !
      qp.eps = iter.epsilon;
      qp.slack = &slack;

      qp.bilateral = kinematic.m; // + kinematic.n;
      
      math::vec c = -f;
      if( !control.obj.vector.empty() ) c -= control.obj.matrix.transpose() * control.obj.vector;
      

      math::vec b = kinematic.rhs();

      x = qp.solve(c, b, it); //, 0, x, &slack );
      
      // x = math::qp(Q, A, AT, kinematic.m).stable(-f + control.obj.vector, 
      // 						 kinematic.rhs(), it, 0, x, &slack );
      
      if( kinematic.m ) lambda = sparse::dense( kinematic.bilateral.matrix.rows(), slack.head( kinematic.m) );
      if( kinematic.n ) mu = sparse::dense( kinematic.unilateral.matrix.rows(), slack.tail( kinematic.n) );
      
     }	    
      
  
  

    void control_constrained::operator()(math::vec& v, 
					 const math::vec& f) const {

      if( !kinematic.k ) {
	unconstrained{M, iter}(v, f);
	return;
      }

      using namespace math;

      natural k = kinematic.k;
      natural m = kinematic.m;
      natural n = kinematic.n;
      
      natural a = control.act.matrix.rows();
      
      auto& A = control.act.matrix;
      					
      auto Q = [&]( const vec& x) {
	return control.obj.matrix.transpose() * ( control.obj.matrix * x);
      };
      
      auto& K = control.metric.matrix;

      auto D = [&]( const math::vec& x) {
	vec res = vec::Zero(k);

	res = M(x.head(k)) - A.transpose() * x.tail(a);
	return res;
      };


      auto DT = [&]( const math::vec& x) {
	vec res = vec::Zero(k + a);

	res.head(k) = M(x);
	res.tail(a) = - A * x;
	
	return res;
      };
      

      auto QQ = [&](const math::vec& x) {
	assert( x.size() == k + a );
	  
	vec res = vec::Zero( k + a );
	
	res.head(k) = Q( res.head(k) );
	res.tail(a) = K( x.tail(a) );
	
	return res;
      };

     	
      // kinematic constraints
      auto J = [&](math::vec::rchunk x) {
	return kinematic.apply(x);
      };
      
      auto JT = [&](math::vec::rchunk x) {
	return kinematic.applyT(x);
      };
	
      vec c = vec::Zero(k + a);
      
      c.head(k) = - control.obj.matrix.transpose() * control.obj.vector;
      c.tail(a) = control.metric.vector;
      
      vec b = kinematic.rhs();
	
      script::require("plot");

      std::string name = "bilevel";
      script::exec("%% = plot.new(); %%.title = { 'residual' }; %%.set.logscale = 'y'",
		   name, name, name);
      math::iter it = iter;
      it.cb = [&](math::natural k, math::real eps){
	script::exec("plot.push(%%, %%, %%)", name, k, eps);
      };
      
      if( warm.empty()) {
      	warm.resize( k + a );
      	warm << v, act;
      }
      
      vec slack = vec::Zero(m + n);
      
      auto qp = math::bilevel(QQ, D, DT, J, JT, m);

      qp.iter = it;
      qp.slack = &slack;
      qp.eps = 1e-5;
      
      // qp.bilateral = n;

      // TODO warm ?
      vec sol = qp.solve(c, f, b); 
      
      warm = sol;
      
      v = sol.head(k);
      act = sol.tail(a);
      
      lambda = sparse::dense( kinematic.bilateral.matrix.rows(), slack.head(m) );
      mu = sparse::dense( kinematic.unilateral.matrix.rows(), slack.tail(n) );

      // core::log("objective:", (control.obj.matrix * v - control.obj.vector).array().square().transpose());
      // core::log("act:", act.transpose());

    }

  }
}
