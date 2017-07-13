PCBNEW-LibModule-V1  29/07/2014 4:03:31 PM
# encoding utf-8
Units mm
$INDEX
125DrillHole
12mm-Battery-SMD-Holder
Bourns-Inductor-SDR7030
LEDWithResistor
MicroSD-Yamaichi-PJS
Molex-Mini-USB-B-Vertical
PIC-ICD2,3-POGOPIN
SPST-PTS645-Pushbutton
TE-5520258-3-RJ12-6P6P
Vertical-Crystal-Can
Vertical-Crystal-Can-Small
$EndINDEX
$MODULE 125DrillHole
Po 0 0 0 15 53D82840 00000000 ~~
Li 125DrillHole
Sc 0
AR 
Op 0 0 0
T0 0 2.54 1 1 0 0.15 N I 21 N "125DrillHole"
T1 1.27 2.54 1 1 0 0.15 N I 21 N "VAL**"
$PAD
Sh "1" C 3.556 3.556 0 0 0
Dr 3.175 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 0 0
$EndPAD
$EndMODULE 125DrillHole
$MODULE 12mm-Battery-SMD-Holder
Po 0 0 0 15 533CDD83 00000000 ~~
Li 12mm-Battery-SMD-Holder
Sc 0
AR 
Op 0 0 0
T0 -0.254 9.906 1 1 0 0.15 N I 21 N "12mm-Battery-SMD-Holder"
T1 0 -9.652 1 1 0 0.15 N V 21 N "VAL**"
DC 0 0 8.636 -0.254 0.15 21
$PAD
Sh "2" R 3.9624 3.9624 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po 0 0
$EndPAD
$PAD
Sh "1" C 2.54 2.54 0 0 0
Dr 1.8542 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 6.604 0
$EndPAD
$PAD
Sh "1" C 2.54 2.54 0 0 0
Dr 1.8542 0 0
At STD N 00E0FFFF
Ne 0 ""
Po -6.604 0
$EndPAD
$EndMODULE 12mm-Battery-SMD-Holder
$MODULE Bourns-Inductor-SDR7030
Po 0 0 0 15 53386E77 00000000 ~~
Li Bourns-Inductor-SDR7030
Sc 0
AR 
Op 0 0 0
T0 0 1.5 1 1 0 0.15 N V 21 N "Bourns-Inductor-SDR7030"
T1 -2 -10.5 1 1 0 0.15 N V 21 N "VAL**"
DS 3.5 -1 3 -1 0.15 21
DS -3.5 -1 -3 -1 0.15 21
DS 3 -8.5 3.5 -8.5 0.15 21
DS -3.5 -8.5 -3 -8.5 0.15 21
DS -3.5 -1 -3.5 -8.5 0.15 21
DS 3.5 -1 3.5 -8.5 0.15 21
$PAD
Sh "1" R 2.4 2.4 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po 0 -1.2
$EndPAD
$PAD
Sh "2" R 2.4 2.4 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po 0 -7.8
$EndPAD
$EndMODULE Bourns-Inductor-SDR7030
$MODULE LEDWithResistor
Po 0 0 0 15 52D3A540 00000000 ~~
Li LEDWithResistor
Sc 0
AR 
Op 0 0 0
T0 5.08 -3.175 1 1 0 0.15 N V 21 N "LEDWithResistor"
T1 8.89 -1.27 1 1 0 0.15 N I 21 N "VAL**"
T2 4.445 -1.27 1 1 0 0.15 N V 21 N "-"
T2 1.905 -1.27 1 1 0 0.15 N V 21 N "+"
DS 0 0 6.35 0 0.15 21
DS 6.35 0 6.35 -2.286 0.15 21
DS 6.35 -2.286 0 -2.286 0.15 21
DS 0 -2.286 0 0 0.15 21
$PAD
Sh "1" R 1.143 1.143 0 0 0
Dr 0.7112 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 0.635 -1.27
$EndPAD
$PAD
Sh "2" C 1.143 1.143 0 0 0
Dr 0.7112 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 5.715 -1.27
$EndPAD
$EndMODULE LEDWithResistor
$MODULE MicroSD-Yamaichi-PJS
Po 0 0 0 15 533861DE 00000000 ~~
Li MicroSD-Yamaichi-PJS
Sc 0
AR 
Op 0 0 0
T0 8.5 2 1 1 0 0.15 N V 21 N "MicroSD-Yamaichi-PJS"
T1 6.5 -18 1 1 0 0.15 N V 21 N "VAL**"
DS -2.5 -17 21 -17 0.15 21
DS 21 -17 21 0.5 0.15 21
DS 21 0.5 -2.5 0.5 0.15 21
DS -2.5 0.5 -2.5 -17 0.15 21
$PAD
Sh "SHLD" R 3.5 1.6 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po 1.35 -0.8
$EndPAD
$PAD
Sh "SHLD" R 3.5 1.4 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po 12.35 -0.7
$EndPAD
$PAD
Sh "SHLD" R 3.5 1.5 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po 12.35 -15.65
$EndPAD
$PAD
Sh "SW1" R 1.4 1.95 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po 9.4 -15.425
$EndPAD
$PAD
Sh "SHLD" R 3.1 1.2 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po 0.85 -15.8
$EndPAD
$PAD
Sh "1" R 1.6 0.8 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po -0.8 -5.65
$EndPAD
$PAD
Sh "2" R 1.6 0.8 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po -0.8 -6.675
$EndPAD
$PAD
Sh "3" R 1.6 0.8 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po -0.8 -7.675
$EndPAD
$PAD
Sh "4" R 1.6 0.8 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po -0.8 -8.675
$EndPAD
$PAD
Sh "5" R 1.6 0.8 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po -0.8 -9.675
$EndPAD
$PAD
Sh "6" R 1.6 0.8 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po -0.8 -10.675
$EndPAD
$PAD
Sh "7" R 1.6 0.8 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po -0.8 -11.675
$EndPAD
$PAD
Sh "8" R 1.6 0.8 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po -0.8 -12.675
$EndPAD
$PAD
Sh "SW2" R 1.6 0.8 0 0 0
Dr 0 0 0
At SMD N 00888000
Ne 0 ""
Po -0.8 -13.675
$EndPAD
$EndMODULE MicroSD-Yamaichi-PJS
$MODULE Molex-Mini-USB-B-Vertical
Po 0 0 0 15 5337EA45 00000000 ~~
Li Molex-Mini-USB-B-Vertical
Sc 0
AR 
Op 0 0 0
T0 3.45 -5.7 1 1 0 0.15 N V 21 N "Molex-Mini-USB-B-Vertical"
T1 3.45 1.15 1 1 0 0.15 N V 21 N "VAL**"
DS 0 0 7.7 0 0.15 21
DS 7.7 0 7.7 -4.7 0.15 21
DS 7.7 -4.7 0 -4.7 0.15 21
DS 0 -4.7 0 0 0.15 21
$PAD
Sh "~" C 2.921 2.921 0 0 0
Dr 2.5781 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 0.2 -2.35
$EndPAD
$PAD
Sh "~" C 2.921 2.921 0 0 0
Dr 2.5781 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 7.5 -2.35
$EndPAD
$PAD
Sh "1" C 1.143 1.143 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 2.25 -1.05
$EndPAD
$PAD
Sh "2" C 1.143 1.143 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 3.05 -3.65
$EndPAD
$PAD
Sh "3" C 1.143 1.143 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 3.85 -1.05
$EndPAD
$PAD
Sh "4" C 1.143 1.143 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 4.65 -3.65
$EndPAD
$PAD
Sh "5" C 1.143 1.143 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 5.45 -1.05
$EndPAD
$EndMODULE Molex-Mini-USB-B-Vertical
$MODULE PIC-ICD2,3-POGOPIN
Po 0 0 0 15 5337AEC4 00000000 ~~
Li PIC-ICD2,3-POGOPIN
Sc 0
AR 
Op 0 0 0
T0 5.08 2.54 1 1 0 0.15 N I 21 N "PIC-ICD2,3-POGOPIN"
T1 2.54 -2.54 1 1 0 0.15 N V 21 N "VAL**"
$PAD
Sh "1" R 1.651 1.651 0 0 0
Dr 1.27 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 0 0
$EndPAD
$PAD
Sh "2" C 1.651 1.651 0 0 0
Dr 1.27 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 2.54 0
$EndPAD
$PAD
Sh "3" C 1.651 1.651 0 0 0
Dr 1.27 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 5.08 0
$EndPAD
$PAD
Sh "4" C 1.651 1.651 0 0 0
Dr 1.27 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 7.62 0
$EndPAD
$PAD
Sh "5" C 1.651 1.651 0 0 0
Dr 1.27 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 10.16 0
$EndPAD
$PAD
Sh "6" C 1.651 1.651 0 0 0
Dr 1.27 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 12.7 0
$EndPAD
$EndMODULE PIC-ICD2,3-POGOPIN
$MODULE SPST-PTS645-Pushbutton
Po 0 0 0 15 533FA400 00000000 ~~
Li SPST-PTS645-Pushbutton
Sc 0
AR 
Op 0 0 0
T0 3.5 3 1 1 0 0.15 N V 21 N "SPST-PTS645-Pushbutton"
T1 1.5 -7 1 1 0 0.15 N V 21 N "VAL**"
DS 3.5 -2 3.5 1.5 0.15 21
DS 3.5 -6 3.5 -4 0.15 21
DS 3.5 -4 2.5 -2.5 0.15 21
DS 8 1.5 8 -6 0.15 21
DS 8 -6 -1.5 -6 0.15 21
DS -1.5 -6 -1.5 1.5 0.15 21
DS -1.5 1.5 8 1.5 0.15 21
$PAD
Sh "2" C 1.5 1.5 0 0 0
Dr 1.0668 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 0 0
$EndPAD
$PAD
Sh "2" C 1.5 1.5 0 0 0
Dr 1.0668 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 6.5 0
$EndPAD
$PAD
Sh "1" C 1.5 1.5 0 0 0
Dr 1.0668 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 6.5 -4.5
$EndPAD
$PAD
Sh "1" C 1.5 1.5 0 0 0
Dr 1.0668 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 0 -4.5
$EndPAD
$EndMODULE SPST-PTS645-Pushbutton
$MODULE TE-5520258-3-RJ12-6P6P
Po 0 0 0 15 5337AB0D 00000000 ~~
Li TE-5520258-3-RJ12-6P6P
Sc 0
AR 
Op 0 0 0
T0 2.5654 -11.9634 1 1 0 0.15 N I 21 N "TE-5520258-3-RJ12-6P6P"
T1 1.5748 7.3533 1 1 0 0.15 N V 21 N "VAL**"
DS -5.969 5.715 -5.969 -10.922 0.15 21
DS -5.969 -10.922 10.16 -10.922 0.15 21
DS 10.16 -10.922 10.16 5.715 0.15 21
DS -5.969 5.715 10.16 5.715 0.15 21
$PAD
Sh "6" C 1.5 1.5 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 5.2705 -7.239
$EndPAD
$PAD
Sh "5" C 1.5 1.5 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 4.0005 -9.779
$EndPAD
$PAD
Sh "4" C 1.5 1.5 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 2.7305 -7.239
$EndPAD
$PAD
Sh "3" C 1.5 1.5 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 1.4605 -9.779
$EndPAD
$PAD
Sh "2" C 1.5 1.5 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 0.1905 -7.239
$EndPAD
$PAD
Sh "1" R 1.5 1.5 0 0 0
Dr 0.889 0 0
At STD N 00E0FFFF
Ne 0 ""
Po -1.0795 -9.779
$EndPAD
$PAD
Sh "~" C 3.429 3.429 0 0 0
Dr 3.2639 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 7.1755 -0.889
$EndPAD
$PAD
Sh "~" C 3.429 3.429 0 0 0
Dr 3.2639 0 0
At STD N 00E0FFFF
Ne 0 ""
Po -2.9845 -0.889
$EndPAD
$EndMODULE TE-5520258-3-RJ12-6P6P
$MODULE Vertical-Crystal-Can
Po 0 0 0 15 5338A741 00000000 ~~
Li Vertical-Crystal-Can
Sc 0
AR 
Op 0 0 0
T0 0.35 2.7 1 1 0 0.15 N V 21 N "Vertical-Crystal-Can"
T1 -0.15 -2.75 1 1 0 0.15 N V 21 N "VAL**"
DC 0 0 1.55 0.05 0.15 21
$PAD
Sh "2" C 0.8128 0.8128 0 0 0
Dr 0.508 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 0.55 0
$EndPAD
$PAD
Sh "1" C 0.8128 0.8128 0 0 0
Dr 0.508 0 0
At STD N 00E0FFFF
Ne 0 ""
Po -0.55 0
$EndPAD
$EndMODULE Vertical-Crystal-Can
$MODULE Vertical-Crystal-Can-Small
Po 0 0 0 15 533A3F0F 00000000 ~~
Li Vertical-Crystal-Can-Small
Sc 0
AR 
Op 0 0 0
T0 0.35 2.7 1 1 0 0.15 N V 21 N "Vertical-Crystal-Can-Small"
T1 -0.15 -2.75 1 1 0 0.15 N V 21 N "VAL**"
DC 0 0 1 0 0.15 21
$PAD
Sh "2" C 0.635 0.635 0 0 0
Dr 0.508 0 0
At STD N 00E0FFFF
Ne 0 ""
Po 0.35 0
$EndPAD
$PAD
Sh "1" C 0.635 0.635 0 0 0
Dr 0.508 0 0
At STD N 00E0FFFF
Ne 0 ""
Po -0.35 0
$EndPAD
$EndMODULE Vertical-Crystal-Can-Small
$EndLIBRARY
