bl_info = {
    "name": "Export Simple Game Object Format Bone 3(.jub)",
    "author": "Yshka",
    "version": (1, 1),
    "blender": (2, 80, 0),
    "location": "File > Export > Export JUB 3",
    "description": "Export to the Simple Game Object Format Bone 3(.jub)",
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



def rawVector(v,param):
	i=0;
	s='';
	s+= "%f," % v[param][0]
	s+= "%f," % v[param][1]
	s+= "%f" % v[param][2]
	return s;

class JUBExporter2(bpy.types.Operator):
    bl_idname = "export.jub"
    bl_label = "Export JUB"
    filepath = bpy.props.StringProperty(subtype='FILE_PATH')
    
    
    
    
    def execute(self, context):
        #Append .sgo if needed
        FilePath = self.filepath
        if not FilePath.lower().endswith(".jub"):
            FilePath += ".jub"
            
        file = open(FilePath, 'w')
        file.write("%s\n" % ("simple game object export addon"))
        
        
        sce = bpy.context.scene
        arm_ob= sce.objects.active


        
        arm = arm_ob.pose
        
        file.write('Bones %d\n' % len(arm.bones.values()))
        
        boneName = '';
        boneMat = '';
        file.write('[\n')
        
        i=0;
        
        for bone in arm.bones.values():
                boneName = bone.name;
                if bone.parent is None:
                    boneName += ' root\n';
                else:
                    boneName += ' ' + bone.parent.name+'\n'; 
                boneMat = rawData(bone.matrix)+'\n';
                file.write(boneName)
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
    self.layout.operator(JUBExporter2.bl_idname, text="Export bones JUB 2(.jub)", icon='EXPORT')
def register():
    bpy.utils.register_class(JUBExporter2)
    bpy.types.INFO_MT_file_export.append(menu_func)
def unregister():
    bpy.utils.unregister_class(JUBExporter2)
    bpy.types.INFO_MT_file_export.remove(menu_func)
if __name__ == "__main__":
    register()