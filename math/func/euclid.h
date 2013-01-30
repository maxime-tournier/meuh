#ifndef MATH_FUNC_EUCLID_H
#define MATH_FUNC_EUCLID_H

#include <math/euclid.h>
#include <math/func/tuple.h>

#include <type_traits>

#include <math/func/compose.h>
#include <math/func.h>

namespace math {

	namespace func {

		namespace euclid {

			template<class E>
			struct dot1;

			template<class E>
			struct scalar;

			template<class E>
			struct line;

			// TODO store euclid structure
			template<class E> 
			struct squared_norm {
      
				math::euclid::space<E> euclid;

				squared_norm(const math::euclid::space<E>& euclid = math::euclid::space<E>()) 
					: euclid(euclid)
				{
	  
				}

				typedef E domain;
				typedef typename math::euclid::space<E>::field range;
      
				range operator()(const domain& e ) const {
					return euclid.norm2( e );
				}
	
				struct push : base< dot1<E> > {
	  
					push(const squared_norm& of, const domain& at) 
						: push::base( of.euclid.scal(2, at),
						              of.euclid ) { 
	    
					}

				};
	
				struct pull : base< line<typename math::euclid::space<E>::dual> > {
					pull( const squared_norm& of, const domain& at) 
						: pull::base( math::euclid::pair( of.euclid.scal(2, at) ),
						              *of.euclid ) {
	    
					}
	  
				};
			};
          

			template<class E>
			struct pair {
	
				const typename math::euclid::space<E> euclid;

				typedef typename math::euclid::space<E>::dual dual;
				typedef std::tuple<dual, E> domain;
      	
				typedef typename math::euclid::space<E>::field range;
	
				range operator()(const domain& x) const {
					return euclid.pair( std::get<0>(x), std::get<1>(x) );
				}
	
			};

    

			template<class E>
			struct line {
				typedef typename math::euclid::space<E>::field domain;
				typedef E range;

				E dir;
				math::euclid::space<E> euclid;
	
				line(const E& dir, const math::euclid::space<E>& euclid = math::euclid::space<E>() ) 
					: dir(dir),
					  euclid(euclid) {

				}

				range operator()(const domain& x) const {
					return euclid.scal(x, dir);
				}

				struct push : base< line >  {
					push(const line& of, const domain& ) 
						: push::base(of.dir, of.euclid) { 

					}
				};

				struct pull : base< dot1< typename math::euclid::space<E>::dual > > {
	  
					pull(const line& of, const domain& ) 
						: pull::base( math::euclid::pair(of.dir),
						              *of.euclid )
					{
	    
					}

				};

			};

  
			template<class E>
			struct sum {
    
				typedef std::tuple<E, E> domain;
				typedef E range;
      
				range operator()(const domain& x) const { 
					return math::euclid::sum(std::get<0>(x), std::get<1>(x));
				}
	
				struct push : base< sum > {
	  
					push(const sum&, const domain& ) { } 
	  
				};

				struct pull : base< tuple_join< id< typename math::euclid::space<E>::dual >,
				                                id< typename math::euclid::space<E>::dual > > > {
					pull(const sum&, const domain& ) { }
				};
			};
      
      
			template<class E>
			struct minus {
	
				typedef E domain;
				typedef E range;
      
				range operator()(const domain& x) const { 
					return math::euclid::minus(x);
				}
    
				struct push : base< minus > {
					push( const minus&, const domain& ) { }
				};

				struct pull : base< minus< typename math::euclid::space<E>::dual > > {
					pull( const minus&, const domain& ) { }
				};

			};

			template<class E>
			struct scalar {
	
				typedef E domain;
				typedef E range;
      
				typedef typename math::euclid::space<E>::field field;
	
				field lambda;
				scalar(const field& x) : lambda(x) { }
      
				range operator()(const domain& x) const {
					return math::euclid::scal(lambda, x);
				}
	
				struct push : base< scalar > {
					push(const scalar& of, const domain& ) : push::base(of) {  }
				};

				struct pull : base< scalar< typename math::euclid::space<E>::dual > > {
					pull(const scalar& of, const domain& )
						: pull::base(of.lambda) { 
	  
					}
				};
			};

			template<class E>
			struct scalar_ref {
	 
				typedef E domain;
				typedef E range;
	 
				typedef typename math::euclid::space<E>::field field;
	 
				const field& lambda;
				scalar_ref(const field& x) : lambda(x) { }
      
				range operator()(const domain& x) const {
					return math::euclid::scal(lambda, x);
				}
	
				struct push : base< scalar_ref > {
					push(const scalar_ref& of, const domain& ) : push::base(of) {  }
				};

				struct pull : base< scalar_ref< typename math::euclid::space<E>::dual > > {
					pull(const scalar_ref& of, const domain& )
						: pull::base(of.lambda) { 
	     
					}
				};
			};

      
      
			template<class E>
			struct scalar2 {
	
				typedef typename math::euclid::space<E>::field field;
	
				typedef std::tuple<field, E> domain;
				typedef E range;
	
				range operator()(const domain& x) const {
					return math::euclid::scal(std::get<0>(x), std::get<1>(x));
				}
	

