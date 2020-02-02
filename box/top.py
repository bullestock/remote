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

e = 0.001

center_x = 107.500 + 2.5
center_x_pcb = (159.766 + 90.424)/2

joystick_y = 44.712 + 16
joystick_y_pcb= 217.424

pcb_x_offset = center_x_pcb - center_x
pcb_y_offset = joystick_y_pcb - joystick_y

# Overall height
oah = 40
hole_h = 50
pcb_z = 10

outline = [1168,5479,1163,5477,1159,5473,1158,5468,1160,5463,1161,5461,1128,5424,1129,5423,1091,5371,1048,5306,1049,5306,1002,5228,1002,5227,952,5137,899,5036,843,4925,844,4925,786,4804,727,4675,667,4538,668,4537,607,4393,547,4243,488,4087,429,3927,430,3927,373,3763,318,3597,319,3597,267,3428,218,3258,219,3258,174,3088,174,3087,133,2918,133,2917,98,2749,67,2582,68,2582,43,2418,25,2258,14,2103,11,2027,10,1953,11,1880,14,1809,19,1739,20,1739,27,1672,36,1606,37,1606,48,1543,49,1543,63,1481,80,1422,99,1366,121,1312,145,1260,146,1260,173,1211,173,1212,247,1098,312,1001,313,1001,372,918,427,847,428,847,481,786,482,786,535,734,591,688,651,646,717,606,718,606,792,566,877,524,974,478,975,478,1215,367,1362,297,1531,216,1531,218,1796,156,2059,96,2189,70,2319,47,2448,28,2448,29,2577,16,2641,12,2705,10,2705,11,2768,10,2768,11,2832,13,2895,17,2895,18,2958,24,2958,25,3021,34,3083,46,3146,61,3208,78,3208,79,3270,99,3270,100,3332,123,3331,124,3393,151,3454,181,3454,182,3516,216,3515,216,3576,254,3578,249,3578,248,3976,249,3975,243,3971,240,3966,239,3966,238,3971,240,3975,244,3976,249,3976,250,3977,250,3977,247,3978,247,3977,253,3974,257,3969,258,3968,258,3968,259,3973,257,3977,253,3978,249,4376,248,4375,243,4436,206,4497,172,4559,142,4559,143,4621,116,4683,92,4683,93,4745,73,4808,56,4870,42,4933,30,4933,31,4996,22,5059,16,5122,12,5186,10,5186,11,5249,11,5313,13,5377,17,5377,18,5505,31,5634,50,5764,73,5764,74,5894,101,6157,161,6422,222,6421,224,6736,376,6975,488,7071,535,7155,578,7228,619,7293,660,7293,661,7352,703,7351,704,7406,751,7406,752,7459,804,7458,805,7512,866,7568,937,7567,937,7628,1020,7695,1116,7772,1228,7771,1229,7797,1278,7821,1330,7820,1330,7842,1384,7841,1385,7860,1441,7860,1442,7876,1501,7890,1562,7889,1562,7901,1626,7900,1626,7910,1692,7909,1692,7916,1759,7921,1829,7923,1900,7924,1973,7923,2047,7922,2047,7919,2123,7907,2278,7888,2438,7863,2602,7832,2768,7796,2937,7755,3107,7710,3277,7661,3447,7609,3616,7555,3782,7498,3946,7439,4106,7379,4262,7319,4412,7259,4556,7258,4556,7199,4693,7198,4693,7139,4822,7082,4943,7081,4943,7026,5054,6972,5155,6922,5245,6875,5323,6832,5388,6831,5388,6793,5439,6759,5476,6756,5472,6756,5469,5557,5464,3958,5462,3956,5462,2357,5464,1158,5469,1159,5463,1161,5461,1135,5431]

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
    return translate([x, y, 0])(cylinder(d = 28, h = hole_h))

def toggle(x, y):
    return translate([x, y, 0])(cylinder(d = 6, h = hole_h))

def holes():
    pot1 = pot(61.393 + 6.552/2, 94.327 + 6.552/2)
    pot2 = pot(151.638 + 6.552/2, 94.327 + 6.552/2)
    pb1l = pushbutton(33.404 + 12.5/2, 105.761 + 12.5/2)
    pb2l = pushbutton(27.333 + 12.5/2, 84.594 + 12.5/2)
    pb3l = pushbutton(20.75 + 12.5/2, 63.428 + 12.5/2)
    pb1r = pushbutton(173.679 + 12.5/2, 105.761 + 12.5/2)
    pb2r = pushbutton(179.750 + 12.5/2, 84.594 + 12.5/2)
    pb3r = pushbutton(186.333 + 12.5/2, 63.428 + 12.5/2)
    jlh = joystick_h(53.583 + 16, joystick_y)
    jrh = joystick_h(134 + 16, joystick_y)
    toggle1 = toggle(40.871 + 3, 134.590 + 4)
    toggle2 = toggle(60.287 + 3, 134.590 + 4)
    toggle3 = toggle(153.296 + 3, 134.590 + 4)
    toggle4 = toggle(172.713 + 3, 134.590 + 4)
    holes1 = pot1 + pot2 + pb1l + pb2l + pb3l + pb1r + pb2r + pb3r
    holes2 = jlh + jrh + toggle1 + toggle2 + toggle3 + toggle4
    slide = translate([center_x - 5/2, joystick_y - 1 - 12/2, 0])(cube([5, 12, hole_h]))
    # 2 mm right of center
    display_w = 34
    display_bottom_y = joystick_y + 25.9 + 37.85
    display = translate([center_x - display_w/2 + 2, display_bottom_y, 0])(cube([display_w, 16, hole_h]))
    return holes1 + holes2 + slide + display

def pcbsupport(x, y):
    return cylinder_at(x, y, 3, pcb_z)

def pcbmounts():
    p1 = pcbsupport(42, 63.428 + 12.5/2)
    return p1

def assembly():
    is_x = True
    c = []
    points = []
    scale = 0.025
    for val in outline:
        c.append(val*scale)
        if is_x:
            is_x = False
        else:
            points.append(c)
            c = []
            is_x = True
    outer = linear_extrude(height = oah)(translate([11, 14.758, 0])(polygon(points = points)))
    slide_c_w = 10
    slide_c_h = 25
    slide_c_d = 10
    slide_c1 = c2cube(slide_c_w, slide_c_h, .1)
    slide_c2 = up(slide_c_d)(c2cube(slide_c_w + slide_c_d, slide_c_h + slide_c_d, .1))
    slide_c = translate([center_x, joystick_y - 1, oah - slide_c_d])(hull()(slide_c1 + slide_c2))
    allholes = holes()
    return outer - down(1)(allholes) - slide_c


if __name__ == '__main__':
    a = assembly()
    scad_render_to_file(a, file_header='$fn = %s;' % SEGMENTS, include_orig_code=False)

