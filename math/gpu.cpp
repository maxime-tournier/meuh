#define VIENNACL_HAVE_EIGEN

#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET

#include "gpu.h"

#include <math/vec.h>

#include <viennacl/linalg/cg.hpp>
#include <viennacl/linalg/gmres.hpp>


namespace math {

  gpu::gpu() { }
  gpu::gpu(const mat& A)  { compute(A); }

  void gpu::compute(const mat& AA) { A = AA.cast<float>(); }

  vec gpu::solve(const vec& b) const {
    const Eigen::MatrixXf& AA = A;
    const Eigen::VectorXf bb = b.cast<float>();
    
    return viennacl::linalg::solve(AA, bb, viennacl::linalg::gmres_tag() ).cast<double>();
  }


}
