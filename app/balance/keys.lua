
key = require 'key'
text = require 'text'


key.doc.y = 'toggle grab'

app.grab_stiff = app.grab_stiff or 1
local grab = 0

key.y = function() 
	   if grab == 0 then 
	      grab = app.grab_stiff 
	   else grab = 0 end
	   app.grab( grab )
	end




key.doc.o = 'toggle lie group/euclidean integration'

local pull = true
key.o = function()
	       pull = not pull
	       if pull then text.go("Lie group", 10)
	       else text.go("Euclidean", 10)
	       end
	       app.pull( pull )
	    end


key.doc.s = 'save current pose as initial pose'
key.s = function() 
	   app.save()
	   text.go("init pose saved")
	end

key.doc.n = 'show some plots'
key.n = function() 
	   plot.show( bilevel )
	end


key.doc.u = 'put character upright'
key.u = app.straighten		-- TODO rename ?


key.doc.h = 'show/hide visual hints'
key.h = key.toggle( app.hints )

key.doc.g = 'toggle gravity'
key.g = key.toggle( app.gravity )


key.doc.a = 'toggle actuation log' 
key.a = function() 
	   if sim.post_animate.log_act then 
	      sim.post_animate.log_act = nil
	   else
	      sim.post_animate.log_act = app.log_act
	   end
	end


key.doc.p = 'spin +/-'
key.p = function() 
	   app.spin(10)
	end

key.P = function() 
	   app.spin(-10)
	end

key.doc.l = 'toggle light drawing on/off'
key.l = key.toggle( app.light.draw )


key.doc.k = 'toggles bretelles on/off'
key.k = key.toggle(app.bretelles)