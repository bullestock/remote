import cadquery as cq

res = (cq.Workplane("XY")
       .rarray(1, 5*2.54, 1, 2)
       .rect(2.54*3, 3)
       .extrude(1.5)
       .edges(">Z")
       .fillet(0.23)
       .faces(">Z")
       .workplane()
       .rect(2.54, 2.54*6)
       .extrude(8.5)
       .edges("|Z")
       .fillet(0.1)
       .rarray(1, 2.54, 1, 4)
       .circle(0.7)
       .cutThruAll()
)

show_object(res)
