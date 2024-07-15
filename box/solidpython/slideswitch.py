#! /usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import division
import os
import sys
import re
from math import cos, radians, sin

# Assumes SolidPython is in site-packages or elsewhwere in sys.path
from solid import *
from solid.utils import *

SEGMENTS = 32

e = 0.001

center_x = 107.500 + 2.5
center_x_pcb = (159.766 + 90.424)/2

joystick_y = -38.276 + 16
joystick_y_pcb= 217.424

joystick_x = 23.977 + 16

pcb_x_offset = center_x_pcb - center_x
pcb_y_offset = joystick_y_pcb - joystick_y

# Rounding radius
rr = 3

# Shell thickness
th = 2

def cylinder_at(x, y, d, h):
    return translate([x, y, 0])(cylinder(d = d, h = h))

# Cube centered in x/y
def c2cube(w, h, d):
    return translate([-w/2, -h/2, 0])(cube([w, h, d]))

th = 2
slide_c_d = 10 - th

def slide_upper_cutout():
    slide_c_w = 10
    slide_c_h = 25
    slide_c1 = c2cube(slide_c_w, slide_c_h, e)
    slide_c2 = up(slide_c_d - th)(c2cube(slide_c_w + slide_c_d, slide_c_h + slide_c_d, e))
    return hull()(slide_c1 + slide_c2)

def slide_lower_cutout():
    slide_c_w = 12
    slide_c_h = 27
    slide_c1 = c2cube(slide_c_w, slide_c_h, e)
    slide_c2 = up(slide_c_d)(c2cube(slide_c_w + slide_c_d + th, slide_c_h + slide_c_d + th, e))
    return hull()(slide_c1 + slide_c2)

def assembly():
    flange = up(slide_c_d - th + e)(c2cube(23.5, 41.5, th-e))
    hole = (c2cube(5, 12, 3))

    #meas = (c2cube(18, 33, 3))
    return flange + slide_lower_cutout() - up(2+e)(slide_upper_cutout()) - down(e)(hole) #+ up(10.5)(meas)

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

