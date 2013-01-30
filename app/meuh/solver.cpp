#include <sparse/matrix.h>

#include "solver.h"

#include <core/log.h>

#include <phys/solver/simple.h>
#include <phys/solver/linear.h>

#include <phys/solver/test.h>

#include <phys/solver/cg.h>
#include <phys/solver/gs.h>
#include <phys/solver/cholesky.h>

#include <sparse/weight.h>
#include <sparse/diag.h>

#include <core/unused.h>
#include <core/timer.h>

#include <math/random.h>
#include <math/cond.h>

static std::function< phys::solver::test::data_type() > previous_data;
static std::function< phys::constraint::bilateral::weight () > previous_lambda;


static std::list< std::unique_ptr<phys::dof> > dofs_pool;
static std::list< std::unique_ptr<phys::constraint::bilateral> > bilateral_pool;

Config& Solver::config() { return configuration; }
// const Config& Solver::config() const { return configuration; }

phys::dof* Solver::make_dof(unsigned int dim) { 
  auto res = new phys::dof(dim);
  dofs_pool.push_back( std::unique_ptr<phys::dof>(res) );
  
  return res;
}

phys::constraint::bilateral* Solver::make_bilateral(unsigned int dim) { 
  auto res = new phys::constraint::bilateral(dim);
  bilateral_pool.push_back( std::unique_ptr<phys::constraint::bilateral>(res) );

  return res;
}

template<class C>
static void check(const C& c, int i) {
  core::use(c, i);
  assert( i < int(c.size()) );
  assert( i >= 0 );
}


int Solver::addDof( int dim ) {
  assert( dim > 0 );
  
  int res = dofs.size();
  dofs.push_back( make_dof(dim) );
  return res;
}


int Solver::addBilateral( int dim ) {
  assert( dim > 0 );
   
  int res = bilateral.size();
  bilateral.push_back(make_bilateral(dim) );
  return res;
}


Vector Solver::getMomentum(int i) {
  check(dofs, i);
  
  return momentum( dofs[i] );
}


Vector Solver::getVelocity(int i) {
  check(dofs, i);
  
  return velocity( dofs[i] );
}


Vector Solver::getDelta(int i) {
  check(dofs, i);
  
  return delta( dofs[i] );
}


void Solver::setMass(int i, double m) {
  check(dofs, i);

  assert( m > 0 );

  auto d = dofs[i];

  system.mass( d, d ) = m * math::mat33::Identity();
  system.resp( d, d ) = (1/m) * math::mat33::Identity();
}


void Solver::solve() {
  assert( impl );
  
  phys::solver::task task( velocity );
  task.momentum = momentum;
  task.bilateral = system.constraint.bilateral.values;
  
  // warm-start
  task.lambda = &lambda;
  
  // core::log("solving");
  impl.solve(task);
}

void Solver::correct() {
  assert( impl );
  
  phys::solver::task task( delta );
  task.bilateral = system.constraint.bilateral.corrections;
  
  // TODO TEST is this needed ?
  task.momentum = momentum;

  // warm-start
  task.lambda = &mu;

  task.momentum.reserve( system.mass.rows().dim() );
  mu.reserve( system.constraint.bilateral.matrix.rows().dim() );
  
  // core::log("correcting");
  impl.solve(task);

  // remember weights for next time-step
  if( !previous_lambda) {
    sparse::weight<phys::constraint::bilateral::key> w(mu.keys().size());
    system.constraint.bilateral.matrix.rows().each([&](phys::constraint::bilateral::key c, core::unused) {
	w(c) = - mu(c).norm();
      });
    previous_lambda = [w] { return w; };
  }

  if( config().graph().getType() == Graph::NONE ) {
    // TODO extract and display exact MST

  }

}


Matrix Solver::getBilateralMatrix(int c, int d) {
  check(bilateral, c);
  check(dofs, d);

  return system.constraint.bilateral.matrix( bilateral[c], dofs[d] );
}


Vector Solver::getBilateralValues(int c) {
  check(bilateral, c);

  return system.constraint.bilateral.values( bilateral[c] );
}

Vector Solver::getBilateralCorrections(int c) {
  check(bilateral, c);

  return system.constraint.bilateral.corrections( bilateral[c] );
}



void Solver::clear() {
  system.clear();
  
  dofs.clear();
  bilateral.clear();

  velocity = phys::dof::vector();
  momentum = phys::dof::vector();
  delta = phys::dof::vector();
  
  lambda = phys::constraint::bilateral::vector();
  mu = phys::constraint::bilateral::vector();

  // TODO clear config ?

  previous_data = 0;
  previous_lambda = 0;

  script::exec("log.clear()");
}






