
local full = { 
   solver = 2,
   iter = 500,
   eps = 1e-16,

   stiff = 5,
   damping = 0.4,
   
   lazy = 1e-3,
   smooth = 1e-5,
   head = 1,
   com = 1,
   -- am = 1,
   zmp = 0.5,
   support = 10,
   ref = 1e-4,

}


local fake = { 
   solver = 1,
   iter = 200,
   eps = 1e-5,
   stiff = 1,
   damping = 1,
   -- ref = 1e-4,			-- something wrong here
   
   -- com = 8e-2,
   -- am = 8e-2,
   -- zmp = 8e-3,
   -- support = 1.5e-2

   ref = 0.5,
   com = 1e-1,
   am = 1e-2,
   head = 0.1,

   zmp = 0.15,
}

local uncontrolled = { 
   solver = 0,
   iter = 50,
   eps = 1e-16,
   stiff = 250,
   damping = 0.1,
}


local fly = {
   solver = 1,
   iter = 100,
   eps = 1e-6,

   stiff = 5,
   damping = 1,
 
   com = 0,
   am = 0,
   zmp = 0,

   
}

function app.light.setup()
   app.light.pos {0, 100, 0}
   app.light.target = function() return app.char.com end
end


sim.post_reset.light = app.light.setup

function app.fly() 
   sim.stop()
   
   app.params = fly

   app.steps( 2 )
   app.spin( 1000 )
   app.grab_stiff = 1
   app.gravity( false )
   app.char.pos {0, 30, 0}
   app.save()
   app.hints( false )
   
   sim.start()
end

function app.fake() 
   sim.stop()
   
   app.params = fake
   
   app.steps( 2 )
   -- app.spin( 1000 )
   
   app.grab_stiff = 1
   
   app.gravity( true )
   -- app.char.pos {0, 30, 0}
   
   app.save()
   app.hints( false )
   app.bretelles( false )
   
   sim.start()

end


function app.full()
   sim.stop()
   
   app.params = full
   app.gravity( true )
   app.steps( 1 )
   app.hints( true )
   
   sim.start();
end

-- visual style
local style = {}

function style.blue() 
   viewer.radius( 2000 )
   
   local bg = 0.14
   local bg3 = {bg, bg, bg}
   
   gl.clear.color( bg3 )
   gl.fog.color( bg3 )
   gl.fog.density( 0.003 )
   gl.ambient {0.4, 0.4, 0.4}

   app.light.pos {0, 100, 0}
   
   app.char.color( gl.html(0xaaaaee) )
   app.light.setup()
      

end


function app.init()
   style.blue();
   
   
   app.exclude { 'ltoes', 'rtoes', 
		 'lthumb', 'rthumb', 
		 'rfingers', 'lfingers',
		 'lhand', 'rhand'
	      }
   
   -- app.bvh(85, 12, 8)
   app.load('breakdance.bvh', 8)
   sim.timestep( 2e-1 )
   
   viewer.resize(900, 600)
   app.fake()
    
   sim.reset()
end



