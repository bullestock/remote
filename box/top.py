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

slide_offset = 0

# Rounding radius
rr = 3

# Shell thickness
th = 2

joystick_dep_h = 8

# Overall height, minimum 12
oah = 12
hole_h = 50
pcb_z = 10

# Cube centered in x/y
def c2cube(w, h, d):
    return translate([-w/2, -h/2, 0])(cube([w, h, d]))

def pot(x, y):
    return translate([x, y, 0])(cylinder(d = 8, h = hole_h))

def pushbutton(x, y):
    return translate([x, y, 0])(cylinder(d = 14.5, h = hole_h))

def joystick_h(x, y):
    return translate([x, y, 0])(hole()(cylinder(d = joystick_hole_d, h = hole_h)) + joystick_holes())

def toggle(x, y):
    return translate([x, y, 0])(cylinder(d = 6.25, h = hole_h))

def holes():
    pot1 = pot(-48.630 + 6.552/2, 12 + 6.552/2)
    pot2 = pot(49 - 6.552/2, 12 + 6.552/2)
    pb1l = pushbutton(-76.619 + 12.5/2, 22.774 + 12.5/2)
    pb2l = pushbutton(-82.690 + 12.5/2, 1.608 + 12.5/2)
    pb3l = pushbutton(-89.273 + 12.5/2, -19.559 + 12.5/2)
    pb1r = pushbutton(76.619 - 12.5/2, 22.774 + 12.5/2)
    pb2r = pushbutton(82.690 - 12.5/2, 1.608 + 12.5/2)
    pb3r = pushbutton(89.273 - 12.5/2, -19.559 + 12.5/2)
    toggle1 = toggle(-69.152 + 3, 51.603 + 3)
    toggle2 = toggle(-49.736 + 3, 51.603 + 3)
    toggle3 = toggle(49.736 - 3, 51.603 + 3)
    toggle4 = toggle(69.152 - 3, 51.603 + 3)
    holes1 = pot1 + pot2 + pb1l + pb2l + pb3l + pb1r + pb2r + pb3r
    holes2 = toggle1 + toggle2 + toggle3 + toggle4
    slide = translate([0, joystick_y - slide_offset, 0])(c2cube(slide_hole_w, slide_hole_h, hole_h))
    display_w = 34
    display_h = 16
    display_bottom_y = joystick_y + 25.9 + 37.85
    display = translate([- display_w/2, display_bottom_y, 0])(cube([display_w, display_h, hole_h]))
    cw = 3
    w = display_w + 2*cw
    display2 = translate([-w/2, display_bottom_y - cw, -hole_h+oah+0.5])(cube([w, display_h + 2*cw, hole_h]))
    return holes1 + holes2 + slide + display + display2

def map(y):
    return 195.5 - y 

def pcbsupport(x, y):
    z = -8
    return translate([x - pcb_x_offset, map(y), z])(cylinder(d1 = 5, d2 = 8, h = oah - z - e))

def screwstud(x, y):
    # Extends 10 mm
    ext = 10
    o = translate([x, y, -ext])(cylinder(d = 8, h = ext + oah - th + e))
    i = translate([x, y, -ext-1])(cylinder(d = insert_d, h = 6))
    return o - i

def screwstud2(x, y):
    d = 2
    o = translate([x, y, 0])(cylinder(d = 8, h = oah-d))
    i = translate([x, y, -3])(cylinder(d = 2, h = oah))
    return o + hole()(i)

def pcbmounts():
    # Bottom left pushbutton
    p1 = pcbsupport(52, 217)
    p2 = pcbsupport(200, 217)
    # Between top left and middle left pushbutton
    p3 = pcbsupport(61, 180)
    p4 = pcbsupport(193, 180)
    # Above top left pushbutton
    p5 = pcbsupport(54, 153)
    p6 = pcbsupport(193, 153)
    # Below toggle 2 from left
    p7 = pcbsupport(86, 149)
    # Toggle 3 from left
    p8 = pcbsupport(162+5, 149-18)
    return p1+p2+p3+p4+p5+p6+p7+p8

def screwstuds():
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

def assembly():
    allholes = holes()
    joystick_z = oah - joystick_dep_h
    jbz = oah - joystick_dep_h
    outer = shell()
    hollow = void()
    jlh = joystick_h(-joystick_x, joystick_y)
    jrh = joystick_h(joystick_x, joystick_y)
    studs = translate([0, joystick_y - slide_offset, -1])(slide_holes()) + pcbmounts()
    antenna1 = translate([antenna_x, antenna_y, 3.25])(rotate([90, 0, 0])(cylinder(d = 10.5, h = 20)))
    antenna2 = translate([antenna_x, antenna_y, 0])(rotate([90, 0, 0])(cylinder(d = 10.5, h = 20)))
    return outer - hollow - back(5)(jlh - jrh) + translate([0, -5, 0])(studs) - \
        translate([0, -5, 0])(down(1)(allholes)) + screwstuds() - hull()(antenna1 + antenna2)
    #return pcbmounts()

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

# Local Variables:
# compile-command: "python top.py"
# End:
