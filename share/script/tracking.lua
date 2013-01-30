
-- app customization table
local custom = { }

-- adds a specific behavior for given motion
custom['124_07'] = function() app.length(60) end

-- same
custom['56_01'] = 
   function()
      local w = 1000;
      app.track( track.com, w )
      app.track( track.lfoot, w )
      app.track( track.rfoot, w )
      app.track( track.lhand, w )
      app.stiffness(1000)
      app.damping(1000)
      app.length(10)
   end


function on_load()

   app.set_geo( 14 )
   app.restart()
   -- app.ccsk()

   update()
   
   app.control:set( true )
   app.tracking:set( true )	-- forces tracking
   
   app.bretelles:set( true )
   app.balance:set( 0 )

   app.push_stiff:set( 2000 )
   -- app.limits:set( true )
   
   app.speed:set(1)
   app.dt:set(0.01)

   app.damp_model:set(1)

   track.update()
   
   -- customization
   if not customize( custom ) then 

      -- default customization goes here
      track.all(5500)
      app.damping():set( 400 )
      app.stiffness():set( 1000 )
   end

end


track = {}

function track.update() 
   track.head  = app.dof('head')
   track.lhand = app.dof('lhand')
   track.rhand = app.dof('rhand')
   track.lfoot = app.dof('lfoot')
   track.rfoot = app.dof('rfoot')
   track.com = -1;
end

function track.all(weight) 
   app.track( track.head,  weight )
   app.track( track.lfoot, weight )
   app.track( track.rfoot, weight )
   app.track( track.lhand, weight )
   app.track( track.rhand, weight )
   app.track( track.com,   weight )
end



load['tracking'] = on_load



-- app.debug( true )