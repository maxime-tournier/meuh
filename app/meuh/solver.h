#ifndef SOLVER_H
#define SOLVER_H


#include <phys/solver/any.h>
#include <vector>

#include <sparse/matrix.h>
#include <phys/constraint/bilateral.h>
#include <phys/system.h>

#include "vector.h"
#include "matrix.h"
#include "config.h"


#include <core/dll.h>

class DLL Solver {

protected:
  typedef std::vector< phys::dof::key > dofs_type;
  dofs_type dofs;
  
  typedef std::vector< phys::constraint::bilateral::key > bilateral_type;
  bilateral_type bilateral;

  phys::dof::velocity velocity, delta;
  phys::dof::momentum momentum;

  phys::constraint::bilateral::vector lambda, mu;

  // system description
  phys::system system;

  // solver implementation
  phys::solver::any impl;
  
  static phys::dof* make_dof(unsigned int);
  static phys::constraint::bilateral* make_bilateral(unsigned int);

  std::function< bool (phys::constraint::bilateral::key) > in_tree;

  Config configuration;

public:

  Solver();
  ~Solver();

  // configuration 
  Config& config();
   
  // adds a dof with dimension @dim to the system, returns its handle
  int addDof(int dim);
  
  // adds a bilateral constraint with dimension @dim to the system,
  // returns its handle
  int addBilateral(int dim);

  // sets (diagonal) mass for dof @i
  void setMass(int i, double m);
  // TODO provide full mass matrix access
  
  // access to the momentum vector for dof @i, set to zero on first
  // access
  Vector getMomentum(int i);

  // access to the velocity vector for dof @i, set to zero on first
  // access
  Vector getVelocity(int i);

  // access to the correction vector for dof @i, set to zero on first
  // access
  Vector getDelta(int i);
  
  // access to the bilateral constraint matrix block for constraint @c
  // and dof @d, set to zero on first access
  Matrix getBilateralMatrix(int c, int d);

  // access to the bilateral constraint value vector for constraint
  // @c, zero on first call
  Vector getBilateralValues(int c);

  // access to the bilateral constraint value vector for constraint
  // @c, zero on first call
  Vector getBilateralCorrections(int c);
  
  // computes necessary stuff before solve/correct. only depend on
  // system geometry (mass/constraint matrices)
  void factor(bool fast = false);
  
  // solves for the system response: M.v = f + J^T.lambda st J.v = b
  // needs factor() first.
  void solve();
  
  // computes position corrections: M.v = J^T.\lambda st J.v = c
  // needs factor() first.
  void correct();
  
  // clears everything (mass/constraint matrices, velocity/momentum)
  void clear();
  
  // returns whether a constraint is in the acyclic tree
  bool inTree(int) const;


  // HACK
  std::function< math::real ( const phys::dof::velocity& ) > non_linear_error;
};

#endif
