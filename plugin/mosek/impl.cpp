#include <plugin/mosek.h>

#include <math/vec.h>
#include <math/mat.h>
#include <core/stl.h>
#include <core/macro/here.h>

#include <core/log.h>
#include <core/use.h>

#include <core/debug.h>

#define MOSEK_HEADER  <MOSEK_INCLUDE/mosek.h>
#include MOSEK_HEADER

namespace plugin {
  namespace impl {
 
    namespace mosek {

      using namespace math;

      static_assert( std::is_same<MSKrealt, real>::value, "bad real type !" );
 

      // computing environment
      struct environment {
	MSKenv_t handle;
      
	environment();
	~environment();
      
	void debug(bool value) const;
      
	static void MSKAPI print(void *handle, char str[]);
    
      };
      
 
      static const environment env;
      
      void MSKAPI environment::print(void *handle, char str[]) {
	core::use(handle);
	std::cout <<  str; // printf("%s",str);
      } 
    
  
      environment::environment() { 
	MSK_makeenv(&handle,NULL,NULL,NULL,NULL);
	MSK_initenv(handle);

	debug( true );

      }

      environment::~environment() { 
	MSKrescodee r = MSK_deleteenv(&handle);
	core::use(r);
      }
  
  
      void environment::debug(bool value) const {
	if( value ) {
	  MSK_linkfunctoenvstream(handle, MSK_STREAM_LOG, NULL, environment::print);
	} else {
	  MSK_linkfunctoenvstream(handle, MSK_STREAM_LOG, NULL, 0);
	}
      }
    
  




      class mosek : public plugin::mosek {
	MSKtask_t    _task;
      public:
      
	mosek() : _task(0) { }
	virtual ~mosek();

	// maketask with estimate
	void make(natural var, natural con) { 
	  MSK_maketask(env.handle, con, var, &_task); 
	}

	// appends var/constraints
	void add(MSKaccmodee mode, natural n) {
	  MSK_append(task(), mode, n);
	}

	const MSKtask_t& task() const {
	  assert( _task );
	  return _task;
	}
    
      
	natural num_variables() const {
	  MSKintt res;
	  MSK_getnumvar(task(), &res);
	  return res;
	}
      
	natural num_constraints() const {
	  MSKintt res;
	  MSK_getnumcon(task(), &res);
	  return res;
	}


      


	// declare variables/constraints
	mosek& init(natural var, natural con) {
	  make(var, con);
	
	  // non zero estimate
	  MSK_putmaxnumanz(task(), var * con);

	  add( variable(0, var) );
	  add( constraint(0, con) );

	  core::log(HERE, "mosek debug enabled");
	  debug( true );

	  return *this;
	}
    
	// append (free) variables/constraints
	mosek& add(const variable& var) {
	  assert(var.start == variables().end );
	  if(!var.dim()) return *this;

	  // add variables: the variables will initially be fixed at zero
	  add(MSK_ACC_VAR, var.dim() );
    
	  // free variables
	  bounds( var );

	  return *this;

	}
      
	mosek& add(const constraint& con) {
	  assert(con.start == constraints().end );
	  if(!con.dim()) return *this;
	
	  // add constraints: the constraints will initially have no bounds
	  add(MSK_ACC_CON, con.dim());
    
	  return *this;
	}
    
      
	mosek& clear() {
	  MSK_deletetask(&_task);
	  return *this;
	}

      

	// linear term in objective
	void c(const variable& v, const vec& data) {
	  assert( data.rows() == int( v.dim() ) );
      
	  data.each([&](natural j ) {
	      MSK_putcj(task(), v.start + j, data(j));
	    });
	}

      
    
