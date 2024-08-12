bat_d1 = 30
bat_d2 = 30
th = 2
extra = 2

result = (cq.Workplane("XY")
          .box(bat_d1 + 2*extra + 3*th + 1, bat_d2, th,
               centered=(True, True, False))
          .faces(">Z")
          .workplane()
          .rarray(bat_d1 + 2*extra + 2*th + 1, 1, 2, 1)
          .box(th, bat_d2, th + 0,
               centered=(True, True, False))
          .faces(">Z")
          .workplane()
          .rarray(bat_d1 + 2*extra + th + 1, 1, 2, 1)
          .box(2*th, bat_d2, th,
               centered=(True, True, False))
          .faces(">Z or <Z")
          .fillet(0.7)
          .edges("|Z")
          .fillet(0.5)
          )

show_object(result)

