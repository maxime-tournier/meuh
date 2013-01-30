#include "bilateral.h"

#include <phys/solver/linear_time.h>
#include <phys/solver/brutal.h>
// #include <phys/solver/si.h>
#include <phys/solver/simple.h>
#include <phys/solver/partition.h>
#include <phys/solver/tree.h>
#include <phys/solver/cg.h>
#include <phys/solver/gs.h>
#include <phys/solver/any.h>
#include <phys/solver/ref.h>

#include <sparse/keep.h>
#include <phys/dof.h>

namespace phys {
  namespace solver {

    struct bilateral::pimpl_type {
      

      const dof::mass& mass;
      const constraint::bilateral::pack pack;
      const solver::simple unconstrained;
      solver::graph::constraints graph;
      tree::analysis analysis;
      solver::any fast;
      
      typedef solver::partition<reference<any>, simple> primary_type;
      primary_type primary;
      
      constraint::bilateral::matrix secondary_matrix;
      
      typedef solver::brutal< primary_type > secondary_type;
      // typedef solver::si< primary_type > secondary_type;
      // typedef solver::cg<primary_type> secondary_type;
      // typedef solver::gs<primary_type> secondary_type;

      std::unique_ptr<secondary_type> secondary;
      
      void update() {
      	// update the acyclic tree TODO from the graph
      	fast.as<linear_time>().update();
	
      	secondary_matrix = sparse::keep( pack.matrix, analysis.isolated.constraints );
	
      	// update the cycle graph
      	secondary.reset( new secondary_type(constraint::bilateral::pack{secondary_matrix, pack.values}, 
      					    primary, 0) );
      }
      
      void span(const solver::graph::constraints& graph) {

	 analysis = solver::tree::span(graph);
	 fast = solver::linear_time(mass, analysis.tree, pack);
	 
	 secondary_matrix = sparse::keep( pack.matrix, analysis.isolated.constraints );
	 secondary.reset( new secondary_type(constraint::bilateral::pack{secondary_matrix, pack.values}, 
					     primary, 0) );
       }
     
 		   

      pimpl_type(const dof::mass& masses,
		 const dof::mass& masses_inv,
		 const constraint::bilateral::pack& pack) 
	: mass(masses),
	  pack(pack), 
	  unconstrained( masses, masses_inv ),
	  graph(solver::graph::make(pack.matrix)),
	  analysis( tree::make(graph) ),
	  fast( linear_time(masses, analysis.tree, pack) ),
	  primary( graph::dofs( analysis.tree ), ref(fast), unconstrained ),
	  secondary_matrix( sparse::keep( pack.matrix, analysis.isolated.constraints ) ),
	  secondary( new secondary_type(constraint::bilateral::pack{secondary_matrix, pack.values}, primary, 0)) {
      }

      dof::velocity operator()(const dof::momentum& f, 
				 const result& res) const {
	assert( sparse::linear::consistent(f) );
	
	// return (primary)(f, res);
	return (*secondary)(f, res);
      }

    };


    // const linear_time& bilateral::acyclic() const { return pimpl->fast.as<linear_time>(); }
    linear_time& bilateral::acyclic() { return pimpl->fast.as<linear_time>(); }
    
    const solver::graph::constraints& bilateral::graph() const { return pimpl->graph; }
    solver::graph::constraints& bilateral::graph() { return pimpl->graph; }
    

    bilateral::bilateral(const dof::mass& masses,
			 const dof::mass& masses_inv,
			 const constraint::bilateral::pack& pack) 
      : pimpl( new pimpl_type(masses, masses_inv, pack) ) {
      
    };


    bilateral::bilateral(const phys::system& system) :
      pimpl( new pimpl_type(system.mass, system.resp, system.constraint.bilateral.pack()) ) {
      assert( system.mass.size() == system.resp.size() );
    }

    bilateral::~bilateral() { }
  

    dof::velocity bilateral::operator()(const dof::momentum& f, const result& res ) const {
      return (*pimpl)( f, res );
    }


    void bilateral::update() {
      pimpl->update();
    }

 
    void bilateral::span(const graph::constraints& g) {
      pimpl->span(g);
    }
  }
}

