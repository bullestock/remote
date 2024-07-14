from cadquery import exporters
import copy, math

# Overall height
oah = 27
hole_h = 50
pcb_z = 10
th = 3

controller_standoffs = [
    (266, 99),
    (266, 146.812),
    (326, 99),
    (326, 140.97),
]
controller_offset = (
    (controller_standoffs[0][0] + controller_standoffs[2][0])/2,
    (controller_standoffs[0][1] + controller_standoffs[1][1])/2 + 75
)
controller_standoffs_new = []
for p in controller_standoffs:
    controller_standoffs_new.append((p[0] - controller_offset[0], p[1] - controller_offset[1]))
controller_standoffs = controller_standoffs_new

mainboard_standoffs = [
    (20, 25),
    (0, 60),
    (5, 37),
    (20, 0),
]

def sign(x):
    return -1 if x < 0 else 1

def make_inset(p):
    x, y = p[0], p[1]
    length = math.sqrt(x*x + y*y)
    if x == 0:
        angle = math.pi/2 # if y > 0 else -math.pi/2
    else:
        angle = math.atan(abs(y/x))
    length -= th
    x = math.cos(angle) * sign(x) * length
    y = math.sin(angle) * sign(y) * length
    return (x, y)
    
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
outer_points = copy.copy(points) # break reference
for i in range(len(points) - 2, 0, -1):
    outer_points.append((-points[i][0], points[i][1]))

# Create inset points
inner_points = []
for p in points:
    inner_points.append(make_inset(p))
for i in range(len(points) - 2, 0, -1):
    inner_points.append(make_inset((-points[i][0], points[i][1])))

outer_spline = cq.Workplane("XY").spline(outer_points).close()

outer = outer_spline.extrude(oah).faces("<Z").fillet(1.35)

inner_spline = cq.Workplane("XY").workplane(th).spline(inner_points).close()

inner = inner_spline.extrude(oah)

result = outer.cut(inner)

result = (result.faces("<Z").workplane(-th).
          pushPoints(controller_standoffs).
          circle(3).
          extrude(-5)
          )

result = (result.faces("<Z").workplane(-th).
          pushPoints(controller_standoffs).
          circle(3/2).
          cutBlind(-5)
          )

show_object(result)
