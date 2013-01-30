#include <math/types.h>
#include <boost/program_options.hpp>

#include <tool/mocap.h>
#include <vector>

#include <math/func/compose.h>
#include <math/func/SE3.h>
#include <math/func/lie/translation.h>
#include <math/func/vector.h>
#include <math/lie/pga.h>
#include <math/lie/dist.h>

#include <phys/rigid/relative.h>
#include <phys/rigid/absolute.h>

#include <core/parallel.h>

#include <mutex>

#include <script/console.h>
#include <script/engine.h>
#include <script/command.h>
#include <script/bundle.h>
#include <core/share.h>

using namespace math;

struct result {
  natural m,n;

  // squarenorm
  math::real pga, euler;
  math::real rms(math::real x) const { return std::sqrt( x / (m * n) ); }
  
  std::string tostring() const {
    return script::cmd('{', " m = ", m, ", n = ", n, ", pga = ", pga, ", euler = ", euler, "}");
  }
  
};



static vector< vector<SO3> > reconstruct(const lie::pga<vector<SO3> >& pga, natural n) {

  vector< vector<SO3> > res;
  res.resize( pga.coords().rows() );
  
  res.each( [&](natural i) {
      res(i) = pga.ccfk(n)( pga.coords().row(i).head(n).transpose() );
    });
  
  return res;
}




static vector<SO3> euler2rotation(const vector<vec3>& angles, const char* order ) {
  vector<SO3> res;
  res.resize( angles.rows() );

  res.each([&](natural j) {
      res(j) = SO3::euler(angles(j), order);
    });
  
  return res;
}

static vector<vec3> rotation2euler( const vector<SO3>& rots) {
  vector<vec3> res;
  res.resize( rots.rows() );

  res.each([&](natural j) {
      res(j) = rots(j).zyx();
    });
  
  return res;
}


static vector< vector<SO3> > reconstruct(const lie::pga<vector<vec3> >& pga, natural n) {
  
  vector< vector<SO3> > res;
  res.resize( pga.coords().rows() );
  
  res.each( [&](natural i) {
      res(i) = euler2rotation(  pga.ccfk(n)( pga.coords().row(i).head(n).transpose() ),
				"zyx" );
      
    });
  
  return res;
}

template<class F>
static vector< typename F::range > apply(const F& f, const vector< typename F::domain >&data ){
  vector < typename F::range > res;
  res.resize( data.rows () );

  res.each( [&](natural i ) {
      res(i) = f(data(i));
    });
  
  return res;
}


static void show_errors(const vector<SO3>& x, const vector<SO3>& y) {

  for(int i = 0; i < x.rows(); ++i) {
    real err =  lie::log(x(i).inverse() * y(i)).squaredNorm();
    if( err > 0.1 ) std::cout << "at: " << i <<  ",  " << err << " ";
  }

  std::cout << std::endl;
}

static real height( const vector<vec3>& pos ) {

  real min = 1e10;
  real max = -1e10;
  
  pos.each([&](natural i ) {
      min = std::min(min, pos(i).y());
      max = std::max(max, pos(i).y());
    });
  
  return max - min;
}


