from cadquery import exporters

# Overall height
oah = 27
hole_h = 50
pcb_z = 10
th = 3

# Half of the shape
points = [ (0, -35),
           (20, -35), 
           (30, -35), 
           (80, -20), 
           (100, 0), 
           (100, 35), 
           (77, 100), 
           (72, 108),
           (70, 108),
           (60, 108),
           (50, 108),
           (0, 108) ]

# Add mirrored half
outer_points = points
for i in range(len(points) - 2, 0, -1):
    outer_points.append((-points[i][0], points[i][1]))

# Create inset points
inner_points = []
for p in points:
    inner_points.append((p[0], p[1]))
for i in range(len(points) - 2, 0, -1):
    inner_points.append((-points[i][0], points[i][1]))

outer_spline = cq.Workplane("XY").spline(outer_points).close()

outer = outer_spline.extrude(oah).faces("<Z").fillet(1.35)

#inner_spline = cq.Workplane("XY").workplane(th).spline(inner_points).close()

#inner = inner_spline.extrude(oah).faces("<Z").fillet(1)

result = outer#.cut(inner)

show_object(result)
