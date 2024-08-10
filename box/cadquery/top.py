from defs import *

# Overall height
oah = 5
# Shell thickness
th = 3
# Inserts
standoff_d = 8
insert_d = 3.5+0.25
insert_l = 5

screwpost_h = oah + 4.5

# Create inset points
inner_points = []
for p in points:
    inner_points.append(make_inset(p, th))
for i in range(len(points) - 2, 0, -1):
    inner_points.append(make_inset((-points[i][0], points[i][1]), th))

outer_spline = cq.Workplane("XY").spline(outer_points).close()

outer = outer_spline.extrude(oah).faces("<Z").fillet(1.35)

inner_spline = cq.Workplane("XY").workplane(th).spline(inner_points).close()

inner = inner_spline.extrude(oah)

# Make shell
result = outer.cut(inner)

# Screwposts

for c in screwpost_coords:
    result = result + screwpost_body(c[0], c[1], 8, screwpost_h, 0)

result = (result.faces(">Z").workplane().
          pushPoints(screwpost_coords).
          circle(3.5/2).
          cutThruAll()
          )

result = (result.faces(">Z").workplane(-oah).
          pushPoints(screwpost_coords).
          circle(6.5/2).
          cutBlind(3)
          )

show_object(result)
