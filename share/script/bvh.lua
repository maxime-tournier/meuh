require 'lfs'

local bvh = {}

local function check_path() 
   if not bvh.path then error('must init first lol') end
end

-- initializes the bvh database using a path
function bvh.init( root )
   if not root then
      root = os.getenv("HOME") .. '/bvh'
   end
   bvh.path = root
end

-- loads a bvh in the main app given subject/trial
function bvh.load(subject, trial)
   check_path()
   
   bvh.current = {subject = subject, trial = trial}

   -- padds zeros
   local ssub = "";
   if subject < 10 then ssub = '0' end
   ssub = ssub .. subject;

   -- padds zeros
   local strial = "";
   if trial < 10 then strial = '0' end
   strial = strial .. trial;
   
   app.load(bvh.path .. '/'.. ssub .. '/' .. ssub .. '_' .. strial ..'.bvh' )
end

-- returns a big table of everything
function bvh.files()
   check_path();
   local res = {}
   for sub in lfs.dir( bvh.path ) do
      
      if lfs.attributes( bvh.path ..'/'.. sub, "mode") == "directory" then
	 
	 local isub = tonumber(sub)
	 if isub then 
	    res[ isub ] = {}
	    for trial in lfs.dir( bvh.path ..'/' .. sub ) do
	       local s, e, ret =  string.find(trial,sub..'_(%d%d).bvh$')
	       if ret then
		  res[isub][tonumber(ret)] = bvh.path ..'/' .. sub ..'/' .. trial
	       end
	    end
	 end
      end
   end
   
   return res
end



-- gives the next subject/trial bvh.load( bvh.next() )
function bvh.next()
   
   local files = bvh.files()
   local res_sub = bvh.current.subject
   local sub = files[ bvh.current.subject ]
   
   local tri = next(sub, bvh.current.trial)
   
   while not tri do
      res_sub = next( files, res_sub )
      
      if not res_sub then error("end!") end

      tri = next( files[res_sub] )
   end
   
   return res_sub, tri
   -- return {subject = res_sub, trial = tri}
end

return bvh
