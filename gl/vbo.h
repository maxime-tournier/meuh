#ifndef GL_VBO_H
#define GL_VBO_H

#include <gl/gl.h>
#include <functional>

#include <math/types.h>
#include <list>
#include <core/tuple/offset.h>

// #include <iostream>

#include <math/eigen.h>
#include <memory>

// opengl vertex buffer object 

// watch out for memory alignemnt bugs ! (i.e. math::vec2 must be
// 16-bytes aligned) 

// usage: 
//   gl::vbo vbo;
//   gl::array< math::vec2, math::vec3 > data;

//   data.layout().texcoord<0>().vertex<1>();

//   auto texcoord = data.access<0>();
//   auto vertex = data.access<1>();

//   texcoord(0) = math::vec2::Zero();
//   ... 
//   // in opengl context
//   vbo.gen().bind();
//   vbo.send( data ).unbind();

//   // draw call: 
//   vbo.draw( GL_TRIANGLES );


namespace gl {
  
  class vbo {
    GLuint id_;
    GLenum usage_;
    GLenum target_;
    
    typedef vbo self;
    
  public:
      
    vbo(GLenum = GL_ARRAY_BUFFER);
    GLuint id() const;      
    
    GLenum target() const;

    vbo& usage(GLenum );
    GLenum usage() const;

    self& gen();
    self& del();
    
    self& bind();
    const self& unbind() const;
 
    struct data_type;
    self& send(std::unique_ptr<data_type>&& data);
    
    // convenience
    self& draw(GLenum mode = GL_TRIANGLES );
    
    struct layout_type {
      typedef layout_type self;

      virtual ~layout_type();

      const self& enable() const;
      const self& draw(GLenum mode) const;
      const self& disable() const;
      
    protected:
      typedef std::function< void () > callback;
      std::list<callback> pre_draw, post_draw;
      std::function< void(GLenum) > on_draw;
    };
    
    const data_type& data() const;
    
  private:
    std::unique_ptr<data_type> _data;
    
  };
    

  // base class for data
  struct vbo::data_type {
    virtual ~data_type();

    virtual const layout_type& layout() const = 0;
    virtual unsigned int size() const = 0;
    virtual unsigned int count() const = 0;
    virtual const GLvoid* bits() const = 0;
  };

  
  // in practice you use this one
  template<class ... Args>
  class array : public vbo::data_type {
    typedef array self;
  public:
    typedef std::tuple<Args...> chunk_type;
      
  private:
    // for alignement
    typedef Eigen::Array< chunk_type, Eigen::Dynamic, 1> bits_type;
    bits_type bits_;

  public:
    const GLvoid* bits() const { return &bits_[0]; }
    unsigned int size() const { return bits_.size() * sizeof(chunk_type); }
    unsigned int count() const { return bits_.size(); }
    unsigned int stride() const { return sizeof(chunk_type); }

    template<int I>
    const GLvoid* offset() const { return (const GLvoid*)core::tuple::offset<I, Args...>::value; }

    template<int I> struct get { 
      typedef typename std::tuple_element<I, chunk_type>::type type;
    };
      
    self& resize(unsigned int i) { bits_.resize(i); return *this;}

    array(unsigned int i) : layout_(this) { resize(i); }
    array() : layout_(this){ }
      
    template<int I, class Bits> struct accessor {
      Bits& bits;
	
      auto operator()(unsigned int i) const -> decltype( std::get<I>(bits(i)) )& {
	return std::get<I>(bits(i));
      }
    };
      
      
    template<int I>
    accessor<I, bits_type> access() { return {bits_}; }

    template<int I>
    accessor<I, const bits_type> access() const { return {bits_}; }
      
    struct layout_type;
    const layout_type& layout() const { return layout_; }
    layout_type& layout() { return layout_; }
    
  private:
    layout_type layout_;
    
  };


  
  template<class > struct info;
  
  // TODO rename size => count for grate justice
  template<class U, int M>
  struct info<math::vector<U, M> >{
    static unsigned int size() { return M * info<U>::size(); }
    static GLenum type() { return info<U>::type(); }
  };
  
  template<>
  struct info<unsigned short> {
    static GLenum type();
    static unsigned int size();
  };


  template<>
  struct info<double> {
    static GLenum type();
    static unsigned int size();
  };



  
  // TODO FIXME !
  template<class ... Args >
  class array<Args...>::layout_type : public vbo::layout_type {
    const array<Args...>* array_;
    typedef layout_type self;
  public:
    layout_type( const array<Args...>* array ) 
      : array_ ( array ) { 
      
      // TODO FIXME crashes if array is deleted before draw
      on_draw = [array](GLenum mode) {
	glDrawArrays(mode, 0, array->count() );
      };
	
    }            

    template<int I>
    self& vertex() {
	
      const unsigned int size = self::size<I>();
      const GLenum type = self::type<I>();
      const unsigned int stride = array_->stride();
      const GLvoid* offset = array_->template offset<I>();
      
      pre_draw.push_back( [stride, size, type, offset ] {
	  glVertexPointer( size, type, stride, offset);
	  glEnableClientState( GL_VERTEX_ARRAY );
	} );
	
      post_draw.push_back( [] {
	  glDisableClientState( GL_VERTEX_ARRAY );
	} );
	
      return *this;
    }


    template<int I>
    self& color() {
	
      const unsigned int size = self::size<I>();
      const GLenum type = self::type<I>();
      const unsigned int stride = array_->stride();
      const GLvoid* offset = array_->template offset<I>();

      pre_draw.push_back( [=] {
	  glColorPointer( size, type, stride, offset);
	  glEnableClientState( GL_COLOR_ARRAY );
	} );
	
      post_draw.push_back( [] {
	  glDisableClientState( GL_COLOR_ARRAY );
	} );
	
      return *this;
    }

    template<int I>
    self& normal()  {
      
      const GLenum type = self::type<I>();
      const unsigned int stride = array_->stride();
      const GLvoid* offset = array_->template offset<I>();

      pre_draw.push_back( [type, stride, offset] {
	  glNormalPointer( type, stride, offset);
	  glEnableClientState( GL_NORMAL_ARRAY );
	} );
	
      post_draw.push_back( [] {
	  glDisableClientState( GL_NORMAL_ARRAY );
	} );
	
      return *this;
    }

    template<int I>
    self& texcoord()  {

      const unsigned int size = self::size<I>();
      const GLenum type = self::type<I>();
      const unsigned int stride = array_->stride();
      const GLvoid* offset = array_->template offset<I>();
      
      pre_draw.push_back( [size,type,stride,offset] {
	  glTexCoordPointer( size, type, stride, offset);
	  glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	} );
	
      post_draw.push_back( [] {
	  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	} );
	
      return *this;
    }


    
    // for index buffers
    template<int I>
    self& element() {
	
      assert(pre_draw.empty());
      assert(post_draw.empty());
	
      const unsigned int size = self::size<I>();
      const unsigned int count = array_->count();
      const GLenum type = self::type<I>();
      
      on_draw = [size, count, type](GLenum mode) {
	glDrawElements(mode, size * count, type, 0 );
      };
	
      return *this;
    }


  private:
    template<int I>
    static unsigned int size() {
      return info< typename get<I>::type >::size();
    }

    template<int I>
    static GLenum type() {
      return info< typename get<I>::type >::type();
    }
  };

}












#endif
