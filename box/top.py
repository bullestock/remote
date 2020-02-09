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

slide_offset = -2.5

# Rounding radius
rr = 3

# Shell thickness
th = 2

joystick_dep_h = 8

# Overall height, minimum 12
oah = 12
hole_h = 50
pcb_z = 10

def cylinder_at(x, y, d, h):
    return translate([x, y, oah - th - 17])(cylinder(d = d, h = h))

# Cube centered in x/y
def c2cube(w, h, d):
    return translate([-w/2, -h/2, 0])(cube([w, h, d]))

def pot(x, y):
    return translate([x, y, 0])(cylinder(d = 8, h = hole_h))

def pushbutton(x, y):
    return translate([x, y, 0])(cylinder(d = 14.5, h = hole_h))

def joystick_h(x, y):
    return translate([x, y, 0])(cylinder(d = 50, h = hole_h))

def toggle(x, y):
    return translate([x, y, 0])(cylinder(d = 6.25, h = hole_h))

def holes():
    pot1 = pot(-48.630 + 6.552/2, 11 + 6.552/2)
    pot2 = pot(49 - 6.552/2, 11 + 6.552/2)
    pb1l = pushbutton(-76.619 + 12.5/2, 22.774 + 12.5/2)
    pb2l = pushbutton(-82.690 + 12.5/2, 1.608 + 12.5/2)
    pb3l = pushbutton(-89.273 + 12.5/2, -19.559 + 12.5/2)
    pb1r = pushbutton(76.619 - 12.5/2, 22.774 + 12.5/2)
    pb2r = pushbutton(82.690 - 12.5/2, 1.608 + 12.5/2)
    pb3r = pushbutton(89.273 - 12.5/2, -19.559 + 12.5/2)
    jlh = joystick_h(-joystick_x, joystick_y)
    jrh = joystick_h(joystick_x, joystick_y)
    toggle1 = toggle(-69.152 + 3, 51.603 + 3)
    toggle2 = toggle(-49.736 + 3, 51.603 + 3)
    toggle3 = toggle(49.736 - 3, 51.603 + 3)
    toggle4 = toggle(69.152 - 3, 51.603 + 3)
    holes1 = pot1 + pot2 + pb1l + pb2l + pb3l + pb1r + pb2r + pb3r
    holes2 = jlh + jrh + toggle1 + toggle2 + toggle3 + toggle4
    slide = translate([0, joystick_y - slide_offset, 0])(c2cube(18, 33, hole_h))
    # 2 mm right of center
    display_w = 34
    display_bottom_y = joystick_y + 25.9 + 37.85
    display = translate([- display_w/2 + 2, display_bottom_y, 0])(cube([display_w, 16, hole_h]))
    return holes1 + holes2 + slide + display

def map(y):
    return 195.5 - y 

def pcbsupport(x, y):
    return cylinder_at(x - pcb_x_offset, map(y), 4, 18)

def screwstud(x, y):
    o = translate([x, y, oah - th - 21])(cylinder(d = 8, h = 21))
    i = translate([x, y, oah - th - 22])(cylinder(d = 2, h = 10))
    return o - i

def screwstud2(x, y):
    d = 2
    o = translate([x, y, 0])(cylinder(d = 8, h = oah-d))
    i = translate([x, y, -3])(cylinder(d = 2, h = oah))
    return o + hole()(i)

def pcbmounts():
    p1 = pcbsupport(52, 217)
    p2 = pcbsupport(200, 217)
    p3 = pcbsupport(61, 180)
    p4 = pcbsupport(193, 180)
    p5 = pcbsupport(54, 153)
    p6 = pcbsupport(193, 153)
    p7 = pcbsupport(86, 149)
    p8 = pcbsupport(162, 149)
    return p1+p2+p3+p4+p5+p6+p7+p8

def screwstuds():
    #s1 = screwstud(84.5, 25) + screwstud(-84.5, 25)
    s2 = screwstud(92, -5) + screwstud(-89, 10)
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

def slide_flange():
    inner = down(e)(c2cube(24, 42, th+2*e))
    outer = c2cube(24+2*th, 42+2*th, th)
    return translate([0, joystick_y - slide_offset, oah-2*th-e])(outer - inner)

def slide_flange_cut():
    cut = down(e)(c2cube(30, 32, th+2*e))
    return translate([0, joystick_y - slide_offset, oah-2*th-e])(cut)

def joystick_flange(x, y):
    id = 55
    inner = down(e)(cylinder(d = id, h = th + 2*e))
    outer = cylinder(d = id+2*th, h = th)
    return translate([x, y, oah - 2*th-e])(outer - inner)

def assembly():
    allholes = holes()
    joystick_z = oah - joystick_dep_h
    jbz = oah - joystick_dep_h
    outer = shell()
    hollow = void()
    jf = joystick_flange(-joystick_x, joystick_y) + joystick_flange(joystick_x, joystick_y)
    return outer - hollow + translate([0, -5, 0])(jf + slide_flange() + pcbmounts()) - translate([0, -5, 0])(down(1)(allholes) + slide_flange_cut()) + screwstuds()
    #return pcbmounts()

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