	// quadratic matrix in objective
	void Q(const variable& v, const mat& data) { // diagonal
	  assert(data.rows() == int( v.dim() ) );
	  assert(data.cols() == int( v.dim() ) );
    
	  data.each_row([&](natural i){
	      for(natural j = 0; j <= i; ++j) {
		MSK_putqobjij(task(), v.start + i, v.start + j, data(i, j));
	      }
	    });
    
  
	}
      
      
	void Q(const variable& vi, const variable& vj, const mat& data) { // (lower) off-diagonal
	  assert(data.rows() == int( vi.dim() ) );
	  assert(data.cols() == int( vj.dim() ) );
	  assert( vj <= vi );

	  data.each([&](natural i, natural j) {
	      assert( vj.start + j <= vi.start + i );
	      MSK_putqobjij(task(), vi.start + i, vj.start + j, data(i, j));
	    });
    
	}

      
	void add_Q(const variable& v, const mat& data) { // diagonal
	  assert(data.rows() == int( v.dim() ) );
	  assert(data.cols() == int( v.dim() ) );

	  data.each_row([&](natural i){
	      for(natural j = 0; j <= i; ++j) {
		double old = 0;
		MSK_getqobjij(task(), v.start + i, v.start + j, &old);
		MSK_putqobjij(task(), v.start + i, v.start + j, old + data(i, j));
	      }
	    });
	}

      
	void add_Q(const variable& vi, const variable& vj, const mat& data) { // (lower) off-diagonal
	  assert(data.rows() == int( vi.dim() ) );
	  assert(data.cols() == int( vj.dim() ) );
	  assert( vj <= vi );

	  data.each_row([&](natural i){
	      for(natural j = 0; j <= i; ++j) {
		double old = 0;
		MSK_getqobjij(task(), vi.start + i, vj.start + j, &old);
		MSK_putqobjij(task(), vi.start + i, vj.start + j, old + data(i, j));
	      }
	    });
	}

      
	// quadratic constraint
	void K(const constraint& c, const variable& v, const mat& data) {
	  assert(c.dim() == 1);
	  assert(data.rows() == data.cols());
	  assert( data.rows() == int(v.dim()));
    
	  data.each([&](natural i, natural j) {
	      if( j <= i ) {
		MSKidxt row = v.start + i;
		MSKidxt col = v.start + j;
		MSKrealt val = data(i, j);
	  
		MSK_putqconk(task(), c.start, 1, &row, &col, &val); 
	  
	      };
	    });    
	}

      
	void K(const constraint& c, const variable& vi, const variable& vj, const mat& data) {
	  assert(c.dim() == 1);
	  assert( data.rows() == int(vi.dim()));
	  assert( data.cols() == int(vj.dim()));
	  assert( vj < vi );
    
	  data.each([&](natural i, natural j) {
	      MSKidxt row = vi.start + i;
	      MSKidxt col = vj.start + j;
	      MSKrealt val = data(i, j);
	
	      MSK_putqconk(task(), c.start, 1, &row, &col, &val); 
	  
	    });
	}


	// linear constraint matrix
	void A(const constraint& ci, const variable& vj, const mat& data) {
	  assert( data.rows() == ci.dim() );
	  assert( data.cols() == vj.dim() );
      
	  storage tmp = storage::plain(data, ci.start, vj.start );
	  MSK_putaijlist( task(), tmp.size(), &(tmp.row[0]), &(tmp.col[0]), &(tmp.data[0]) );
	}
      
	static MSKboundkeye convert(mosek::bound b) {
	  switch(b) {
	  case mosek::bound::lower: return MSK_BK_LO;
	  case mosek::bound::upper: return MSK_BK_UP;
	  case mosek::bound::fixed: return MSK_BK_FX;
	  };
	  throw math::error("invalid bound");
	}

      
	// rhs for constraints
	void b(const constraint& c, const vec& data, mosek::bound b) {
	  assert( data.rows() == c.dim() );

	  data.each([&](natural i ) {
	  
	      const real low = data(i);
	      const real up = (b == bound::lower) ? +MSK_INFINITY : low;
	  
	      MSK_putbound(task(),
			   MSK_ACC_CON, /* Put bounds on constraints.*/
			   c.start + i,           /* Index of constraint.*/
			   convert(b),      /* Bound key.*/
			   low,      /* Numerical value of lower bound.*/
			   up);     /* Numerical value of upper bound.*/
	    });

	}

      
	void b(const constraint& c, const vec& lower, const vec& upper) {
	  assert( lower.rows() == c.dim() );
	  assert( upper.rows() == c.dim() );
      
	  lower.each([&](natural i ) {
	  
	      MSK_putbound(task(),
			   MSK_ACC_CON, /* Put bounds on constraints.*/
			   c.start + i,           /* Index of constraint.*/
			   MSK_BK_RA,      /* Bound key.*/
			   lower(i),      /* Numerical value of lower bound.*/
			   upper(i));     /* Numerical value of upper bound.*/
	    });

	}
      
