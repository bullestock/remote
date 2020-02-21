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

def assembly():
    return cylinder(d = joystick_flange_d, h = th) - down(e)(cylinder(d = joystick_dep_d, h = th+2*e)) - joystick_holes()

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

# Local Variables:
# compile-command: "python joystickspacer.py"
# End:
