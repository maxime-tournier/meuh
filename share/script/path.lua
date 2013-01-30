local path = {}

function path.add(dir)
   package.path = dir..'/?.lua;'..package.path
end

return path