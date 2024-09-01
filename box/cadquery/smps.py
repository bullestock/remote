smps_d1 = 20.6
smps_d2 = 43.6
smps_d3 = 4
smps_d4 = 1.5
smps_z = 2

result = (cq.Workplane("XY")
          .box(smps_d1 + 2*smps_d4,
               smps_d2 + 2*smps_d4,
               smps_z + smps_d3,
               centered=(True, True, False))
          .edges("|Z")
          7ujn.fillet(1)
          )

smps_inner = (cq.Workplane().
          box(smps_d1, smps_d2, smps_d3, centered=(True, True, False)))

result = result - smps_inner.translate((0, 0, smps_z))

result = (result
          .faces(">Z")
          .workplane()
          .transformed(offset=(smps_d1/2, 0, 0))
          .box(smps_d4, smps_d2, 10, centered=(False, True, False))
          )

show_object(result)

