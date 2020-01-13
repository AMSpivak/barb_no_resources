bl_info = {
    "name": "Export Simple Game Object Format Animation.jan) 3",
    "author": "Yshka",
    "version": (1, 1),
    "blender": (2, 80, 0),
    "location": "File > Export > Export JAN 3",
    "description": "Export to the Simple Game Object Format Animation(.jan) 3",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Import-Export"
}

import bpy, struct, math, os, time, mathutils



def rawData(m):
	s='';
	for i in range(4):
		s+= "%f " % m[i][0]
		s+= "%f " % m[i][1]
		s+= "%f " % m[i][2]
		s+= "%f \n" % m[i][3] 
	return s;



def rawVector(v,param):
	i=0;
	s='';
	s+= "%f," % v[param][0]
	s+= "%f," % v[param][1]
	s+= "%f" % v[param][2]
	return s;

class JANExporter(bpy.types.Operator):
    bl_idname = "export.jan"
    bl_label = "Export JAN"
    filepath = bpy.props.StringProperty(subtype='FILE_PATH')
    
    
    
    
    def execute(self, context):
        #Append .sgo if needed
        FilePath = self.filepath
        if not FilePath.lower().endswith(".jan"):
            FilePath += ".jan"
            
        file = open(FilePath, 'w')
        file.write("%s\n" % ("simple game object export addon"))
        
        sce = bpy.context.scene

        arm_ob= context.view_layer.objects.active

        
        arm = arm_ob.pose
        
        #arm = bpy.data.objects["Armature"].pose
        
        file.write('Bones %d\n' % len(arm.bones.values()))
        
        boneName = '';
        boneMat = '';
        file.write('[\n')
        file.write('Frames %d\n' % (sce.frame_end-sce.frame_start+1))
        
        for f in range(sce.frame_start, sce.frame_end+1):
            sce.frame_set(f)
            file.write('Frame %i\n' % f)
            for bone in arm.bones.values():
                    boneName = bone.name;
                    boneMat = rawData(bone.matrix)+'\n';
                    file.write(boneName)
                    file.write('\n')
                    file.write(boneMat)
                
                                   
        
        file.write('];\n')
        
        file.close()
        
        return {"FINISHED"}
    
    def invoke(self, context, event):
        self.filepath = ""
        wm = context.window_manager
        wm.fileselect_add(self)
        return {'RUNNING_MODAL'}

def menu_func_export_button(self, context):
    self.layout.operator(JANExporter.bl_idname, text="Export animation JAN(.jan)")


classes = [
    JANExporter,
]


def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_file_export.append(menu_func_export_button)
    #bpy.types.VIEW3D_PT_tools_objectmode.prepend(menu_func_export_button)


def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export_button)
    #bpy.types.VIEW3D_PT_tools_objectmode.remove(menu_func_export_button)
    for cls in classes:
        bpy.utils.unregister_class(cls)


if __name__ == "__main__":
    register()