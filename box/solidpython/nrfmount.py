#! /usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import division
import os
import sys
import re

# Assumes SolidPython is in site-packages or elsewhwere in sys.path
from solid import *
from solid.utils import *

SEGMENTS = 32

w1 = 15.5
h1 = 8.5 + 2
l1 = 15
h2 = 3
l2 = 14

e = 0.001

def block():
    return translate([-w1/2, 0, 0])(cube([w1, l1, h1]) -
                                    # hole for magnet
                                    translate([w1/2 - 0, l1/2 - 0, -1])(cylinder(h = 3, d = 10.5)))

def tab():
    # screw holes for 1 mm screws
    s = hole()(cylinder(h = 5, d = 1))
    d2 = 12
    sw = 8
    return translate([-w1/2, 0, 0])(cube([w1, l2, h2]) -
                                    translate([(sw-1)/2, 4, -1])(cube([sw+1, 15, h2+2])) +
                                    translate([w1/2 - d2/2, l2 - 1.5, 0])(s) +
                                    translate([w1/2 + d2/2, l2 - 1.5, 0])(s))

def assembly():
    b = block()
    return b + translate([0, l1, h1 - h2])(tab())

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

# Local Variables:
# compile-command: "python nrfmount.py"
# End:
