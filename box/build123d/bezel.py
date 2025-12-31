from build123d import *
from ocp_vscode import *

wi = 29.5       # inside width
hi = 29         # inside height
fw1 = 3.5
fw2 = 2    # frame width
th1 = 2
th2 = 3

wm = wi + 2*fw1
hm = hi + 2*fw1

wo = wm + 2*fw2
ho = hm + 2*fw2

with BuildPart() as p:
    with BuildSketch():
        RectangleRounded(wo, ho, 2)
    extrude(amount=th1)
    with BuildSketch(Plane.XY.offset(th1)):
        Rectangle(wm, hm)
    extrude(amount=th2)

with BuildPart() as inner:
    with BuildSketch():
        Rectangle(wm, hm)
    with BuildSketch(Plane.XY.offset(th1)):
        Rectangle(wi, hi)
    loft()
    with BuildSketch(Plane.XY.offset(th1)):
        Rectangle(wi, hi)
    extrude(amount=th2)

p.part -= inner.part

show(p)

export_step(p.part, 'bezel.step')
