#! /usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import division
import os
import sys
import re

# Assumes SolidPython is in site-packages or elsewhwere in sys.path
from solid import *
from solid.utils import *

SEGMENTS = 128

def assembly():
    r1 = 16
    r2 = 15
    outer_s = sphere(r = r1)
    inner_s = sphere(r = r2)
    sph = outer_s - inner_s
    tube_h = 3
    hole1 = cylinder(r = 14, h = 25)
    hole2 = cylinder(r = r2, h = 5)
    tube = cylinder(r = 16, h = tube_h) - down(1)(cylinder(r = 14, h = tube_h+2))
    block = translate([-20, -20, -20])(cube([40, 40, 20]))
    flange = translate([0, 0, 0])(cylinder(r = 20, h = 2))
    return flange + up(tube_h)(sph) + tube - block - down(1)(hole1) - down(1)(hole2)

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)
