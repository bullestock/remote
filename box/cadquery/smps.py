smps_d1 = 17.5
smps_d2 = 37.5
smps_d3 = 3
smps_d4 = 1.5
smps_z = 2

result = (cq.Workplane("XY")
          .box(smps_d1 + 2*smps_d4,
               smps_d2 + 2*smps_d4,
               smps_z + smps_d3,
               centered=(True, True, False))
          )

smps_inner = (cq.Workplane().
          box(smps_d1, smps_d2, smps_d3, centered=(True, True, False)))

result = result - smps_inner.translate((0, 0, smps_z))

show_object(result)

