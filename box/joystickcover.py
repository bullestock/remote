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
    a = sph - block - down(1)(hole1)
    return translate([x, y, z])(a)
