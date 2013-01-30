
local api = require 'api'
local sofa = api.load('sofa')

local rocks = require 'rocks'
rocks.require( 'LuaXml' )


local tree = require 'tree'

function sofa.new() 
  

   local res = sofa.node("Root")
   res.dt = "1e-03"

   
   local vis = sofa.obj("VisualStyle")
   vis.displayFlags = "hideVisual showBehaviorModels hideForceFields hideInteractionForceFields hideCollision hideMapping hideOptions"
   
   res:append( vis )
   
   return res;
   
end


function sofa.node( name ) 
   local res = xml.new("Node")
   res.name = name
   return res
end

function sofa.obj( name, attr ) 
   local res =  xml.new( name )

   if attr then 
      for k, v in pairs(attr) do
	 res[k] = v
      end
   end
   
   return res
end


function sofa.dofs( type )
   local res = sofa.obj("MechanicalObject")
   res.template = type

   return res
end

function sofa.map(name, from, to) 
   local res = sofa.obj(name)
   res.template = from..','..to
   
   return res
end

function sofa.vec3_coord( v )
   local res = v.x..' '..
      v.y..' '..
      v.z

   return res
end

function sofa.quat_coord( q )
   local res = q.x..' '..
      q.y..' '..
      q.z..' '..
      q.w

   return res
end

function sofa.rigid_coord( g ) 
   -- if not g then print(debug.traceback()) end
   local res = sofa.vec3_coord(g.pos)..' '..sofa.quat_coord(g.orient)
   return res
end


local cpp = {}

cpp.skeleton = sofa.skeleton

function sofa.skeleton(filename) 
   
   local cmd = "return "..cpp.skeleton(filename)
   return loadstring(cmd)()

end

function sofa.scene(node, skel) 
   local rigid = "Rigid"
   local vec6 = "Vec6d"
   
   local dofs = sofa.dofs( rigid )
   local mass = sofa.obj("UniformMass", attr)
   
   dofs.position = ''

   for i, b in ipairs(skel.body) do
      dofs.position = dofs.position..' '..sofa.rigid_coord( b.config )
   end
   
   local joints = sofa.node("Joints")
   local joint_dofs = sofa.dofs( rigid )
   local joint_mapping = sofa.map("JointRigidMapping", rigid, rigid)
   
   joints:append( joint_dofs )
   joints:append( joint_mapping )
   
   -- TODO set joints config

   joint_mapping.source = ''
   
   joint_dofs.position = '' -- TODO is this needed lol ?
   
   local default_rigid = '0 0 0 0 0 0 0'
   local default_vec6 = '0 0 0 0 0 0'
   
   for i, p in ipairs(skel.joint) do
      for k,j in pairs(p) do
	 joint_dofs.position = joint_dofs.position ..' '.. default_rigid
	 joint_mapping.source = joint_mapping.source..' '..j.id..' '..sofa.rigid_coord( j.offset )
      end
   end
   
   local relative = sofa.node("Relative")
   local relative_dofs = sofa.dofs( rigid )
   local relative_mapping = sofa.map("RelativeRigidMapping", rigid, rigid )
   
   
   relative_mapping.edges = ''
   relative_dofs.position = ''
      
   for i, b in ipairs(skel.joint) do
      relative_dofs.position = relative_dofs.position ..' '.. default_rigid
      relative_mapping.edges = relative_mapping.edges .. (2 * (i - 1)) .. ' ' ..  (2 * (i - 1) + 1) .. ' '
   end
   
   joints:append( relative )
   relative:append( relative_dofs )
   relative:append( relative_mapping )
   
   local rest = sofa.node("Rest")
   local rest_dofs = sofa.dofs( rigid )
   local rest_map = sofa.map("DisplacementMapping", rigid, rigid)
   
   rest:append(rest_dofs)
   rest:append(rest_map)
   
   rest_map.reference = ''
   rest_dofs.position = ''
   
   for i, g in ipairs(skel.rest) do
      rest_dofs.position = rest_dofs.position .. ' ' .. default_rigid
      rest_map.reference = rest_map.reference ..' '.. sofa.rigid_coord( g )
   end
   
   
   relative:append(rest)


   local log = sofa.node("Log")
   local log_dofs = sofa.dofs( vec6 )
   local log_map = sofa.map("LogRigidMapping", rigid, vec6 )
   local log_comp = sofa.obj("DiagonalCompliance", {template = vec6, dampingRatio = 0.001} )
   
   
   local trans_comp = 0
   local rot_comp = 1e-5

   local default_comp = trans_comp ..' '.. trans_comp ..' '.. trans_comp ..' '.. rot_comp ..' '.. rot_comp ..' '.. rot_comp;
   
   log_dofs.position = ''
   log_comp.compliance = ''
   
   for i, b in ipairs(skel.joint) do
      log_dofs.position = log_dofs.position .. ' ' .. default_vec6
      log_comp.compliance = log_comp.compliance .. ' ' .. default_comp
   end
   
   log:append( log_dofs )
   log:append( log_map )
   log:append( log_comp )
   
   rest:append( log )
   
   node:append( dofs )
   node:append( mass )
   node:append( joints )
   
end


function sofa.test( out ) 
   require 'refresh'
   

   local root = sofa.new()
   local skel = sofa.node("RigidBody")
   
   root:append( skel )

   local solver = sofa.obj("MinresSolver", { implicitVelocity = 1,
					     implicitPosition = 1,
					     rayleighStiffness = 0,
					     rayleighMass = 0 }
			)
   skel:append(solver)
   
   local filename = '/home/max/meuh/share/mocap/test.bvh'
   sofa.scene( skel, sofa.skeleton( filename ) )
   
    
   if out then 
      xml.save( root, out )
   else
      print( root )
   end
   
end


sofa.test( '/tmp/skeleton.scn' )



return sofa

