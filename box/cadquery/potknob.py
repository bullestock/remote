import cadquery as cq

d1 = 6
d2 = 10
d3 = 20
d4 = 15

l1 = 5
l2 = 15
l3 = 12
l4 = 9
l5 = 10

w1 = 1.5

nof_knurls = 12
knurl_dia = 2

res = (cq.Workplane("XY")
       .tag("o")
       # stem
       .circle(d2/2)
       .extrude(l5)
       # knob body
       .workplaneFromTagged("o")
       .workplane(l1)
       .circle(d3/2)
       .extrude(l3)
       # hole for pot
       .workplaneFromTagged("o")
       .circle(d1/2)
       .cutBlind(l2)
       # slit
       .workplaneFromTagged("o")
       .rect(w1, d2)
       .cutBlind(l4)
)

# cutout
cut = (cq.Workplane("XY")
       .circle(d4/2)
       .extrude(l5)
       .circle(d2/2)
       .cutThruAll()
       )

res = res - cut

# fillet top

res = (res
       .faces(">Z")
       .fillet(0.1)
)

# knurling

knurl = (cq.Workplane("XY")
         .polygon(nof_knurls, d3, forConstruction=True)
         .vertices()
         .circle(knurl_dia/2)
         .extrude(l1 + l3)
         )

res = res - knurl

# fillet knurls

res = (res
       .edges("|Z")
       .fillet(0.5)
       )

show_object(res)