	void b(const constraint& c ) { // disables constraint
	  for(natural i = c.start; i < c.end; ++i) {
	    MSK_putbound(task(),
			 MSK_ACC_CON, /* Put bounds on constraints.*/
			 i,           /* Index of constraint.*/
			 MSK_BK_FR,      /* Bound key.*/
			 -MSK_INFINITY,      /* Numerical value of lower bound.*/
			 +MSK_INFINITY);     /* Numerical value of upper bound.*/
	  }      
	}

      

	// bounds on variables
	void bounds(const variable& v, const vec& data, mosek::bound b ){
	  assert( data.rows() == int( v.dim() ) );
      
	  data.each( [&](natural i) {

	      const real low = data(i);
	      const real up = (b == bound::lower) ? +MSK_INFINITY : low;
	  
	      MSK_putbound(task(),
			   MSK_ACC_VAR, /* Put bounds on variable.*/
			   v.start + i,           /* Index of variable.*/
			   convert(b),      /* Bound key.*/
			   low,      /* Numerical value of lower bound.*/
			   up);     /* Numerical value of upper bound.*/
	  
	    });

	}

      
	void bounds(const variable& v, const vec& lower, const vec& upper  ) {
	  assert( lower.rows() == int( v.dim() ) );
	  assert( upper.rows() == int( v.dim() ) );
      
	  lower.each( [&](natural i) {
	
	      MSK_putbound(task(),
			   MSK_ACC_VAR, /* Put bounds on variable.*/
			   v.start + i,           /* Index of variable.*/
			   MSK_BK_RA,      /* Bound key.*/
			   lower(i),      /* Numerical value of lower bound.*/
			   upper(i));     /* Numerical value of upper bound.*/
	  
	    });

	}

      
	void bounds(const variable& v ){
	    
	  for( natural i = v.start; i < v.end; ++i) {
	    MSK_putbound(task(),
			 MSK_ACC_VAR, /* Put bounds on variable.*/
			 i,           /* Index of variable.*/
			 MSK_BK_FR,      /* Bound key.*/
			 -MSK_INFINITY,      /* Numerical value of lower bound.*/
			 +MSK_INFINITY);     /* Numerical value of upper bound.*/
	 
	  }

	}
      
      
	// solves and returns part of the solution
	vec solve(const variable& v) throw(math::mosek::exception) {
	
	  // storage for solution
	  vec storage = vec::Zero( v.dim() );

	  MSKrescodee trmcode;
    
	  // optimize 
	  MSKrescodee r = MSK_optimizetrm(task(), &trmcode);
    
	  // debug
	  MSK_solutionsummary (task(),MSK_STREAM_LOG); 
    
	  if ( r==MSK_RES_OK ) {
	    MSKsolstae solsta;
	    // int j;
      
	    MSK_getsolutionstatus (task(), MSK_SOL_ITR, NULL, &solsta);
      	  
	    switch(solsta) {
	    case MSK_SOL_STA_OPTIMAL:   
	    case MSK_SOL_STA_NEAR_OPTIMAL:
	      MSK_getsolutionslice(task(),
				   MSK_SOL_ITR,    /* Request the interior solution. */
				   MSK_SOL_ITEM_XX,/* Which part of solution.     */
				   v.start,              /* Index of first variable.    */
				   v.end,         /* Index of last variable+1.   */
				   storage.data());
	      return storage;
	    case MSK_SOL_STA_DUAL_INFEAS_CER:
	    case MSK_SOL_STA_PRIM_INFEAS_CER:
	    case MSK_SOL_STA_NEAR_DUAL_INFEAS_CER:
	    case MSK_SOL_STA_NEAR_PRIM_INFEAS_CER:  
	      {
		core::log("Primal or dual infeasibility certificate found.");
		infeasible e; e.result = std::move(storage);
		throw e;
	      }
	
	    case MSK_SOL_STA_UNKNOWN:
	      {
		core::log("The status of the solution could not be determined");
		unknown_status e; e.result = std::move(storage);
		throw e;
	      }
	    default:
	      core::log("Other solution status.");
	      break;
	    }
	  }
	  else
	    {
	      core::log("Error while optimizing");
	      return storage;
	    }


	  return storage;
	  std::abort();

	}
      
	void debug( bool value) {
	
	  if( value ) {
	    MSK_linkfunctotaskstream(task(),MSK_STREAM_LOG,NULL, environment::print);
	  } else {
	    MSK_linkfunctotaskstream(task(),MSK_STREAM_LOG,NULL, 0);
	  }
	}
      
