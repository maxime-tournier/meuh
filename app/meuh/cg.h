#ifndef CG_H
#define CG_H

#include <core/dll.h>

class DLL CG {
  double error, damping;
  int iterations;
  bool prec;
  bool minres;
public:
  
  CG();
  
  double getError() const;
  void setError(double);

  double getDamping() const;
  void setDamping(double );

  int getIterations() const;
  void setIterations(int);
 
  bool getPrec() const;
  void setPrec(bool);

  bool getMinres() const;
  void setMinres(bool);


};


#endif
