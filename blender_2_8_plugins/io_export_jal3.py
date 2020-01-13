bl_info = {
    "name": "Export Simple Game Object Format (.jal) 3",
    "author": "Yshka",
    "version": (1, 1),
    "blender": (2, 80, 0),
    "location": "File > Export > Export JAL 3",
    "description": "Export to the Simple Game Object Format Mesh (.jal) 3",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Import-Export"
}

import bpy, struct, math, os, time



def rawData(m):
	s='';
	for i in range(4):
		s+= "%f " % m[i][0]
		s+= "%f " % m[i][1]
		s+= "%f " % m[i][2]
		s+= "%f \n" % m[i][3] 
	return s;



def rawVector(v):
	i=0;
	s='';
	s+= "%f " % v[0]
	s+= "%f " % v[1]
	s+= "%f" % v[2]
	return s;

def rawUV(v):
	i=0;
	s='';
	s+= "%f " % v[0]
	s+= "%f" % v[1]
	return s;

class JALExporter(bpy.types.Operator):
    bl_idname = "export.jal"
    bl_label = "Export JAL"
    filepath = bpy.props.StringProperty(subtype='FILE_PATH')
    
    
    
    
    def execute(self, context):
        #Append .sgo if needed
        FilePath = self.filepath
        if not FilePath.lower().endswith(".jal"):
            FilePath += ".jal"
            
        file = open(FilePath, 'w')
        file.write("%s\n" % ("simple game object export addon"))
        
        file.write('Meshes %d\n' % len(bpy.data.meshes.values()))
        cycle = [0,1,2]
        sce = bpy.context.scene
        mesh_ob= sce.objects.active

       
        mesh = mesh_ob

	
        polys = mesh.data.polygons
        verts = mesh.data.vertices
        tex = mesh.data.uv_layers.active.data
                
        file.write('Faces %d\n' % len(polys.values()))
        file.write('[\n')
 
        mesh.data.calc_normals_split()
        loops = mesh.data.loops
		
        for p in mesh.data.polygons.values():
                    
        	boneName = 'face %d' %p.index; 
        	boneName +='\n';
        	for c in cycle:
                	#boneName = rawVector(verts[p.vertices[c]].co)+' ' +rawVector(verts[p.vertices[c]].normal)+' ';
                	boneName = rawVector(verts[p.vertices[c]].co)+' ' +rawVector(loops[p.loop_indices[c]].normal)+' ';
                	boneName += rawUV(tex[p.loop_indices[c]].uv)+ '\n';
                    
                	file.write(boneName)
                	boneMat = '';
                    
                	for g in verts[p.vertices[c]].groups.values():
                		boneMat +='%d '  % g.group;
                		boneMat +='%f '  % g.weight;
                	boneMat +='\n';
                	file.write(boneMat) 
                
                
        
        file.write('];\n')
            
        file.close()
        
        return {"FINISHED"}
    
    def invoke(self, context, event):
        self.filepath = ""
        wm = context.window_manager
        wm.fileselect_add(self)
        return {'RUNNING_MODAL'}

def menu_func(self, context):
    self.layout.operator(JALExporter.bl_idname, text="Export JAL (.jal)", icon='EXPORT')
def register():
    bpy.utils.register_class(JALExporter)
    bpy.types.INFO_MT_file_export.append(menu_func)
def unregister():
    bpy.utils.unregister_class(JALxporter)
    bpy.types.INFO_MT_file_export.remove(menu_func)
if __name__ == "__main__":
    register()