template<class C>
static std::set<C> sort(const std::vector<C>& data) {
  return std::set<C>(data.begin(), data.end());
}




bool Solver::inTree(int c) const {
  if (!in_tree) return false;
  return in_tree( bilateral[c] );
}




void Solver::factor(bool fast) { 
  
  auto type = config().graph().getType();
  
  const bool none = (type == Graph::NONE) || (type == Graph::PGS) || (type  == Graph::CG);
  

  // TODO do we count matrix filling ?
  core::timer timer;
  
  using namespace phys;
  
 
  const bool condition = false;
  
  if( ! none ) {
    
    using namespace phys;
    using namespace solver;
    
    core::log("graph type:", type);

    auto build_data = [&,type] {
      core::log("building graph", type);
      auto graph = test::graph(system);
      test::edge_set edges;
    
      switch( type ) {
      case Graph::DFS: 
	edges = test::dfs( graph );
	break;
      case Graph::RND: 
	edges = test::rnd( graph );
	break;
      case Graph::CMK: 
	edges = test::cuthill_mckee( graph );
	break;
      case Graph::MST: {
	
	sparse::weight<constraint::bilateral::key> w(mu.keys().size());

	if( previous_lambda ) w = previous_lambda();
	else {
	  system.constraint.bilateral.matrix.rows().each([&](constraint::bilateral::key c, core::unused) {
	      w(c) = 0;
	    });
	}
	
	edges = test::mst( graph, w );
	previous_lambda = 0;
	
      } break;  
      case Graph::PROP: {
	dof::velocity tmp;
	solver::task task(tmp);
	
	// task.momentum = momentum;
	task.bilateral = system.constraint.bilateral.corrections;
	
	edges = test::prop(graph, system, task);
      } break;

      default:
	break;
      }
    
      assert( !edges.empty() );
      
      const bool perturbation = false;
      if( perturbation ) {
	auto replaced = math::rand<math::natural>(0, edges.size() - 1);
	auto candidates = test::perturb(graph, edges, edges[ replaced  ] );
	
	if(! candidates.empty() ) {
	  edges[ replaced ] = candidates[ math::rand<math::natural>(0, candidates.size() - 1) ];
	}
	
      }
      
      return test::data_type{ test::span(graph, edges) };
    };
    
    auto data = fast ? previous_data() : build_data();
    previous_data = [data] { return data; };
    
    // data.factor( system, config().cg().getDamping() );
    data.factor_precond(system, config().cg().getDamping() );
    
    test::linear linear( std::move(data) );
    
    // extract cycles/single matrices
    phys::constraint::bilateral::matrix cycles(system.constraint.bilateral.matrix.keys().size());
    
    auto index = system.constraint.bilateral.matrix.index();

    core::each( core::all( linear.data.span.cycles ), [&](phys::constraint::bilateral::key c) {
	core::each( index[c], [&](dof::key d) {
	    cycles(c, d) = system.constraint.bilateral.matrix(c, d);
	  });
      });
    
    auto acyclic_index = linear.data.resp_constraint.index();
    
    in_tree = [acyclic_index](constraint::bilateral::key c) {
      return acyclic_index.find(c) != acyclic_index.end();
    };
    
    const bool use_prec = config().cg().getPrec();
    
    auto prec_big = [use_prec, linear](const constraint::bilateral::vector& lambda) 
      -> constraint::bilateral::vector {
      if(use_prec) {
	dof::velocity v;
	task t(v);
	t.bilateral = lambda;

	constraint::bilateral::vector res;
	t.lambda = &res;
	
	// t.momentum = sparse::transp( cycles ) * lambda;
 
	linear.precond( t );
	// linear.solve( t );
	
	return res;
      }
      else return lambda;
    };


    auto prec_small = [use_prec, linear](const constraint::bilateral::vector& lambda) 
      -> constraint::bilateral::vector {
      if(use_prec) {
	auto res = (linear.data.resp_constraint * lambda);
	
	return res;
      }
      else return lambda;
    };
    
    
 
    const bool compare = true;
    
    if( compare ) {
      phys::solver::simple simple(system);
      
      // big schur
      auto res_prec = make_cg(simple , system.constraint.bilateral.matrix, prec_big );
      auto res_unprec = make_cg( simple, system.constraint.bilateral.matrix);
      
      res_prec.config.iterations = config().cg().getIterations();
      res_prec.config.precision = config().cg().getError();
      res_prec.config.damping = config().cg().getDamping();
      res_prec.config.use_minres = config().cg().getMinres();


      res_unprec.config.iterations = config().cg().getIterations();
      res_unprec.config.precision = config().cg().getError();
      res_unprec.config.damping = config().cg().getDamping();
      res_unprec.config.use_minres = config().cg().getMinres();
      

      // // small schur
      // auto res_prec = make_cg( linear, cycles, prec_small );
      // auto res_unprec = make_cg( linear, cycles);
      
      core::log("preconditioned");
      impl = res_prec;
      correct();
      
      script::exec("cg.set.title = [[\"]]..linear.graph_name[%%]..[[\"]]", config().graph().getType());
      script::exec("cg_prec = cg; cg_prec.plot['1:2'].title = '\"preconditioned\"' ");
      
      core::log("normal");
      impl = res_unprec;
      correct();
      
      script::exec("cg.set.title = [[\"]]..linear.graph_name[%%]..[[\"]]", config().graph().getType());
      script::exec("cg_unprec = cg; cg_unprec.plot['1:2'].title = '\"normal\"' ");
    }

    auto res = make_cg( std::move(linear), cycles );

    if(non_linear_error) {

      math::real error = 1e42;
      math::natural it = 0;

      script::require("plot");
      script::exec("non_linear = plot.new(); non_linear.plot['1:2'] = { with = 'lines'}; \
                    non_linear.set.logscale = 'y'");
      
      res.early_stop = [&, error, it] (const phys::dof::velocity& v) mutable {

	math::real new_error = non_linear_error( v );
	math::real relative = std::abs(new_error - error) / new_error;
	const bool res = relative < 1e-3;
	
	// core::log("error:", error, "new_error:", new_error, "relative:", relative, "result:", res);

	// script::async("non_linear.data[%%] = {%%}", it, new_error );
	
	if( res ) { 
	  // script::async("if not non_linear.set.x2tics then non_linear.set.x2tics = '(%%)' end", it);
	}
	
	error = new_error;
	++it;
	return res;
      };


    }

    if( condition ) {
      compliance<constraint::bilateral> K(res.solver, cycles);
      math::real cond = math::cond_symm( K.dense );
      script::exec("log.now().cond = %%", cond );
    }

    
    res.config.iterations = config().cg().getIterations();
    res.config.precision = config().cg().getError();
    res.config.damping = config().cg().getDamping();

    impl = std::move(res);
    
  } else {
    using namespace phys::solver;

    // if( condition ) {
    //   compliance<constraint::bilateral> K(simple(system), system.constraint.bilateral.matrix);
    //   math::real cond = math::cond_symm( K.dense );

    //   script::exec("log.now().cond = %%", cond );

    //   math::mat test;
    //   math::natural n = system.mass.rows().dim();
    //   math::natural m = system.constraint.bilateral.matrix.rows().dim();

    //   test.resize( m + n, m + n );
    //   test << math::mat::Identity(n , n), K.dense_matrix.transpose(),
    // 	K.dense_matrix, math::mat::Zero(m, m);

    //   script::exec("log.now().cond_full = %%", math::cond_symm(test) );
    // }
    
    switch( config().graph().getType() ) {

    case Graph::CG: {

      auto res = phys::solver::make_cg(phys::solver::simple(system), 
				       system.constraint.bilateral.matrix);
      
      res.config.iterations = config().cg().getIterations();
      res.config.precision = config().cg().getError();
      res.config.damping = config().cg().getDamping();
      
      in_tree = 0;

      impl = std::move(res);
    
 

    } break;
    case Graph::NONE: {

      auto res =  phys::solver::cholesky< phys::solver::simple >(phys::solver::simple(system), 
								 system.constraint.bilateral.matrix,
								 config().cg().getDamping());
      
      in_tree = 0;

      impl = std::move(res);
    } break;

    case Graph::PGS : {

      auto res = phys::solver::make_gs(phys::solver::simple(system), system.constraint.bilateral.matrix);
   
      in_tree = 0;
      res.damping = config().cg().getDamping();
      res.iter.bound = config().cg().getIterations();
      res.iter.epsilon = config().cg().getError();
      res.omega = 1.2;    
      
      impl = std::move(res);
      
    } break;
      
    }

      
  
  }

  // log graph type
  script::exec("log.now().graph = {type = %%, time = %%}", 
	       config().graph().getType(),
	       none ? 0 : timer.start() );
  
  // impl = cholesky<simple>(simple(system) , system.constraint.bilateral.matrix );
}


Solver::Solver() { 

  script::require("log");
}


Solver::~Solver() { }

// void Solver::clearBilateralVector() {
//   system.constraint.bilateral.values = phys::constraint::bilateral::vector();
// }

// void Solver::clearRHS() {
//   rhs = phys::dof::vector();
// }
