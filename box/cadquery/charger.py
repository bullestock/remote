charger_d1 = 19.1 # 17.5
charger_d2 = 27.8
charger_d3 = 3
charger_d4 = 1.5
charger_z = 4

result = (cq.Workplane("XY")
          .box(charger_d1 + 2*charger_d4,
               charger_d2 + 2*charger_d4,
               charger_z + charger_d3,
               centered=(True, True, False))
          )

charger_inner = (cq.Workplane().
          box(charger_d1, charger_d2, charger_d3, centered=(True, True, False)))

plug_cutout = (cq.Workplane().
          box(15, charger_d2 + 5, charger_d3, centered=(True, True, False)))

result = result - charger_inner.translate((0, 0, charger_z))

result = result - plug_cutout.translate((0, -5, charger_z))

show_object(result)

