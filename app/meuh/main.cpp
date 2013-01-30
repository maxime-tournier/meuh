
#include <sparse/vector.h>
#include "solver.h"

int main(int, char**) {

  using namespace std;

  cout << "hi, imma create a solver yo" << endl;

  Solver solver;
  int i = solver.addDof(3);
  
  solver.config().cg().setDamping( 1.0 );

  cout << "imma exit now yo yo" << endl;
}
