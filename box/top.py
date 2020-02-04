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
from joystickcover import *

SEGMENTS = 32

e = 0.001

center_x = 107.500 + 2.5
center_x_pcb = (159.766 + 90.424)/2

joystick_y = -38.276 + 16
joystick_y_pcb= 217.424

joystick_x = 23.977 + 16

pcb_x_offset = center_x_pcb - center_x
pcb_y_offset = joystick_y_pcb - joystick_y

rr=3

# Overall height
oah = 40
hole_h = 50
pcb_z = 10

def cylinder_at(x, y, d, h):
    return translate([x, y, 0])(cylinder(d = d, h = h))

# Cube centered in x/y
def c2cube(w, h, d):
    return translate([-w/2, -h/2, 0])(cube([w, h, d]))

def pot(x, y):
    return translate([x, y, 0])(cylinder(d = 8, h = hole_h))

def pushbutton(x, y):
    return translate([x, y, 0])(cylinder(d = 14.25, h = hole_h))

def joystick_h(x, y):
    return translate([x, y, 0])(cylinder(d = 30, h = oah+rr+e))

def toggle(x, y):
    return translate([x, y, 0])(cylinder(d = 6, h = hole_h))

def holes():
    pot1 = pot(-48.630 + 6.552/2, 11.340 + 6.552/2)
    pot2 = pot(48.630 - 6.552/2, 11.340 + 6.552/2)
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
    slide = translate([- 5/2, joystick_y - 1 - 12/2, 0])(cube([5, 12, hole_h]))
    # 2 mm right of center
    display_w = 34
    display_bottom_y = joystick_y + 25.9 + 37.85
    display = translate([- display_w/2 + 2, display_bottom_y, 0])(cube([display_w, 16, hole_h]))
    return holes1 + holes2 + slide + display

def pcbsupport(x, y):
    return cylinder_at(x, y, 3, pcb_z)

def pcbmounts():
    p1 = pcbsupport(42, 63.428 + 12.5/2)
    return p1

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
    bot_brim = extrude_along_path(shape_pts = bot_shape, path_pts = points)
    return hull()(bot_brim, top_brim)

def void():
    rr = 2
    th = 2
    points = inner_shape()
    top_shape = circle_p(num_points=10, rad=rr)
    top_brim = up(oah - 2*rr - th)(extrude_along_path(shape_pts = top_shape, path_pts = points))
    bot_shape = square_p(rad=rr)
    bot_brim = down(1)(extrude_along_path(shape_pts = bot_shape, path_pts = points))
    return hull()(bot_brim, top_brim)

def slide_upper_cutout():
    slide_c_w = 10
    slide_c_h = 25
    slide_c_d = 10
    slide_c1 = c2cube(slide_c_w, slide_c_h, .1)
    slide_c2 = up(slide_c_d)(c2cube(slide_c_w + slide_c_d, slide_c_h + slide_c_d, .1))
    return translate([0, joystick_y - 1, oah - slide_c_d])(hull()(slide_c1 + slide_c2))

def slide_lower_cutout():
    slide_c_w = 12
    slide_c_h = 27
    slide_c_d = 12
    slide_c1 = c2cube(slide_c_w, slide_c_h, .1)
    slide_c2 = up(slide_c_d)(c2cube(slide_c_w + slide_c_d, slide_c_h + slide_c_d, .1))
    return translate([0, joystick_y - 1, oah - slide_c_d])(hull()(slide_c1 + slide_c2))

def assembly():
    allholes = holes()
    hollow = void() - slide_lower_cutout()
    joystick_z = oah
    jcovers = joystick_cover(-joystick_x, joystick_y, joystick_z) + joystick_cover(joystick_x, joystick_y, joystick_z)
    return shell() + jcovers - hollow - down(1)(allholes) - slide_upper_cutout()


if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

