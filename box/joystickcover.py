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
    outer_s = sphere(r = 16)
    inner_s = sphere(r = 14)
    hole = cylinder(r = 13, h = 20)
    block = translate([-20, -20, -20])(cube([40, 40, 20]))
    flange = translate([0, 0, 0])(cylinder(r = 20, h = 2))
    return flange + outer_s - inner_s - block - hole

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)
