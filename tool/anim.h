#ifndef TOOL_ANIM_H
#define TOOL_ANIM_H



#include <math/types.h>
#include <math/real.h>
#include <QObject>

#include <QTimer>
#include <QTime>

#include <memory>
#include <mutex>

namespace tool {

  namespace anim {
    
    class time : public QObject {
      Q_OBJECT;
      
      math::real value;
    public:
      
      time();

      const math::real& get() const;

    public slots:
      void set(double x);

    signals:
      void changed( double x );
      
    };



    class control : public QObject {
      Q_OBJECT;
      
      math::real _start, _end, _speed;
      bool _loop;

      QTime _qtime;
      QTimer _qtimer;

      std::unique_ptr<anim::time> _time;
      void connect();

      std::mutex mutex;
    public:
      
      control();
      
      anim::time* time() const;
      
      math::real start() const;
      math::real end() const;
      math::real speed() const;

      math::real interval() const;
      bool loop() const;
	 	
      bool playing() const;
      math::T<math::real> dt() const;	  

    public slots:

      // properties
      void start(math::real);
      void end(math::real);
      void speed(math::real);
      void loop(bool);
      void interval(math::real t);

      // action
      void go(math::real t);
      
      void play();
      void stop();
      void pause();
  
      void restart();
      void reset();





      // convenience
      math::real length() const;
      void step(math::real dt);
		
      math::real clamp(math::real t) const;

    signals:
      void playback( bool );
					  
    private slots:
      void poll();
    };



  }

}


#endif
