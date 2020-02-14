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

from outline import *

SEGMENTS = 32

e = 0.001

center_x = 107.500 + 2.5
center_x_pcb = (159.766 + 90.424)/2

joystick_y = -38.276 + 16
joystick_y_pcb = 217.424

joystick_x = 23.977 + 16
joystick_x_pcb = 166.624

pcb_x_offset = joystick_x_pcb - joystick_x
pcb_y_offset = joystick_y_pcb - joystick_y

# Rounding radius
rr = 3

# Shell thickness
th = 2

# Overall height, minimum 12
oah = 20
hole_h = 50
pcb_z = 10

def cylinder_at(x, y, d, h):
    return translate([x, y, oah - th - 17])(cylinder(d = d, h = h))

# Cube centered in x/y
def c2cube(w, h, d):
    return translate([-w/2, -h/2, 0])(cube([w, h, d]))

def holes():
    return cylinder_at(0, 0, 0, 0)

def map(y):
    return 195.5 - y 

def pcbsupport(x, y):
    return cylinder_at(x - pcb_x_offset, map(y), 4, 18)

def screwstud(x, y):
    o = translate([x, y, oah - th - 7])(cylinder(d = 8, h = 8))
    i = translate([x, y, oah - th - 8])(cylinder(d = 3, h = 12))
    recess = translate([x, y, oah - 3.5])(cylinder(d = 6, h = 10))
    return o - hole()(i + recess)

def screwstud2(x, y):
    d = 2
    o = translate([x, y, 0])(cylinder(d = 8, h = oah-d))
    i = translate([x, y, -1])(cylinder(d = 3, h = oah + 2))
    recess = translate([x, y, oah - 3.5])(cylinder(d = 6, h = 10))
    return o + hole()(i + recess)

def pcbmounts():
    #!!
    p1 = pcbsupport(52, 217)
    return p1

def screwstuds():
    s2 = screwstud(-92, -5) + screwstud(89, 10)
    y = 67
    s3 = screwstud2(67, y) + screwstud2(-67, y)
    s4 = screwstud2(0, y)
    s5 = screwstud(30, -63) + screwstud(-30, -63)
    return s2 + s3 + s4 + s5

def circle_p(num_points=10, rad=2):
    circle_pts = []
    for i in range(2 * num_points):
        angle = radians(360 / (2 * num_points) * i)
        circle_pts.append(Point3(rad * cos(angle), rad * sin(angle), 0))
    return circle_pts

def square_p(rad=2):
    square_pts = []
    square_pts.append(Point3(-rad, -rad, 0))
    square_pts.append(Point3(rad, -rad, 0))
    square_pts.append(Point3(rad, rad, 0))
    square_pts.append(Point3(-rad, rad, 0))
    return square_pts

def outer_shape():
    c = []
    points = []
    scale = 0.025
    max_x = 0
    max_y = 0
    is_x = True
    for val in outline:
        if is_x:
            x = val*scale
            if x > max_x:
                max_x = x
            is_x = False
        else:
            y = val*scale
            if y > max_y:
                max_y = y
            is_x = True
    x_offset = max_x/2
    y_offset = max_y/2
    is_x = True
    for val in outline:
        if is_x:
            x = val*scale
            c.append(x - x_offset)
            is_x = False
        else:
            y = val*scale
            c.append(y - y_offset)
            points.append(c)
            c = []
            is_x = True
    return points

def inner_shape():
    c = []
    points = []
    scale = 0.025
    max_x = 0
    max_y = 0
    is_x = True
    for val in small_outline:
        if is_x:
            x = val*scale
            if x > max_x:
                max_x = x
            is_x = False
        else:
            y = val*scale
            if y > max_y:
                max_y = y
            is_x = True
    x_offset = max_x/2
    y_offset = max_y/2
    is_x = True
    for val in small_outline:
        if is_x:
            x = val*scale
            c.append(x - x_offset)
            is_x = False
        else:
            y = val*scale
            c.append(y - y_offset)
            points.append(c)
            c = []
            is_x = True
    return points

def shell():
    points = outer_shape()
    top_shape = circle_p(num_points=10, rad=rr)
    top_brim = up(oah - rr)(extrude_along_path(shape_pts = top_shape, path_pts = points))
    bot_shape = square_p(rad=rr)
    bot_brim = up(rr)(extrude_along_path(shape_pts = bot_shape, path_pts = points))
    return hull()(bot_brim, top_brim)

def void():
    rr = 2
    points = inner_shape()
    top_shape = circle_p(num_points=10, rad=rr)
    top_brim = up(oah - rr - th)(extrude_along_path(shape_pts = top_shape, path_pts = points))
    bot_shape = square_p(rad=rr)
    bot_brim = down(1)(extrude_along_path(shape_pts = bot_shape, path_pts = points))
    return hull()(bot_brim, top_brim)

def assembly():
    allholes = holes()
    outer = shell()
    hollow = void()
    return outer - hollow + translate([0, -5, 0])(pcbmounts()) - translate([0, -5, 0])(down(1)(allholes)) + screwstuds()

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

