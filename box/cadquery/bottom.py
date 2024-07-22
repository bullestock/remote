import copy, math

# Overall height
oah = 33.5
# Shell thickness
th = 3
# Height of standoffs for mainboard
mainboard_standoff_h = 11.6
controller_standoff_h = 5
standoff_d = 8
insert_d = 3.5
insert_l = 5

controller_standoffs = [
    (266, 99),
    (266, 146.812),
    (326, 99),
    (326, 140.97),
]
controller_offset = (
    (controller_standoffs[0][0] + controller_standoffs[2][0])/2,
    (controller_standoffs[0][1] + controller_standoffs[1][1])/2 + 69
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
mainboard_offset = (76.3675, 85 + 6.5)
mainboard_standoffs_l = []
for p in mainboard_standoffs:
    mainboard_standoffs_l.append((p[0] - mainboard_offset[0], p[1] - mainboard_offset[1]))
mainboard_standoffs_r = []
for p in mainboard_standoffs:
    mainboard_standoffs_r.append((-p[0] + mainboard_offset[0], p[1] - mainboard_offset[1]))

def sign(x):
    return -1 if x < 0 else 1

# Screwpost for corners of a box, with heat insert nut
def square_screwpost_nut(d, h, r):
    return (cq.Workplane()
            .box(d, d, h)
            .edges("|Z").fillet(r)
            .faces(">Z")
            # hole for insert
            .circle(insert_r).cutBlind(-insert_l)
            # hole for screw end
            .faces(">Z")
            .circle(insert_sr+.25).cutBlind(-3*insert_l)
            )

# Screwpost for corners of a box
def square_screwpost_body(x, y):
    return (cq.Workplane()
            .transformed(offset=(x, y, th))
            .box(10, 10, oah - th, centered=(True, True, False))
            .edges("|Z").fillet(4)
            )

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

# Make shell
result = outer.cut(inner)

# Controller standoffs

result = (result.faces("<Z").workplane(-th).
          pushPoints(controller_standoffs).
          circle(standoff_d/2).
          extrude(-controller_standoff_h)
          )

result = (result.faces("<Z").workplane(-th).
          pushPoints(controller_standoffs).
          circle(insert_d/2).
          cutBlind(-insert_l)
          )

# Mainboard standoffs, L

result = (result.faces("<Z").workplane(-th).
          tag("main_s").
          pushPoints(mainboard_standoffs_l).
          circle(standoff_d/2).
          extrude(-mainboard_standoff_h)
          )

result = (result.workplaneFromTagged("main_s").workplane(-mainboard_standoff_h).
          pushPoints(mainboard_standoffs_l).
          circle(insert_d/2).
          cutBlind(insert_l)
          )

# Mainboard standoffs, R

result = (result.faces("<Z").workplane(-th).
          pushPoints(mainboard_standoffs_r).
          circle(standoff_d/2).
          extrude(-mainboard_standoff_h)
          )

result = (result.workplaneFromTagged("main_s").workplane(-mainboard_standoff_h).
          pushPoints(mainboard_standoffs_r).
          circle(insert_d/2).
          cutBlind(insert_l)
          )

# Screwposts

screwpost_coords = [
    (0, 101),
    (71, 101),
    (-71, 101),
    (95, 20),
    (-95, 20),
    (40, -28),
    (-40, -28),
]

for c in screwpost_coords:
    result = result + square_screwpost_body(c[0], c[1])

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

# Antenna cutout
ant_d = 10.5
ant_d2 = 2*ant_d
result = (result.
          faces("<Z").workplane().
          transformed(rotate=(90, 0, 90), offset=(30 - 4.3, -100, -oah + ant_d - ant_d2/2)).
          slot2D(ant_d2, ant_d).
          cutBlind(10)
          )

# Charging port

chp_d1 = 8
chp_d2 = 15
result = (result.
          faces("<Z").workplane(centerOption='CenterOfMass').
          transformed(rotate=(90, 0, 0), offset=(0, 75, -10)).
          slot2D(chp_d2, chp_d1).
          cutBlind(10)
          )

show_object(result)
