#include "cg.h"

#include <cassert>


CG::CG() : error(1e-8), 
	   damping( 0 ), 
	   iterations(10),
	   prec( false ),
	   minres(false)
{
}

double CG::getError() const { return error; }
void CG::setError(double e)  { 
  assert( e >= 0 );
  error = e; 
}

double CG::getDamping() const { return damping; }
void CG::setDamping(double e)  { 
  assert( e >= 0 );
  damping = e; 
}

int CG::getIterations() const { return iterations; }
void CG::setIterations(int e)  { 
  assert( e >= 0 );
  iterations = e; 
}

bool CG::getPrec() const { return prec; }
void CG::setPrec(bool value) { prec = value; }


bool CG::getMinres() const { return minres; }
void CG::setMinres(bool value) { minres = value; }
