EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L remote-rescue:POT RV1
U 1 1 5967EB86
P 7800 1375
F 0 "RV1" V 7625 1375 50  0000 C CNN
F 1 "POT" V 7700 1375 50  0000 C CNN
F 2 "Remote:Potentiometer_6mm_axis_square" H 7800 1375 50  0001 C CNN
F 3 "" H 7800 1375 50  0001 C CNN
	1    7800 1375
	-1   0    0    -1  
$EndComp
$Comp
L remote-rescue:POT RV2
U 1 1 5967EC7D
P 8850 1375
F 0 "RV2" V 8675 1375 50  0000 C CNN
F 1 "POT" V 8750 1375 50  0000 C CNN
F 2 "Remote:Potentiometer_6mm_axis_square" H 8850 1375 50  0001 C CNN
F 3 "" H 8850 1375 50  0001 C CNN
	1    8850 1375
	-1   0    0    -1  
$EndComp
$Comp
L remote-rescue:GND #PWR0104
U 1 1 5967ECF1
P 7450 1925
F 0 "#PWR0104" H 7450 1675 50  0001 C CNN
F 1 "GND" H 7450 1775 50  0000 C CNN
F 2 "" H 7450 1925 50  0001 C CNN
F 3 "" H 7450 1925 50  0001 C CNN
	1    7450 1925
	1    0    0    -1  
$EndComp
$Comp
L remote-rescue:+5V #PWR0105
U 1 1 5967EE29
P 7800 875
F 0 "#PWR0105" H 7800 725 50  0001 C CNN
F 1 "+5V" H 7800 1015 50  0000 C CNN
F 2 "" H 7800 875 50  0001 C CNN
F 3 "" H 7800 875 50  0001 C CNN
	1    7800 875 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 1875 7800 1875
Wire Wire Line
	8850 1875 8850 1525
Wire Wire Line
	7800 1525 7800 1875
Connection ~ 7800 1875
Wire Wire Line
	7800 875  7800 1075
Wire Wire Line
	7800 1075 8850 1075
Wire Wire Line
	8850 1075 8850 1225
Connection ~ 7800 1075
Wire Wire Line
	7450 975  7450 1375
Wire Wire Line
	7450 1375 7650 1375
Wire Wire Line
	8700 1375 8450 1375
Text GLabel 7450 975  1    60   Input ~ 0
A7
Text GLabel 8450 975  1    60   Input ~ 0
A6
Wire Wire Line
	8450 975  8450 1375
$Sheet
S 5700 975  850  650 
U 598770F9
F0 "Switches" 60
F1 "switches.sch" 60
$EndSheet
Text GLabel 6675 2225 0    60   Input ~ 0
A6
Text GLabel 6425 2125 0    60   Input ~ 0
A7
Wire Wire Line
	7450 1875 7450 1925
Wire Wire Line
	7800 1075 7800 1225
$Comp
L power:+5V #PWR0106
U 1 1 5E2F4009
P 1800 4050
F 0 "#PWR0106" H 1800 3900 50  0001 C CNN
F 1 "+5V" H 1815 4223 50  0000 C CNN
F 2 "" H 1800 4050 50  0001 C CNN
F 3 "" H 1800 4050 50  0001 C CNN
	1    1800 4050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 5E30283B
P 1900 7075
F 0 "#PWR0107" H 1900 6825 50  0001 C CNN
F 1 "GND" H 1905 6902 50  0000 C CNN
F 2 "" H 1900 7075 50  0001 C CNN
F 3 "" H 1900 7075 50  0001 C CNN
	1    1900 7075
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 1875 8850 1875
$Comp
L power:+5V #PWR0109
U 1 1 5E3728A7
P 1700 1100
F 0 "#PWR0109" H 1700 950 50  0001 C CNN
F 1 "+5V" H 1715 1273 50  0000 C CNN
F 2 "" H 1700 1100 50  0001 C CNN
F 3 "" H 1700 1100 50  0001 C CNN
	1    1700 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 1100 1700 1250
Wire Wire Line
	1700 1250 1350 1250
$Comp
L power:GND #PWR0111
U 1 1 5E376982
P 1700 1600
F 0 "#PWR0111" H 1700 1350 50  0001 C CNN
F 1 "GND" H 1705 1427 50  0000 C CNN
F 2 "" H 1700 1600 50  0001 C CNN
F 3 "" H 1700 1600 50  0001 C CNN
	1    1700 1600
	1    0    0    -1  
