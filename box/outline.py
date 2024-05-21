from cadquery import exporters

points1 = [ (0, -35),
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

points = points1
for i in range(len(points1) - 2, 0, -1):
    log(i)
    points.append((-points1[i][0], points1[i][1]))

log(points)

spline_path = cq.Workplane("XY").spline(points).close()

#solid = cq.Workplane("YZ").rect(10, 12).sweep(spline_path)

#show_object(solid)
show_object(spline_path)

exporters.exportDXF(spline_path, "outline.dxf")