int main(int argc, char** argv) {
  std::map<std::string, result> results;
  std::map<int, unsigned int> histogram;
  
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("variance,v", po::value<math::real>()->default_value(0.99), "target variance")
    ("number,n", po::value<int>(), "reconstruction modes (defaults to target variance)")
    ("offset,o", po::value<math::real>()->default_value(0), "offset in rotations")
    ("files,f", po::value<std::vector<std::string> >()->multitoken(), "filenames (required)");
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm); 

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  real variance = 0.99;
  int n_modes = -1;

  struct error {
    real pos;
    real orient;
  };
  
  error pga, euler;

  pga.pos = 0;
  pga.orient = 0;

  euler.pos = 0;
  euler.orient = 0;


  std::map<math::real, std::string> orient_diff, pos_diff;

  int n_samples = 0;
  
  if( vm.count("variance") ) variance = vm["variance"].as<real>();
  if( vm.count("number") ) n_modes = vm["number"].as<int>();
  
  
  real off_alpha = vm["offset"].as<real>();
  
  core::log("OFF_ALPHA = ", off_alpha);

  if( vm.count("files") ) {

    std::mutex mutex;

    const auto files = vm["files"].as<std::vector<std::string> >();
    natural left = files.size();    
    
  

    auto task = [&](natural start, natural end) {

      for(natural i = start; i < end; ++i) {
	const std::string& file = files[i];
	const tool::mocap pack(file);	

	auto root = pack.skeleton().root;
	
	result res;

	auto map = func::vector< func::rotation<> >() << pack.relative();
	
	vector< vector<SO3> > orient_data = pack.clip.sampling.sample( map );
	orient_data.each([&](natural i) { 
	    orient_data(i)(root) = SO3::identity();
	  });

	natural n_joints = pack.skeleton().size() - 1;
	
	using func::operator<<;
	
	auto pos_fun =  
	func::vector< func::translation<> >() 
	<< phys::rigid::absolute(pack.skeleton())
	<< func::vector< func::pure_rotation<> >();
	
	// << mocap::global(pack.clip.skeleton.get()) 
	// << func::lie::L( mocap::tee_pose(  pack.clip.skeleton.get() ) )
	// << func::vector< func::pure_rotation<> >();
	
	res.m = orient_data.rows();
	res.n = n_joints;

	vector<vector<vec3> > pos_data = apply(pos_fun, orient_data);

	auto h = height(pos_data(0));

	core::log("character height:", h);

	// suppose our character measures 1.75 meters
	real meter = 1.75 / h;
	int samples = (orient_data.rows() * n_joints );

	core::log("samples:", samples);
	
	real odiff = 0, pdiff =0;

	SO3 off = Lie<SO3>().exp()(off_alpha * vec3::UnitX());
	
	vector<SO3> offset = vector<SO3>::Constant( orient_data(0).rows(), off );
	offset(root) = SO3::identity();
	vector< vector< SO3> > offsets = vector< vector< SO3> >::Constant(orient_data.rows(), offset);
	
	// PGA
	auto fun_pga = [&] {
	  using func::operator<<;
	  vector<vector<SO3> > data = lie::prod(offsets, orient_data);
	  
	  error& zob = pga;

	  lie::pga< vector<SO3> > pga;
	  pga.compute(data, algo::stop().eps(1e-7), lie::of( data(0) ), variance );

	  natural n_reconst = n_modes > 0 ? std::min<int>(data.rows(), n_modes) : res.pga;

	  // orientation error
	  const vector<vector<SO3> > approx = lie::prod(lie::inv(offsets), reconstruct(pga, n_reconst));
	  
	  real orient_error = lie::dist2(orient_data, approx);
	  res.pga = orient_error;
	  
	  mutex.lock();
	  zob.orient += orient_error;
	  mutex.unlock();
	  
	  real orient_rms = std::sqrt( orient_error  /  samples);
	  
	  core::log("pga orientation error:", orient_rms / math::deg, "deg" );

	  real pos_error =  lie::dist2(pos_data, apply(pos_fun, approx) );

	  mutex.lock();
	  zob.pos += pos_error;
	  mutex.unlock();

	  real pos_rms = std::sqrt( pos_error / samples );
	  core::log("pga position error:", pos_rms * meter, "meters" );

	  odiff = orient_rms;
	  pdiff = pos_rms;
	};


	// Euler
	auto fun_euler =[&] {
	  
	  
	  vector< vector <vec3> > data;
	  data.resize( pack.clip.sampling.frames );
	  data.each([&](natural i) {
	      vector< vec3 > alter = rotation2euler(func::lie::L(offset)(orient_data(i)));
	      data(i) = alter;
	    });      
	    
	  lie::pga< vector<vec3> > pga;
	  
	  pga.compute(data, algo::stop().eps(1e-9), lie::of( data(0) ), variance );

	  natural n_reconst = n_modes > 0 ? std::min<int>(data.rows(), n_modes) : res.euler;

	  vector<vector<SO3> > approx = lie::prod( lie::inv(offsets), reconstruct(pga, n_reconst)); 
	  
	  real orient_error = lie::dist2(orient_data, approx);
	  res.euler = orient_error;
	  
	  mutex.lock();
	  euler.orient += orient_error;
	  mutex.unlock();

	  real orient_rms = std::sqrt( orient_error / samples );

	  if( orient_rms / math::deg > 10 ) {
	    core::log("\tsuspicious stuff !!");
	  }

	  core::log("euler orientation error:", orient_rms / math::deg, "deg" );

	  real pos_error =  lie::dist2(pos_data, apply(pos_fun, approx) );

	  mutex.lock();
	  euler.pos += pos_error;
	  mutex.unlock();

	  real pos_rms = std::sqrt( pos_error / samples );
	  core::log("euler position error:", pos_rms * meter, "meters" );
	 
	  odiff -= orient_rms;
	  pdiff -= pos_rms;
	};

	fun_pga();
	fun_euler();
	
	// >0 when fewer modes are needed for pga
	real score = res.euler - res.pga;
	int progress = (100 * (files.size() - left)) / files.size();
	
	mutex.lock(); 
	results[file] = res;
	
	histogram[score]++;
	--left;
	
	n_samples += samples;

	core::log(file, 
		  "\tpga:\t", res.rms(res.pga), 
		  "\teuler:\t",  res.rms(res.euler), 
		  "\tscore:\t", score, 
		  "\t", progress, "%");

	orient_diff [ odiff ] = file;
	pos_diff [ pdiff ] = file;

	mutex.unlock();
	
      }
    };
    
    core::parallel(0, files.size(), task);
    // task(0, files.size());
    
    core::each(histogram, [&](int diff, natural n) {
	std::cout << diff << ": " << n << ", ";
      });

    std::cout << std::endl;

    // 
    core::log("total samples", n_samples);
    core::log("mean orient error, pga:\t", std::sqrt(pga.orient / n_samples ) / math::deg);
    core::log("mean orient error, euler:\t", std::sqrt(euler.orient / n_samples )/ math::deg);

    core::log("mean pos error, pga:\t", std::sqrt(pga.pos / n_samples ));
    core::log("mean pos error, euler:\t", std::sqrt(euler.pos / n_samples ));
    
    core::log("best orient diff:", orient_diff.begin()->first, orient_diff.begin()->second );
    core::log("best pos diff:", pos_diff.begin()->first, pos_diff.begin()->second );

    script::engine engine;
    engine.start();
    engine.lua().file( core::share("/script/meuh.lua") );
    
    auto expose = [&](const std::string& name) {
      std::string cmd = name;

      cmd += " = ";
      cmd += "{ ";
      
      core::each(results, [&](const std::string& name, const ::result& res) {
	  cmd += "[\"";
	  cmd += name;
	  cmd += "\"] = ";

	  cmd += res.tostring();
	  cmd += ", ";
	});
      
      cmd += " } ";
      
      engine.exec( cmd );
      
    };

    
    script::bundle bundle(engine.lua(), "app");
    bundle.fun("modes", [&]() -> natural {  return n_modes; } );
    
    expose("result");
    engine.exec("require 'analysis'");
    engine.stop();
    // script::console console(engine);
    // console.start().join();
  } else {
    std::cout << desc << "\n";
    return 1;
  }
  

}
