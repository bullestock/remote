from math import cos, radians, sin
from remotedefs import *

# Assumes SolidPython is in site-packages or elsewhwere in sys.path
from solid import *
from solid.utils import *

e = 0.001
th = 2

joystick_dep_d = 45
joystick_dep_h = 5
joystick_flange_d = 54
joystick_hole_d = 45

slide_hole_w = 18
slide_hole_h = 33
slide_ring_th1 = 2
slide_ring_th2 = 5
slide_ring_h = 10 - th

# Cube centered in x/y
def c2cube(w, h, d):
    return translate([-w/2, -h/2, 0])(cube([w, h, d]))

def joystick_hole(a):
    h = down(e)(cylinder(d = 3, h = 2*joystick_dep_h+2*e))
    r = joystick_dep_d/2 + 2
    ra = radians(a)
    return translate([r*sin(ra), r*cos(ra), 0])(h)
        
def joystick_holes():
    return joystick_hole(0) + joystick_hole(120) + joystick_hole(240)

def slide_holes():
    h = down(e)(cylinder(d = 3, h = slide_ring_h+5))
    d = slide_hole_h/2 + slide_ring_th2/2
    return translate([0, -d, 0])(h) + translate([0, d, 0])(h)
