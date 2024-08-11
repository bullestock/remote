bat_d1 = 30
bat_d2 = 37
bat_d3 = 7
bat_d4 = 1.5
bat_z = 2

result = (cq.Workplane("XY")
          .box(bat_d1 + 2*bat_d4,
               bat_d2 + 2*bat_d4,
               bat_z + bat_d3,
               centered=(True, True, False))
          )

bat_inner = (cq.Workplane().
          box(bat_d1, bat_d2, bat_d3, centered=(True, True, False)))

result = result - bat_inner.translate((0, 0, bat_z))

ridge_offset = 2
bat_th = 6

result = (result
          .edges("|Z")
          .fillet(0.65)
          .faces(">Z")
          .tag("ref")
          .transformed(rotate=(90, 0, 0), offset=(bat_d1/2 + ridge_offset, bat_d2/2, bat_z + bat_th - 0))
          .rect(2, 2)
          .extrude(bat_d2)
          .workplaneFromTagged("ref")
          .transformed(rotate=(90, 0, 0), offset=(-(bat_d1/2 + ridge_offset), bat_d2/2, bat_z + bat_th - 0))
          .rect(2, 2)
          .extrude(bat_d2)
          .faces(">Z")
          .fillet(0.7)
          )

show_object(result)

