import bpy

def main():
    o = bpy.data.objects['Cylinder']
    
    print("Test")

    frame_start = bpy.context.scene.frame_start
    frame_end = bpy.context.scene.frame_end

    for i in range(frame_start, frame_end):
        bpy.context.scene.frame_set(i)
        # bpy.context.scene.update()

        #  m = o.to_mesh(bpy.context.scene, True, 'PREVIEW')
        
        m = o.to_mesh(preserve_all_data_layers = True)

        print("Frame %d:" % i)

        for v in m.vertices:
            print("    (%f %f %f)" % (v.co.x, v.co.y, v.co.z))

        print("\n")
        
main()