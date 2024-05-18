import bpy

mesh_list = []
index_list = []

def get_mesh_index(mesh): # Returns -1 if couldn't be found in existing data structure
    global mesh_list, index_list
    index = 0
    for other_mesh in mesh_list:
        same = True
        for i in range(len(other_mesh.vertices)):
            if(other_mesh.vertices[i] != mesh.vertices[i]):
                same = False
                break
        if same:
            return index
        else:
            index += 1
    return -1

def handle_mesh(mesh):
    global mesh_list, index_list
    index = get_mesh_index(mesh)
    if(index == -1):
        index_list.append(len(mesh_list))
        mesh_list.append(mesh)
    else:
        index_list.append(index)

def organise_mesh_data(context):
    dg = context.evaluated_depsgraph_get()
    o = context.object.evaluated_get(dg)
    
    frame_start = context.scene.frame_start
    frame_end = context.scene.frame_end
    
    for i in range(frame_start, frame_end):
        context.scene.frame_set(i)    
        m = o.to_mesh(preserve_all_data_layers=True, depsgraph=dg).copy()
        
        handle_mesh(m)

def write_some_data(context, filepath, use_some_setting):
    print("running write_some_data...")
    
    organise_mesh_data(context)
    
    f = open(filepath, 'w', encoding='utf-8')
    # f.write("Hello World %s\n\n" % use_some_setting)
    
    f.write(str(context.scene.render.fps) + "\n")
    
    f.write(str(context.scene.frame_end - context.scene.frame_start) + "\n")
    
    for i in index_list:
        f.write(str(i) + "\n")
        
    for m in mesh_list:
        f.write("\n")
        for v in m.vertices:
            f.write("%f %f %f %f %f %f\n" % (v.co.x, v.co.y, v.co.z, v.normal.x, v.normal.y, v.normal.z))
            
    f.close()
    
    # f.write(str(m.vertices[0].co.x))
    
    """frame_start = context.scene.frame_start
    frame_end = context.scene.frame_end
    
    for i in range(frame_start, frame_end):
        context.scene.frame_set(i)
        
        m = o.to_mesh(preserve_all_data_layers=True, depsgraph=dg)
        
        for v in m.vertices:
            f.write("%f %f %f %f %f %f\n" % (v.co.x, v.co.y, v.co.z, v.normal.x, v.normal.y, v.normal.z))
        f.write("next_keyframe\n")
    
    f.close()"""

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export_test.some_data"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export Some Data"

    # ExportHelper mixin class uses this
    filename_ext = ".anim"

    filter_glob: StringProperty(
        default="*.anim",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    use_setting: BoolProperty(
        name="Example Boolean",
        description="Example Tooltip",
        default=True,
    )

    type: EnumProperty(
        name="Example Enum",
        description="Choose between two items",
        items=(
            ('OPT_A', "First Option", "Description one"),
            ('OPT_B', "Second Option", "Description two"),
        ),
        default='OPT_A',
    )

    def execute(self, context):
        return write_some_data(context, self.filepath, self.use_setting)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Text Export Operator")


# Register and add to the "file selector" menu (required to use F3 search "Text Export Operator" for quick access).
def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export_test.some_data('INVOKE_DEFAULT')