$EndComp
Text Notes 650  1250 0    60   ~ 0
+5V IN
Text Notes 675  1375 0    60   ~ 0
GND
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5E38B7FA
P 4150 2375
F 0 "#FLG0101" H 4150 2450 50  0001 C CNN
F 1 "PWR_FLAG" H 4150 2548 50  0000 C CNN
F 2 "" H 4150 2375 50  0001 C CNN
F 3 "~" H 4150 2375 50  0001 C CNN
	1    4150 2375
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0127
U 1 1 5E38BC50
P 4475 2375
F 0 "#PWR0127" H 4475 2225 50  0001 C CNN
F 1 "+5V" H 4490 2548 50  0000 C CNN
F 2 "" H 4475 2375 50  0001 C CNN
F 3 "" H 4475 2375 50  0001 C CNN
	1    4475 2375
	1    0    0    -1  
$EndComp
Wire Wire Line
	4475 2375 4475 2600
Wire Wire Line
	4475 2600 4150 2600
Wire Wire Line
	4150 2600 4150 2375
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5E394FDE
P 4875 2375
F 0 "#FLG0102" H 4875 2450 50  0001 C CNN
F 1 "PWR_FLAG" H 4875 2548 50  0000 C CNN
F 2 "" H 4875 2375 50  0001 C CNN
F 3 "~" H 4875 2375 50  0001 C CNN
	1    4875 2375
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0128
U 1 1 5E395741
P 4875 2625
F 0 "#PWR0128" H 4875 2375 50  0001 C CNN
F 1 "GND" H 4880 2452 50  0000 C CNN
F 2 "" H 4875 2625 50  0001 C CNN
F 3 "" H 4875 2625 50  0001 C CNN
	1    4875 2625
	1    0    0    -1  
$EndComp
Wire Wire Line
	4875 2625 4875 2375
$Comp
L COM-09032:COM-09032 U7
U 1 1 5E3A7320
P 1050 4750
F 0 "U7" H 943 5517 50  0000 C CNN
F 1 "COM-09032" H 943 5426 50  0000 C CNN
F 2 "Remote:AnalogJoystick_ALPS_10k" H 1050 4750 50  0001 L BNN
F 3 "Manufacturer Recommendations" H 1050 4750 50  0001 L BNN
F 4 "SparkFun Electronics" H 1050 4750 50  0001 L BNN "Field4"
F 5 "30.1mm" H 1050 4750 50  0001 L BNN "Field5"
F 6 "N/A" H 1050 4750 50  0001 L BNN "Field6"
	1    1050 4750
	-1   0    0    -1  
$EndComp
$Comp
L COM-09032:COM-09032 U8
U 1 1 5E3B426C
P 1050 6350
F 0 "U8" H 943 7117 50  0000 C CNN
F 1 "COM-09032" H 943 7026 50  0000 C CNN
F 2 "Remote:AnalogJoystick_ALPS_10k" H 1050 6350 50  0001 L BNN
F 3 "Manufacturer Recommendations" H 1050 6350 50  0001 L BNN
F 4 "SparkFun Electronics" H 1050 6350 50  0001 L BNN "Field4"
F 5 "30.1mm" H 1050 6350 50  0001 L BNN "Field5"
F 6 "N/A" H 1050 6350 50  0001 L BNN "Field6"
	1    1050 6350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1800 4050 1800 4350
Wire Wire Line
	1800 4350 1550 4350
Wire Wire Line
	1800 4350 1800 4750
Wire Wire Line
	1800 4750 1550 4750
Connection ~ 1800 4350
Wire Wire Line
	1800 4750 1800 5950
Wire Wire Line
	1800 5950 1550 5950
Connection ~ 1800 4750
Wire Wire Line
	1800 5950 1800 6350
Wire Wire Line
	1800 6350 1550 6350
Connection ~ 1800 5950
Wire Wire Line
	1900 6450 1550 6450
Wire Wire Line
	1550 6050 1900 6050
Wire Wire Line
	1900 6050 1900 6450
Connection ~ 1900 6450
Wire Wire Line
	1550 4850 1900 4850
Wire Wire Line
	1900 4850 1900 5350
Connection ~ 1900 6050
Wire Wire Line
	1550 4450 1900 4450
