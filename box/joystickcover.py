#! /usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import division
import os
import sys
import re
from math import cos, radians, sin
from remotedefs import *

# Assumes SolidPython is in site-packages or elsewhwere in sys.path
from solid import *
from solid.utils import *

SEGMENTS = 32

def joystick_opening(x, y, z):
    r2 = 15
    inner_s = hole()(sphere(r = r2))
    return translate([x, y, z])(hole()(inner_s))

def joystick_hollow1(x, y):
    cyl = up(e)(cylinder(d = joystick_dep_d, h = joystick_dep_h))
    cone = up(0)(cylinder(d2 = 24, d1 = joystick_dep_d, h = joystick_dep_h))
    return translate([x, y, th+e])(cyl - cone)

def joystick_hollow2(x, y):
    d = th-e
    return translate([x, y, e])(cylinder(d = joystick_dep_d-e, h = joystick_dep_h+d))

def assembly():
    hollow1 = joystick_hollow1(0, 0)
    hollow2 = joystick_hollow2(0, 0)
    flange = cylinder(d = joystick_flange_d, h = th)
    return hollow2 - hollow1 + joystick_opening(0, 0, -3) + flange - joystick_holes()

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

# Local Variables:
# compile-command: "python joystickcover.py"
# End:
