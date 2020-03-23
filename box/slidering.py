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

e = 0.001

# Shell thickness
th = 2

def assembly():
    outer = c2cube(slide_hole_w + 2*slide_ring_th1, slide_hole_h + 2*slide_ring_th2, slide_ring_h)
    h1 = c2cube(slide_hole_w, slide_hole_h, e)
    d = slide_hole_h/3
    h2 = c2cube(slide_hole_w-d, slide_hole_h-d, e)
    hole = hull()(up(slide_ring_h+e)(h1) + h2)
    return outer - down(e)(hole) - slide_holes()

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

# Local Variables:
# compile-command: "python slidering.py"
# End:
