import cadquery as cq
import copy, math

def sign(x):
    return -1 if x < 0 else 1

def square_screwpost_body(x, y, th, h, oah):
    w = 10
    return (cq.Workplane()
            .transformed(offset=(x, y, th))
            .box(w, w, oah - th - h, centered=(True, True, False))
            .edges("|Z").fillet(w*0.49)
            )

def make_inset(p, th):
    x, y = p[0], p[1]
    length = math.sqrt(x*x + y*y)
    if x == 0:
        angle = math.pi/2
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

