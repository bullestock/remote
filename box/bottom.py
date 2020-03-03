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

include_placeholders = False#True

e = 0.001

pcb_h = 8 + 4 - 1.2

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
th = 4

# Overall height
oah = 22
hole_h = 50
pcb_z = 10

def cylinder_at(x, y, d, h):
    return translate([x, y, oah-h-th])(cylinder(d = d, h = h))

# Cube centered in x/y
def c2cube(w, h, d):
    return translate([-w/2, -h/2, 0])(cube([w, h, d]))

def holes():
    return cylinder_at(0, 0, 0, 0)

def map_y(y):
    return 195.5 - y 

def map_x(x):
    return -(x - pcb_x_offset)

def pcbsupport(x, y):
    z = 3
    return translate([map_x(x), map_y(y), oah-pcb_h-th])(cylinder(d1 = 5, d2 = 8, h = pcb_h))

def pcbsupport2(x, y):
    z = 3
    c1 = cylinder(d = 5, h = pcb_h)
    return translate([map_x(x), map_y(y), oah-pcb_h-th])(hull()(c1 + forward(8)(c1)))

# Recessed
def screwstud(x, y):
    # Top extends 10.6
    xh = 10.6
    o = translate([x, y, xh])(cylinder(d = 8, h = oah - xh - th + e))
    i = translate([x, y, xh - 1])(cylinder(d = 3, h = 12))
    recess = translate([x, y, oah - 3.5])(cylinder(d = 6, h = 10))
    return o - hole()(i + recess)

# Flush
def screwstud2(x, y):
    d = 2
    o = translate([x, y, 0])(cylinder(d = 8, h = oah-d))
    i = translate([x, y, -1])(cylinder(d = 3, h = oah + 2))
    recess = translate([x, y, oah - 3.5])(cylinder(d = 6, h = 10))
    return o + hole()(i + recess)

# for PCB, with insert
def screwstud3(x, y):
    o = translate([map_x(x), map_y(y), oah - th - pcb_h])(cylinder(d1 = 8, d2 = 12, h = pcb_h + 1))
    i = translate([map_x(x), map_y(y), -th])(cylinder(d = 2, h = oah))
    i2 = translate([map_x(x), map_y(y), oah - th - pcb_h - 1])(cylinder(d = insert_d, h = 6))
    return o + hole()(i + i2)

# for PCB, zero ref'd, variable height
def screwstud4(x, y, h):
    o = translate([x, y, oah - th - h])(cylinder(d1 = 8, d2 = 12, h = h + 1))
    i = translate([x, y, oah - th - h - 1])(cylinder(d = insert_d, h = 4))
    return o + hole()(i)

def pcbmounts():
    # Left of stepup
    p1 = pcbsupport(48, 176)
    p2 = pcbsupport(44, 198)
    p3 = pcbsupport(202, 176)
    p4 = pcbsupport(203, 198)
    p5 = pcbsupport(54, 140)
    # Left of charger
    p6 = pcbsupport(63, 130)
    # Right of charger
    p7 = pcbsupport2(89, 140)
    p8 = pcbsupport(80, 192)
    p9 = pcbsupport(86, 217)
    p10 = pcbsupport(126, 204)
    p11 = pcbsupport(126, 228)
    p12 = pcbsupport(166, 217)
    p13 = pcbsupport(166, 135)
    p14 = pcbsupport(199, 140)
    p15 = pcbsupport(182, 132)
    s1 = screwstud3(60.452, 156.972)
    s2 = screwstud3(52.578, 225.552)
    s3 = screwstud3(176.53, 166.878)
    s4 = screwstud3(191.008, 225.552)
    return s1 + s2 + s3 + s4 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9 + p10 + p11 + p12 + p13 + p14 + p15

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

def lipo():
    l = c2cube(49, 30, 8) - c2cube(45, 26, 10) - translate([22, 12, 0])(c2cube(10, 10, 10))
    if include_placeholders:
        l = l + color([0.5, 0, 1, 0.2])(c2cube(44, 26, 8))
    return up(oah - th - 8 + e)(l)

def controller():
    w = 70
    d = 61.5
    h = 2
    studs = screwstud4(w/2-5, d/2-6.4, h) + screwstud4(w/2-65, d/2-6.4, h) + screwstud4(w/2-5, d/2-54.1, h) + screwstud4(w/2-65, d/2-48.25, h)
    if include_placeholders:
        return studs + color([0, 0, 1, 0.2])(c2cube(w, d, 15))
    else:
        return studs
    
def charger():
    ch = 29
    c = color([0, 1, 1, 0.2])(c2cube(18, ch, 4))
    hw = 15
    hh = 8
    c1 = cylinder(d = hh, h = 10)
    chole = translate([-hh/2, ch - 5, -0.5])(hull()(rotate([90, 0, 0])(c1 + translate([hw-hh, 0])(c1))))
    inner = translate([0, 1, -1])(c2cube(17.5, 28, 5))
    iw = hw+7
    outer = c2cube(iw, 29.5, 4)
    ridge = up(1)(outer - inner) + translate([0, 12, 3])(c2cube(15, 2, 2)) + translate([0, -5, 3])(c2cube(15, 2, 2))
    pin = down(1)(c2cube(1.5, 4, 2))
    pins = translate([iw/2-.5, ch/2-2])(pin) + translate([-iw/2+.5, ch/2-2])(pin) + \
        translate([iw/2-.5, -(ch/2-9)])(pin) + translate([-iw/2+.5, -(ch/2-9)])(pin)
    a = ridge + up(-3)(hole()(chole)) + pins
    if include_placeholders:
        a = a + c
    return up(oah - th - 4 + e)(a)
    
def stepup():
    s = c2cube(41, 21, 4) - c2cube(38, 18, 5)
    if include_placeholders:
        s = s + color([1, .5, .5, 0.2])(c2cube(38, 18, 7))
    return up(oah - th - 4 + e)(s)
    
def assembly():
    allholes = holes()
    outer = shell()
    hollow = void()
    charger_x = 50
    antenna_tab = c2cube(10, 4, 10) - hole()(forward(10)(rotate([90, 0, 0])(cylinder(d = 10.5, h = 20))))
    all = outer - hollow + translate([0, -5, 0])(pcbmounts()) - translate([0, -5, 0])(down(1)(allholes)) + screwstuds() + \
        translate([0, 30, 0])(controller()) + \
        translate([35, -45, 0])(lipo()) + \
        translate([55, 18, 0])(stepup()) + \
        translate([charger_x, 55-4, 0])(charger()) + \
        translate([-antenna_x, antenna_y, -3])(antenna_tab)
    # Charger submodule test
    #allcube = down(2)(c2cube(250, 150, 50))
    #chargercube = translate([charger_x, 55, 5])(c2cube(20, 38, 20))
    #return all - (allcube - chargercube)
    return all

if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

# Local Variables:
# compile-command: "python bottom.py"
# End:
