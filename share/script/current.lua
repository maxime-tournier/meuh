



function restart()
   app = cpp.app()
   
   post_animation = animate(app.coord, 4)
   app.dt:set(0.01)
end

function on_restart() 
   
   -- haha !
   -- refresh()
   -- restart()
end




-- post_animation = function() print('i are here') end

cpp.app().log( 'loaded current script' )
