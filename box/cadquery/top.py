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

result = (result.faces(">Z").workplane()
          .tag("main_s")
          .pushPoints(screwpost_coords)
          .circle(3.5/2)
          .cutBlind(-insert_l)
          )

# Pushbutton holes

pushbutton_d = 14.5
pushbuttons_c = [ (24.6 + 14.5/2, 40 + 14.5/2),
                  (18.6 + 14.5/2, 61 + 14.5/2),
                  (12.6 + 14.5/2, 81 + 14.5/2),
                ]
pushbuttons = []
ox = 205.65/2
oy = 108
for c in pushbuttons_c:
    pushbuttons.append(((c[0] - ox), -c[1] + oy))
    pushbuttons.append((-(c[0] - ox), -c[1] + oy))

result = (result
          .workplaneFromTagged("main_s")
          .pushPoints(pushbuttons)
          .circle(pushbutton_d/2)
          .cutThruAll()
          )

# Toggle switch

toggle_d = 6
toggles_c = [ (33.85 + 3, 18.35 + 3),
              (53.85 + 3, 18.35 + 3),
             ]
toggles = []
for c in toggles_c:
    toggles.append(((c[0] - ox), -c[1] + oy))
    toggles.append((-(c[0] - ox), -c[1] + oy))

result = (result
          .workplaneFromTagged("main_s")
          .pushPoints(toggles)
          .circle(toggle_d/2)
          .cutThruAll()
          )

# Pots

pot_d = 7
pots_c = [ (54.85 + 7/2, 58.75 + 7/2) ]
pots = []
for c in pots_c:
    pots.append(((c[0] - ox), -c[1] + oy))
    pots.append((-(c[0] - ox), -c[1] + oy))

result = (result
          .workplaneFromTagged("main_s")
          .pushPoints(pots)
          .circle(pot_d/2)
          .cutThruAll()
          )

# Joystick large hole

stick_d = 37.5
sticks_c = [ (44.5 + 37.5/2, 80 + 37.5/2) ]
sticks = []
for c in sticks_c:
    sticks.append(((c[0] - ox), -c[1] + oy))
    sticks.append((-(c[0] - ox), -c[1] + oy))

result = (result
          .workplaneFromTagged("main_s")
          .pushPoints(sticks)
          .circle(stick_d/2)
          .cutThruAll()
          )

# Joystick screw hole

stick_s_d = 2
sticks_s_c = [ (44.75 + 1, 76.6 + 1),
               (44.75 + 1, 118.6 + 1),
               (79.75 + 1, 76.6 + 1),
               (79.75 + 1, 118.6 + 1),
              ]
sticks_s = []
for c in sticks_s_c:
    sticks_s.append(((c[0] - ox), -c[1] + oy))
    sticks_s.append((-(c[0] - ox), -c[1] + oy))

result = (result
          .workplaneFromTagged("main_s")
          .pushPoints(sticks_s)
          .circle(stick_s_d/2)
          .cutThruAll()
          )


show_object(result)