Wire Wire Line
	1900 4450 1900 4850
Connection ~ 1900 4850
NoConn ~ 1550 5050
NoConn ~ 1550 5150
NoConn ~ 1550 6650
NoConn ~ 1550 6750
Wire Wire Line
	1550 5350 1900 5350
Connection ~ 1900 5350
Wire Wire Line
	1900 5350 1900 6050
Wire Wire Line
	1550 6950 1900 6950
Wire Wire Line
	1900 6950 1900 7075
Wire Wire Line
	1900 6450 1900 6950
Connection ~ 1900 6950
Wire Wire Line
	2100 5850 1550 5850
Wire Wire Line
	1550 6250 2250 6250
$Comp
L Mechanical:MountingHole H1
U 1 1 5E421CF4
P 10300 725
F 0 "H1" H 10400 771 50  0000 L CNN
F 1 "MountingHole" H 10400 680 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 10300 725 50  0001 C CNN
F 3 "~" H 10300 725 50  0001 C CNN
	1    10300 725 
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5E42244B
P 10300 1075
F 0 "H2" H 10400 1121 50  0000 L CNN
F 1 "MountingHole" H 10400 1030 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 10300 1075 50  0001 C CNN
F 3 "~" H 10300 1075 50  0001 C CNN
	1    10300 1075
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5E4229B7
P 10300 1400
F 0 "H3" H 10400 1446 50  0000 L CNN
F 1 "MountingHole" H 10400 1355 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 10300 1400 50  0001 C CNN
F 3 "~" H 10300 1400 50  0001 C CNN
	1    10300 1400
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5E422F37
P 10300 1725
F 0 "H4" H 10400 1771 50  0000 L CNN
F 1 "MountingHole" H 10400 1680 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 10300 1725 50  0001 C CNN
F 3 "~" H 10300 1725 50  0001 C CNN
	1    10300 1725
	1    0    0    -1  
$EndComp
Wire Wire Line
	6675 2225 6825 2225
Wire Wire Line
	6825 2125 6425 2125
Wire Wire Line
	1550 4650 2100 4650
$Comp
L Connector_Generic:Conn_01x02 J6
U 1 1 5E6E9A5E
P 2725 4250
F 0 "J6" H 2805 4242 50  0000 L CNN
F 1 "Conn_01x02" H 2805 4151 50  0000 L CNN
F 2 "Milling:JST_XH_B02B-XH-A_1x02_P2.50mm_Vertical_Milling" H 2725 4250 50  0001 C CNN
F 3 "~" H 2725 4250 50  0001 C CNN
	1    2725 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 4250 2525 4250
Wire Wire Line
	2100 4350 2100 4650
Wire Wire Line
	2525 4350 2100 4350
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 5E6F7807
P 2450 6150
F 0 "J2" H 2530 6142 50  0000 L CNN
F 1 "Conn_01x02" H 2530 6051 50  0000 L CNN
F 2 "Milling:JST_XH_B02B-XH-A_1x02_P2.50mm_Vertical_Milling" H 2450 6150 50  0001 C CNN
F 3 "~" H 2450 6150 50  0001 C CNN
	1    2450 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 5850 2100 6150
Wire Wire Line
	2100 6150 2250 6150
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 5E3EC8B6
P 1150 1250
F 0 "J1" H 1068 1467 50  0000 C CNN
F 1 "Conn_01x02" H 1068 1376 50  0000 C CNN
F 2 "Milling:JST_XH_B02B-XH-A_1x02_P2.50mm_Vertical_Milling" H 1150 1250 50  0001 C CNN
F 3 "~" H 1150 1250 50  0001 C CNN
	1    1150 1250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1350 1350 1700 1350
Wire Wire Line
	1700 1350 1700 1600
$Comp
L Connector_Generic:Conn_01x02 J5
U 1 1 5E3F203B
P 7025 2125
F 0 "J5" H 7105 2117 50  0000 L CNN
F 1 "Conn_01x02" H 7105 2026 50  0000 L CNN
F 2 "Milling:JST_XH_B02B-XH-A_1x02_P2.50mm_Vertical_Milling" H 7025 2125 50  0001 C CNN
F 3 "~" H 7025 2125 50  0001 C CNN
	1    7025 2125
	1    0    0    -1  
$EndComp
$EndSCHEMATC
