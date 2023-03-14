import bpy
import struct

def export_omp(context, filepath):
    print("Running omp export...")
    
    verts = []
    indices = []
    
    with open(filepath, 'w') as f:
        # magic and version information
        f.write("omp 1\n")
            

        objects = bpy.context.scene.objects
        for obj in objects:
            # start of object
            mesh = obj.data
            f.write("obj " + str(len(mesh.polygons)) + " " + str(len(mesh.vertices)) + "\n")
            f.write("t " + str(obj.active_material_index) + "\n")
                
            #vertices
            for face in mesh.polygons:
                f.write("f ")
                f.write(str(face.vertices[0]) + " ")
                f.write(str(face.vertices[1]) + " ")
                f.write(str(face.vertices[2]) + "\n")
            
            # vertex pos
            for vertex in mesh.vertices:
                f.write("v ")
                f.write(str(vertex.co[0]) + " ")
                f.write(str(vertex.co[2]) + " ")
                f.write(str(vertex.co[1]) + "\n")
                
    print("Done!")

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export.omp"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export map to omp"

    # ExportHelper mixin class uses this
    filename_ext = ".omp"

    filter_glob: StringProperty(
        default="*.omp",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    def execute(self, context):
        return export_omp(context, self.filepath)

# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Text Export Operator")


def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()
    bpy.ops.export.omp('INVOKE_DEFAULT')