	void write(const std::string& filename) const {
	    MSK_writedata ( task(), const_cast<char*>( filename.c_str() ) );
	}
    
	variable variables() const {
	  variable res;
	  res.end = num_variables(); 
	  return res;
	}
    
 
	constraint constraints() const {
	  constraint res;
	  res.end = num_constraints();
	  return res;
	}


	mosek& relax() {
	
	  MSKtask_t relaxed = 0;
    
	  vec 
	    wlc = vec::Ones( num_constraints() ),
	    wuc = vec::Ones( num_constraints() ),
	    wlx = vec::Ones( num_variables() ),
	    wux = vec::Ones( num_variables() );
    
	  MSK_putintparam ( task() , MSK_IPAR_FEASREPAIR_OPTIMIZE , MSK_FEASREPAIR_OPTIMIZE_PENALTY );
    
	  // compute closest bounds for constraints
	  MSK_relaxprimal(task(), &relaxed, 
			  wlc.data(), wuc.data(),
			  wlx.data(), wux.data());
    
	  // set new feasible bounds on variables/constraints
	  bounds( variables(), wlx, wux);
	  b( constraints(), wlc, wuc );
    
	  return *this;

	}

	mosek& read(const std::string& filename) {
	  throw core::error("broken");
	
	  MSKtask_t task;

	  MSK_makeemptytask(env.handle, &task);
	  MSK_readdata ( task, const_cast<char*>(filename.c_str()));
    
	  // impl.reset(new impl_type ); 
    
	  // TODO set task ?! FIXME !
    
	  return *this;

	}


	struct storage {
    
	  std::vector< MSKidxt > row, col;
	  std::vector< MSKrealt > data;
    
	  natural size() const { assert( row.size() == col.size() && row.size() == data.size() ); return row.size(); }
    
	  storage(math::natural size) : row(size), col(size), data(size) { }

	  static storage symmetric(const mat& data, natural ioff = 0, natural joff = 0) {
	    assert( data.rows() == data.cols() );
    
       
	    const natural n = data.rows();
	    const natural size = (n * (n + 1) ) / 2;

	    storage res(size);

	    natural off = 0;

	    // TODO cache miss => traverse by columns instead
	    for(natural i = 0; i < n; ++i ) {
	      for(natural j = 0; j <= i; ++j) {
	
		res.row[ off ] = ioff + i;
		res.col[ off ] = joff + j;
	   
		res.data[ off ] = data(i, j);

		++off;
	      }
	    }

	    assert( off == size );
       
	    return res;
	  }

	  static storage plain(const mat& data, natural ioff = 0, natural joff = 0) {
      
	    const natural m = data.rows();
	    const natural n = data.cols();
      
	    const natural size = data.rows() * data.cols();
      
	    storage res(size);

	    natural off = 0;

	    // TODO cache miss => traverse by columns instead
	    for(natural i = 0; i < m; ++i ) {
	      for(natural j = 0; j < n; ++j) {
	  
		res.row[ off ] = ioff + i;
		res.col[ off ] = joff + j;
	  
		res.data[ off ] = data(i, j);
	  
		++off;
	      }
	    }

	    assert( off == size );
       
	    return res;
	  }

	  void reserve(natural n) {
	    row.reserve(n);
	    col.reserve(n);
	    data.reserve(n);
	  };
    
	  void append(const storage& other) {
	    reserve( size() + other.size() );

	    row.insert(row.end(), other.row.begin(), other.row.end());
	    col.insert(col.end(), other.col.begin(), other.col.end());
	    data.insert(data.end(), other.data.begin(), other.data.end());
      
	  }

	};











	typedef std::map< variable, std::map< variable, math::mat > > sparse_qobj;
	void Q(const sparse_qobj& data) {
	  storage tmp(0);

	  core::each( data, [&](variable i, const std::map<variable, mat>& row) {
	      core::each( row, [&](variable j, const mat& chunk) {
	    
		  if( i == j) {
		    tmp.append( storage::symmetric(chunk, i.start, j.start) ); 
		  } else {
		    tmp.append( storage::plain(chunk, i.start, j.start) );
		  }
		});
	    });

	  MSK_putqobj( task(), tmp.size(), &(tmp.row[0]), &(tmp.col[0]), &(tmp.data[0]) );

	}



      };

      mosek::~mosek() { }
      
      static const plugin::loader<plugin::mosek> loader( [] { return new mosek; } );
      
    }
  }
}