				struct push {
					const domain at;
	  
					push(const scalar2& , const domain& at) : at(at) { };
	
					range operator()(const domain& body) const {
						using namespace std;
						return get<0>(body) * get<1>(at) + get<0>(at) * get<1>(body);
					}
	
				};


			};
    

			template<class E, int M>
			struct projection {
	
				// orthogonal basis
				math::vector<E, M> subspace;
				projection(const math::vector<E, M>& subspace) : subspace(subspace) { }
				
				typedef typename math::euclid::space<E>::field field;
      
				typedef E domain;
				typedef math::vector< field, M > range;
      
				range operator()(const domain& e) const {
	
					range res;
					res.resize( subspace.rows() );

					res.each( [&](natural i) {
							res(i) = math::euclid::dot(this->subspace(i), e);
						});

					return res;
				}

				struct push : base< projection > {

					push( const projection& of, const domain&) 
						: push::base( of ) {  }
	
				};

				struct pull {
					math::vector<E, M> basis;
	  
					pull( const projection& of, const domain&) 
						: basis(of.subspace) {

					}


					typename math::euclid::space<E>::dual operator()(const math::covector<field, M>& f) const {
	    
						auto primal = math::euclid::of(basis(0));
						auto dual = *primal;
						typename math::euclid::space<E>::dual res = dual.zero();
	    
						f.each([&](natural i) {
								res = dual.sum(res, dual.scal(f(i), primal.transp(basis(i))));
							});
	    
						return res;
					};
	
				};
      

			};

    
   

			template<class E>
			struct dot1 {
      
				E with;
				math::euclid::space<E> euclid;

				dot1(const E& with,
				     const math::euclid::space<E>& euclid = math::euclid::space<E>() ) 
					: with(with),
					  euclid(euclid) {

				}
      
				typedef typename math::euclid::space<E>::field range;
      
				range operator()(const E& x) const { 
					return euclid.dot(with, x);
				}
      
				struct push : base< dot1 > {
					push(const dot1& of, const E& ) 
						: push::base(of.with, of.euclid) {

					}
				};

				struct pull : base< line<typename math::euclid::space<E>::dual> > {
	
					pull(const dot1& of, const E& ) 
						: pull::base(of.euclid.transp(of.with) , *of.euclid) { }
	
				};

			};



			namespace impl {

				template<class F, class = void> struct minus {};
      
				template<class F>
				struct minus<F, decltype( func::requires<F>() ) > {
					typedef compose< euclid::minus< typename traits<F>::range >, F > type;
				};


				template<class F, class G, class = void> struct sum { };

				template<class F, class G>
				struct sum<F, G, decltype( func::requires<F, G>() ) > {
					typedef compose< euclid::sum< typename traits<F>::range >, tuple_join<F, G> > type;
				};
      

				template<class F, class G, class = void> struct diff;
      
				template<class F, class G>
				struct diff<F, G, decltype( func::requires<F, G>() ) > : sum<F, typename minus<G>::type >{ 
	
				};
      

				template<class F, class = void> struct scalar;
      
				template<class F>
				struct scalar<F, decltype( func::requires<F>() ) > {
					typedef compose< euclid::scalar< typename traits<F>::range> , F > type;
				};
      

			}

		}


		// convenience constructors
		template<class F>
		auto norm2( const F& f, const math::euclid::space< typename traits<F>::range >& euclid =
		            math::euclid::space< typename traits<F>::range > () ) 
			-> decltype(requires<F>(), euclid::squared_norm< typename traits<F>::range >() << f ) {
			return euclid::squared_norm< typename traits<F>::range >(euclid) << f;
		}

		// TODO norm
    
		template<class E, int M>
		euclid::projection<E, M> project(const math::vector<E, M>& subspace) { return { subspace }; }
    

		// operators
		template<class F>
		typename euclid::impl::minus<F>::type operator-(const F& f) {
			return euclid::minus< typename traits<F>::range >()  << f;
		}
    

		template<class F, class G>
		typename euclid::impl::sum<F, G>::type operator+(const F& f, const G& g) {
			return euclid::sum< typename traits<F>::range >() << join(f, g);
		}
    
		template<class F, class G>
		typename euclid::impl::diff<F, G>::type operator-(const F& f, const G& g)  {
			return f + (-g);
		}
    
    
		template<class F>
		typename euclid::impl::scalar<F>::type operator*(typename math::euclid::space< typename traits<F>::range >::field lambda, const F& f)
		{
			return euclid::scalar< typename traits<F>::range >(lambda) << f;
		}
      
		template<class F>
		typename euclid::impl::scalar<F>::type operator*(const F& f, 
		                                                 typename math::euclid::space< typename traits<F>::range >::field lambda) 
		{
			return lambda * f;
		}
    
		template<class F>
		typename euclid::impl::scalar<F>::type operator/(const F& f, 
		                                                 typename math::euclid::space< typename traits<F>::range >::field lambda) 
		{
			assert( lambda != 0 );
			return f * (1.0 /lambda);
		}

    
	}

}

#endif
