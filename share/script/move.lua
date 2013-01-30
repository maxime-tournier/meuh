
local pd = require 'pd'
local move = {}

move.stance = { p = 0.01, d = 0}
move.height = { p = 1, d = 0}

function move.right() move.stance.desired = 0; text.go("Right") end
function move.center() move.stance.desired = 0.5; text.go("Center") end
function move.left() move.stance.desired = 1; text.go("Left") end

function move.crouch() 
   move.height.desired = 7 ;
   move.height.p = 0.1
   move.jumping = false
   text.go("Crouch") 
end

function move.stand () 
   move.height.p = 0.1
   move.height.desired = 14 ; 
   move.jumping = false;
   text.go("Stand") 
end

function move.jump() 
   local val = 14; 
   move.height.p = 0.5
   move.height.desired = 15 ; 
   move.jumping = true
   text.go("Jump") 
end


function move.on_load() 
   
end

function move.on_restart() 
   
   move.height.set = function (x) app.com_height:set(x) end
   
   move.stance.current = 0.5
   move.height.current = app.com_height:get() 
   
   move.center()
   move.stand();

end

function move.post_animation() 

   if not move.stance.set then 
      move.stance.set = cpp.app().stance
   end
   
   pd.update( move.height )
   pd.update( move.stance )

end

-- register hooks
animate['move'] = move.post_animation
restart['move'] = move.on_restart
load['move'] = move.on_load

keys['1'] = move.right
keys['2'] = move.center
keys['3'] = move.left

keys['c'] = move.crouch
keys['v'] = move.stand
keys['b'] = move.jump

return move