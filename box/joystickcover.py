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

joystick_dep_d = 45
joystick_dep_h = 5
th = 2

def joystick_cover(x, y, z):
    r1 = 16
    r2 = 15
    outer_s = sphere(r = r1)
    inner_s = sphere(r = r2)
    # Spherical shell
    sph = outer_s - inner_s
    # Top opening
    hole1 = cylinder(r = 12, h = 20)
    # Cube to cut off bottom
    block = translate([-20, -20, -20])(cube([40, 40, 20]))
    return translate([x, y, z])(sph) - block - down(1)(hole1)

def joystick_hollow1(x, y):
    return translate([x, y, 1.5*th + 3*e])(cylinder(d1 = 35, d2 = joystick_dep_d, h = joystick_dep_h))

def joystick_hollow2(x, y):
    d = 3
    return translate([x, y, e])(cylinder(d1 = 35+d, d2 = 50+d, h = joystick_dep_h+d))

def assembly():
    bottom = cylinder(d = 35, h = th)
    hollow1 = joystick_hollow1(0, 0)
    hollow2 = joystick_hollow2(0, 0)
    hole = down(e)(cylinder(d = 28, h = 1.6*th+2*e))
    flange = up(joystick_dep_h + .5*th)(cylinder(d = 54, h = th) - down(e)(cylinder(d = 50, h = th+2*e)))

    return hollow2 - hollow1 + joystick_cover(0, 0, -2.5) + bottom - hole + flange

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

# Local Variables:
# compile-command: "python joystickcover.py"
# End:
