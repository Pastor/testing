EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L power:GND #PWR0101
U 1 1 5EE63630
P 10200 1200
F 0 "#PWR0101" H 10200 950 50  0001 C CNN
F 1 "GND" H 10205 1027 50  0000 C CNN
F 2 "" H 10200 1200 50  0001 C CNN
F 3 "" H 10200 1200 50  0001 C CNN
	1    10200 1200
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0102
U 1 1 5EE64129
P 10200 1000
F 0 "#PWR0102" H 10200 850 50  0001 C CNN
F 1 "+5V" H 10215 1173 50  0000 C CNN
F 2 "" H 10200 1000 50  0001 C CNN
F 3 "" H 10200 1000 50  0001 C CNN
	1    10200 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C6
U 1 1 5EE64A3B
P 10100 1100
F 0 "C6" H 9850 1150 50  0000 L CNN
F 1 "0.1uF" H 9800 1050 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10100 1100 50  0001 C CNN
F 3 "~" H 10100 1100 50  0001 C CNN
	1    10100 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C7
U 1 1 5EE65197
P 10300 1100
F 0 "C7" H 10392 1146 50  0000 L CNN
F 1 "0.1uF" H 10392 1055 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10300 1100 50  0001 C CNN
F 3 "~" H 10300 1100 50  0001 C CNN
	1    10300 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	10300 1000 10200 1000
Connection ~ 10200 1000
Wire Wire Line
	10200 1000 10100 1000
Wire Wire Line
	10300 1200 10200 1200
Connection ~ 10200 1200
Wire Wire Line
	10200 1200 10100 1200
$Comp
L Timer:LM555xM U6
U 1 1 5EE6A2F0
P 7350 1600
F 0 "U6" H 7500 2100 50  0000 C CNN
F 1 "LM555xM" H 7600 2000 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 8200 1200 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm555.pdf" H 8200 1200 50  0001 C CNN
	1    7350 1600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5EE6C864
P 7350 2100
F 0 "#PWR0103" H 7350 1850 50  0001 C CNN
F 1 "GND" H 7355 1927 50  0000 C CNN
F 2 "" H 7350 2100 50  0001 C CNN
F 3 "" H 7350 2100 50  0001 C CNN
	1    7350 2100
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0104
U 1 1 5EE6DBA7
P 7350 950
F 0 "#PWR0104" H 7350 800 50  0001 C CNN
F 1 "+5V" H 7365 1123 50  0000 C CNN
F 2 "" H 7350 950 50  0001 C CNN
F 3 "" H 7350 950 50  0001 C CNN
	1    7350 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 1200 7350 950 
Wire Wire Line
	7350 2000 7350 2100
Wire Wire Line
	7850 1800 8000 1800
Wire Wire Line
	8000 1800 8000 2100
Wire Wire Line
	8000 2100 7350 2100
Connection ~ 7350 2100
$Comp
L Device:C_Small C5
U 1 1 5EE6E63D
P 6800 1950
F 0 "C5" H 6892 1996 50  0000 L CNN
F 1 "0.01uF" H 6892 1905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6800 1950 50  0001 C CNN
F 3 "~" H 6800 1950 50  0001 C CNN
	1    6800 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 2050 6800 2100
Wire Wire Line
	6800 2100 7350 2100
Wire Wire Line
	6850 1600 6800 1600
Wire Wire Line
	6800 1600 6800 1850
Wire Wire Line
	6800 2100 6200 2100
Wire Wire Line
	6200 2100 6200 1700
Connection ~ 6800 2100
Wire Wire Line
	6600 1800 6700 1800
Wire Wire Line
	6600 1600 6600 1400
Wire Wire Line
	6600 1400 6850 1400
$Comp
L Device:R_Small R5
U 1 1 5EE74636
P 6600 1100
F 0 "R5" H 6450 1150 50  0000 L CNN
F 1 "1K" H 6450 1050 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 6600 1100 50  0001 C CNN
F 3 "~" H 6600 1100 50  0001 C CNN
	1    6600 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R7
U 1 1 5EE75612
P 6700 1100
F 0 "R7" H 6759 1146 50  0000 L CNN
F 1 "1K" H 6759 1055 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 6700 1100 50  0001 C CNN
F 3 "~" H 6700 1100 50  0001 C CNN
	1    6700 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6600 1000 6600 950 
Wire Wire Line
	6600 950  6700 950 
Connection ~ 7350 950 
Wire Wire Line
	6700 1000 6700 950 
Connection ~ 6700 950 
Wire Wire Line
	6700 950  7350 950 
Wire Wire Line
	6600 1200 6600 1400
Connection ~ 6600 1400
Wire Wire Line
	6700 1200 6700 1800
Connection ~ 6700 1800
Wire Wire Line
	6700 1800 6850 1800
$Comp
L Timer:LM555xM U4
U 1 1 5EE77AC8
P 5050 1600
F 0 "U4" H 5200 2100 50  0000 C CNN
F 1 "LM555xM" H 5300 2000 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5900 1200 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm555.pdf" H 5900 1200 50  0001 C CNN
	1    5050 1600
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0105
U 1 1 5EE7938C
P 5050 950
F 0 "#PWR0105" H 5050 800 50  0001 C CNN
F 1 "+5V" H 5065 1123 50  0000 C CNN
F 2 "" H 5050 950 50  0001 C CNN
F 3 "" H 5050 950 50  0001 C CNN
	1    5050 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R4
U 1 1 5EE797ED
P 5600 1100
F 0 "R4" H 5659 1146 50  0000 L CNN
F 1 "1M" H 5659 1055 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 5600 1100 50  0001 C CNN
F 3 "~" H 5600 1100 50  0001 C CNN
	1    5600 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R3
U 1 1 5EE7A885
P 4300 1100
F 0 "R3" H 4359 1146 50  0000 L CNN
F 1 "1K" H 4359 1055 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 4300 1100 50  0001 C CNN
F 3 "~" H 4300 1100 50  0001 C CNN
	1    4300 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 950  5600 1000
Wire Wire Line
	4550 1800 4500 1800
Wire Wire Line
	4500 1800 4500 950 
Wire Wire Line
	4500 950  5050 950 
Wire Wire Line
	4300 1000 4300 950 
Wire Wire Line
	4300 950  4500 950 
Connection ~ 4500 950 
$Comp
L Device:C_Small C4
U 1 1 5EE7DEF8
P 5600 2000
F 0 "C4" H 5692 2046 50  0000 L CNN
F 1 "0.1uF" H 5692 1955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5600 2000 50  0001 C CNN
F 3 "~" H 5600 2000 50  0001 C CNN
	1    5600 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C3
U 1 1 5EE7F0C0
P 4300 2000
F 0 "C3" H 4392 2046 50  0000 L CNN
F 1 "0.01uF" H 4392 1955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4300 2000 50  0001 C CNN
F 3 "~" H 4300 2000 50  0001 C CNN
	1    4300 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 950  5600 950 
Connection ~ 5050 950 
Wire Wire Line
	5550 1600 5600 1600
Wire Wire Line
	5600 1600 5600 1800
Wire Wire Line
	5550 1800 5600 1800
Connection ~ 5600 1800
Wire Wire Line
	5600 1800 5600 1900
Wire Wire Line
	5600 1600 5600 1200
Connection ~ 5600 1600
Wire Wire Line
	5600 2100 5600 2150
Wire Wire Line
	5600 2150 5050 2150
Wire Wire Line
	4300 2150 4300 2100
Wire Wire Line
	5050 2000 5050 2150
Connection ~ 5050 2150
Wire Wire Line
	5050 2150 4300 2150
Wire Wire Line
	4300 1900 4300 1600
Wire Wire Line
	4300 1600 4550 1600
Wire Wire Line
	4550 1400 4300 1400
Wire Wire Line
	4300 1400 4300 1200
$Comp
L Switch:SW_Push SW1
U 1 1 5EE885A1
P 4050 1400
F 0 "SW1" H 4050 1600 50  0000 C CNN
F 1 "SW_Push" H 4050 1300 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_Push_1P1T_NO_6x6mm_H9.5mm" H 4050 1600 50  0001 C CNN
F 3 "~" H 4050 1600 50  0001 C CNN
	1    4050 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 1400 4250 1400
Connection ~ 4300 1400
Wire Wire Line
	3850 1400 3850 2150
Wire Wire Line
	3850 2150 4300 2150
Connection ~ 4300 2150
$Comp
L power:GND #PWR0106
U 1 1 5EE8B1ED
P 5050 2150
F 0 "#PWR0106" H 5050 1900 50  0001 C CNN
F 1 "GND" H 5055 1977 50  0000 C CNN
F 2 "" H 5050 2150 50  0001 C CNN
F 3 "" H 5050 2150 50  0001 C CNN
	1    5050 2150
	1    0    0    -1  
$EndComp
$Comp
L Timer:LM555xM U1
U 1 1 5EE8BCB5
P 2550 1600
F 0 "U1" H 2700 2100 50  0000 C CNN
F 1 "LM555xM" H 2800 2000 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 3400 1200 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm555.pdf" H 3400 1200 50  0001 C CNN
	1    2550 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R1
U 1 1 5EE8D129
P 3150 1100
F 0 "R1" H 3209 1146 50  0000 L CNN
F 1 "1K" H 3209 1055 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 3150 1100 50  0001 C CNN
F 3 "~" H 3150 1100 50  0001 C CNN
	1    3150 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R2
U 1 1 5EE8E124
P 3250 1600
F 0 "R2" V 3350 1600 50  0000 C CNN
F 1 "1K" V 3145 1600 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 3250 1600 50  0001 C CNN
F 3 "~" H 3250 1600 50  0001 C CNN
	1    3250 1600
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C1
U 1 1 5EE8E870
P 1500 2150
F 0 "C1" H 1592 2196 50  0000 L CNN
F 1 "0.01uF" H 1592 2105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 1500 2150 50  0001 C CNN
F 3 "~" H 1500 2150 50  0001 C CNN
	1    1500 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:CP_Small C2
U 1 1 5EE9291F
P 2000 2150
F 0 "C2" H 2088 2196 50  0000 L CNN
F 1 "1uF 10V" H 2088 2105 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 2000 2150 50  0001 C CNN
F 3 "~" H 2000 2150 50  0001 C CNN
	1    2000 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:R_POT_Small RV1
U 1 1 5EE95A12
P 3450 1600
F 0 "RV1" H 3390 1554 50  0000 R CNN
F 1 "1M" H 3390 1645 50  0000 R CNN
F 2 "Potentiometer_THT:Potentiometer_Runtron_RM-065_Vertical" H 3450 1600 50  0001 C CNN
F 3 "~" H 3450 1600 50  0001 C CNN
	1    3450 1600
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR0107
U 1 1 5EE96C74
P 2550 950
F 0 "#PWR0107" H 2550 800 50  0001 C CNN
F 1 "+5V" H 2565 1123 50  0000 C CNN
F 2 "" H 2550 950 50  0001 C CNN
F 3 "" H 2550 950 50  0001 C CNN
	1    2550 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 1200 3150 1600
Wire Wire Line
	3050 1600 3150 1600
Connection ~ 3150 1600
Wire Wire Line
	3450 1700 3450 2050
Wire Wire Line
	3450 2050 3050 2050
Wire Wire Line
	2000 2050 1750 2050
Wire Wire Line
	1750 2050 1750 1400
Wire Wire Line
	1750 1400 2050 1400
Connection ~ 2000 2050
Wire Wire Line
	3050 1800 3050 2050
Connection ~ 3050 2050
Wire Wire Line
	3050 2050 2000 2050
Wire Wire Line
	2550 2000 2550 2300
Wire Wire Line
	2550 2300 2000 2300
Wire Wire Line
	1500 2300 1500 2250
Wire Wire Line
	2000 2250 2000 2300
Connection ~ 2000 2300
Wire Wire Line
	2000 2300 1500 2300
$Comp
L power:GND #PWR0108
U 1 1 5EEA4B3A
P 2550 2300
F 0 "#PWR0108" H 2550 2050 50  0001 C CNN
F 1 "GND" H 2555 2127 50  0000 C CNN
F 2 "" H 2550 2300 50  0001 C CNN
F 3 "" H 2550 2300 50  0001 C CNN
	1    2550 2300
	1    0    0    -1  
$EndComp
Connection ~ 2550 2300
Wire Wire Line
	1500 2050 1500 1600
Wire Wire Line
	1500 1600 2050 1600
Wire Wire Line
	2550 1200 2550 1000
Wire Wire Line
	2050 1800 2000 1800
Wire Wire Line
	2000 1800 2000 1000
Wire Wire Line
	2000 1000 2550 1000
Connection ~ 2550 1000
Wire Wire Line
	2550 1000 2550 950 
Wire Wire Line
	2550 1000 3150 1000
$Comp
L 74xx:74LS08 U3
U 1 1 5EEAE362
P 4700 3350
F 0 "U3" H 4700 3675 50  0000 C CNN
F 1 "74LS08" H 4700 3584 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 4700 3350 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS08" H 4700 3350 50  0001 C CNN
	1    4700 3350
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS08 U3
U 2 1 5EEAFC2A
P 4700 3900
F 0 "U3" H 4700 4225 50  0000 C CNN
F 1 "74LS08" H 4700 4134 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 4700 3900 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS08" H 4700 3900 50  0001 C CNN
	2    4700 3900
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS08 U3
U 3 1 5EEB1198
P 6050 3700
F 0 "U3" H 6050 4025 50  0000 C CNN
F 1 "74LS08" H 6050 3934 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 6050 3700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS08" H 6050 3700 50  0001 C CNN
	3    6050 3700
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS08 U3
U 5 1 5EEB32D2
P 10350 2300
F 0 "U3" H 10580 2346 50  0000 L CNN
F 1 "74LS08" H 10580 2255 50  0000 L CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 10350 2300 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS08" H 10350 2300 50  0001 C CNN
	5    10350 2300
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS04 U2
U 1 1 5EEB772B
P 4000 3800
F 0 "U2" H 4000 4117 50  0000 C CNN
F 1 "74LS04" H 4000 4026 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 4000 3800 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS04" H 4000 3800 50  0001 C CNN
	1    4000 3800
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS04 U2
U 2 1 5EEB9784
P 4000 4500
F 0 "U2" H 4000 4817 50  0000 C CNN
F 1 "74LS04" H 4000 4726 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 4000 4500 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS04" H 4000 4500 50  0001 C CNN
	2    4000 4500
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS04 U2
U 3 1 5EEBA455
P 7300 3700
F 0 "U2" H 7300 4017 50  0000 C CNN
F 1 "74LS04" H 7300 3926 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 7300 3700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS04" H 7300 3700 50  0001 C CNN
	3    7300 3700
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS04 U2
U 7 1 5EEBBB4D
P 8850 2300
F 0 "U2" H 9080 2346 50  0000 L CNN
F 1 "74LS04" H 9080 2255 50  0000 L CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 8850 2300 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS04" H 8850 2300 50  0001 C CNN
	7    8850 2300
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS32 U5
U 1 1 5EEBD3E6
P 5350 3600
F 0 "U5" H 5350 3925 50  0000 C CNN
F 1 "74LS32" H 5350 3834 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 5350 3600 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS32" H 5350 3600 50  0001 C CNN
	1    5350 3600
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS32 U5
U 5 1 5EEBEE09
P 9650 2300
F 0 "U5" H 9880 2346 50  0000 L CNN
F 1 "74LS32" H 9880 2255 50  0000 L CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 9650 2300 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS32" H 9650 2300 50  0001 C CNN
	5    9650 2300
	1    0    0    -1  
$EndComp
NoConn ~ 7850 1600
Wire Wire Line
	5650 3600 5750 3600
Wire Wire Line
	4300 4500 5700 4500
Wire Wire Line
	5700 4500 5700 3800
Wire Wire Line
	5700 3800 5750 3800
Wire Wire Line
	5000 3900 5000 3700
Wire Wire Line
	5000 3700 5050 3700
Wire Wire Line
	5050 3500 5000 3500
Wire Wire Line
	5000 3500 5000 3350
Wire Wire Line
	4400 3800 4300 3800
Wire Wire Line
	3700 3800 3500 3800
Wire Wire Line
	3500 3800 3500 3450
Wire Wire Line
	3500 3450 4400 3450
Wire Wire Line
	3050 1400 3700 1400
Wire Wire Line
	3700 1400 3700 3250
Wire Wire Line
	3700 3250 4400 3250
Wire Wire Line
	5550 1400 5950 1400
Wire Wire Line
	5950 1400 5950 2850
Wire Wire Line
	5950 2850 3250 2850
Wire Wire Line
	3250 2850 3250 4000
Wire Wire Line
	3250 4000 4400 4000
Wire Wire Line
	7850 1400 8100 1400
Wire Wire Line
	8100 1400 8100 2950
Wire Wire Line
	8100 2950 3500 2950
Wire Wire Line
	3500 2950 3500 3450
Connection ~ 3500 3450
Wire Wire Line
	6350 3700 6650 3700
Text GLabel 3700 4500 0    50   Input ~ 0
HLT
Text GLabel 7600 3700 2    50   Input ~ 0
~CLK
Text GLabel 7600 3900 2    50   Input ~ 0
CLK
$Comp
L Device:LED_ALT D1
U 1 1 5EF0CB49
P 6650 4150
F 0 "D1" V 6689 4032 50  0000 R CNN
F 1 "BLUE" V 6598 4032 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm_Clear" H 6650 4150 50  0001 C CNN
F 3 "~" H 6650 4150 50  0001 C CNN
	1    6650 4150
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R6
U 1 1 5EF0E2AA
P 6650 4450
F 0 "R6" H 6709 4496 50  0000 L CNN
F 1 "220" H 6709 4405 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 6650 4450 50  0001 C CNN
F 3 "~" H 6650 4450 50  0001 C CNN
	1    6650 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5EF0EED8
P 6650 4550
F 0 "#PWR0109" H 6650 4300 50  0001 C CNN
F 1 "GND" H 6655 4377 50  0000 C CNN
F 2 "" H 6650 4550 50  0001 C CNN
F 3 "" H 6650 4550 50  0001 C CNN
	1    6650 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 4300 6650 4350
Connection ~ 6650 3700
Wire Wire Line
	6650 3700 7000 3700
Wire Wire Line
	7600 3900 6650 3900
Wire Wire Line
	6650 3900 6650 3700
Wire Wire Line
	6650 4000 6650 3900
Connection ~ 6650 3900
Wire Wire Line
	8850 1800 9650 1800
Connection ~ 9650 1800
Wire Wire Line
	9650 1800 10350 1800
Wire Wire Line
	8850 2800 9650 2800
Connection ~ 9650 2800
Wire Wire Line
	9650 2800 10350 2800
$Comp
L power:GND #PWR0110
U 1 1 5EF369C9
P 9650 2800
F 0 "#PWR0110" H 9650 2550 50  0001 C CNN
F 1 "GND" H 9655 2627 50  0000 C CNN
F 2 "" H 9650 2800 50  0001 C CNN
F 3 "" H 9650 2800 50  0001 C CNN
	1    9650 2800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0111
U 1 1 5EF37BA7
P 9650 1800
F 0 "#PWR0111" H 9650 1650 50  0001 C CNN
F 1 "+5V" H 9665 1973 50  0000 C CNN
F 2 "" H 9650 1800 50  0001 C CNN
F 3 "" H 9650 1800 50  0001 C CNN
	1    9650 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 5EF3E603
P 9500 3700
F 0 "J1" H 9580 3692 50  0000 L CNN
F 1 "Clock" H 9580 3601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x01_P2.54mm_Vertical" H 9500 3700 50  0001 C CNN
F 3 "~" H 9500 3700 50  0001 C CNN
	1    9500 3700
	1    0    0    -1  
$EndComp
Text GLabel 9300 3700 0    50   Input ~ 0
~CLK
Text GLabel 9300 3800 0    50   Input ~ 0
CLK
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 5EF40B92
P 9500 4050
F 0 "J2" H 9580 4042 50  0000 L CNN
F 1 "Power" H 9580 3951 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x01_P2.54mm_Vertical" H 9500 4050 50  0001 C CNN
F 3 "~" H 9500 4050 50  0001 C CNN
	1    9500 4050
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0112
U 1 1 5EF418F4
P 9300 4050
F 0 "#PWR0112" H 9300 3900 50  0001 C CNN
F 1 "+5V" V 9315 4178 50  0000 L CNN
F 2 "" H 9300 4050 50  0001 C CNN
F 3 "" H 9300 4050 50  0001 C CNN
	1    9300 4050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 5EF42F84
P 9300 4150
F 0 "#PWR0113" H 9300 3900 50  0001 C CNN
F 1 "GND" V 9305 4022 50  0000 R CNN
F 2 "" H 9300 4150 50  0001 C CNN
F 3 "" H 9300 4150 50  0001 C CNN
	1    9300 4150
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J3
U 1 1 5EF43F17
P 9500 4450
F 0 "J3" H 9580 4492 50  0000 L CNN
F 1 "Halt" H 9580 4401 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 9500 4450 50  0001 C CNN
F 3 "~" H 9500 4450 50  0001 C CNN
	1    9500 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0114
U 1 1 5EF47FB2
P 9300 4350
F 0 "#PWR0114" H 9300 4100 50  0001 C CNN
F 1 "GND" V 9305 4222 50  0000 R CNN
F 2 "" H 9300 4350 50  0001 C CNN
F 3 "" H 9300 4350 50  0001 C CNN
	1    9300 4350
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR0115
U 1 1 5EF48F80
P 9300 4550
F 0 "#PWR0115" H 9300 4400 50  0001 C CNN
F 1 "+5V" V 9315 4678 50  0000 L CNN
F 2 "" H 9300 4550 50  0001 C CNN
F 3 "" H 9300 4550 50  0001 C CNN
	1    9300 4550
	0    -1   -1   0   
$EndComp
Text GLabel 9300 4450 0    50   Input ~ 0
HLT
$Comp
L Switch:SW_DPDT_x2 SW2
U 1 1 5EF97118
P 6400 1700
F 0 "SW2" H 6400 1985 50  0000 C CNN
F 1 "Lock" H 6400 1894 50  0000 C CNN
F 2 "Button_Switch_THT:SW_DIP_SPSTx03_Slide_6.7x9.18mm_W7.62mm_P2.54mm_LowProfile" H 6400 1700 50  0001 C CNN
F 3 "~" H 6400 1700 50  0001 C CNN
	1    6400 1700
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 5EFD6F0D
P 10350 3700
F 0 "H1" V 10304 3850 50  0000 L CNN
F 1 "MountingHole_Pad" V 10395 3850 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 10350 3700 50  0001 C CNN
F 3 "~" H 10350 3700 50  0001 C CNN
	1    10350 3700
	0    1    1    0   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 5EFD8626
P 10350 3950
F 0 "H2" V 10304 4100 50  0000 L CNN
F 1 "MountingHole_Pad" V 10395 4100 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 10350 3950 50  0001 C CNN
F 3 "~" H 10350 3950 50  0001 C CNN
	1    10350 3950
	0    1    1    0   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 5EFDBD93
P 10350 4200
F 0 "H3" V 10304 4350 50  0000 L CNN
F 1 "MountingHole_Pad" V 10395 4350 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 10350 4200 50  0001 C CNN
F 3 "~" H 10350 4200 50  0001 C CNN
	1    10350 4200
	0    1    1    0   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 5EFDF412
P 10350 4450
F 0 "H4" V 10304 4600 50  0000 L CNN
F 1 "MountingHole_Pad" V 10395 4600 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 10350 4450 50  0001 C CNN
F 3 "~" H 10350 4450 50  0001 C CNN
	1    10350 4450
	0    1    1    0   
$EndComp
Wire Wire Line
	10250 3700 10250 3950
Connection ~ 10250 3950
Wire Wire Line
	10250 3950 10250 4200
Connection ~ 10250 4200
Wire Wire Line
	10250 4200 10250 4450
$Comp
L power:GND #PWR01
U 1 1 5EFE6688
P 10250 4550
F 0 "#PWR01" H 10250 4300 50  0001 C CNN
F 1 "GND" H 10255 4377 50  0000 C CNN
F 2 "" H 10250 4550 50  0001 C CNN
F 3 "" H 10250 4550 50  0001 C CNN
	1    10250 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	10250 4550 10250 4450
Connection ~ 10250 4450
NoConn ~ 3450 1450
Wire Wire Line
	3450 1450 3450 1500
$EndSCHEMATC