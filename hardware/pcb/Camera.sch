EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev "1"
Comp ""
Comment1 "Design for JLCPCB 1-2 Layer Service"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:USB_B_Micro J_input_power
U 1 1 61BF5862
P 4350 1700
F 0 "J_input_power" H 4407 2167 50  0000 C CNN
F 1 "USB_B_Micro" H 4407 2076 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Molex-105017-0001" H 4500 1650 50  0001 C CNN
F 3 "~" H 4500 1650 50  0001 C CNN
	1    4350 1700
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_A J_MC_power
U 1 1 61BF68F6
P 4400 3950
F 0 "J_MC_power" V 4503 4280 50  0000 L CNN
F 1 "USB_A" V 4412 4280 50  0000 L CNN
F 2 "Connector_USB:USB_A_Molex_67643_Horizontal" H 4550 3900 50  0001 C CNN
F 3 " ~" H 4550 3900 50  0001 C CNN
	1    4400 3950
	0    -1   -1   0   
$EndComp
$Comp
L Regulator_Switching:R-78S3.3-0.1 U_MC_regulator
U 1 1 61C0BD8F
P 4650 2550
F 0 "U_MC_regulator" V 4604 2754 50  0000 L CNN
F 1 "5.0V" V 4695 2754 50  0000 L CNN
F 2 "Converter_DCDC:Converter_DCDC_RECOM_R-78HB-0.5L_THT" H 4700 2300 50  0001 L CIN
F 3 "https://www.recom-power.com/pdf/Innoline/R-78Sxx-0.1.pdf" H 4650 2550 50  0001 C CNN
	1    4650 2550
	0    1    1    0   
$EndComp
Wire Wire Line
	4350 2550 4350 3350
Wire Wire Line
	4350 3350 4800 3350
Wire Wire Line
	4800 3350 4800 3950
Connection ~ 4350 2550
Wire Wire Line
	4200 3650 4200 2850
Wire Wire Line
	4200 2850 4650 2850
Wire Wire Line
	4650 2250 4650 2050
Wire Wire Line
	4650 2050 4850 2050
Wire Wire Line
	4850 1500 4650 1500
Wire Wire Line
	4850 2050 4850 1850
Wire Wire Line
	7100 1100 7100 1250
Wire Wire Line
	7650 1100 7100 1100
Wire Wire Line
	7650 1600 7650 1100
Wire Wire Line
	7000 2350 7000 2950
Wire Wire Line
	8300 2350 7000 2350
Wire Wire Line
	8300 1600 8300 2350
Wire Wire Line
	8050 1600 8300 1600
Wire Wire Line
	7000 2950 7350 2950
$Comp
L Device:R_US R_const
U 1 1 61BFE2EF
P 7500 2950
F 0 "R_const" V 7295 2950 50  0000 C CNN
F 1 "10k" V 7386 2950 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 7540 2940 50  0001 C CNN
F 3 "~" H 7500 2950 50  0001 C CNN
	1    7500 2950
	0    1    1    0   
$EndComp
$Comp
L Device:R_PHOTO R_photo
U 1 1 61BFD7CE
P 7800 2950
F 0 "R_photo" V 7475 2950 50  0000 C CNN
F 1 "R_PHOTO" V 7566 2950 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0411_L9.9mm_D3.6mm_P12.70mm_Horizontal" V 7850 2700 50  0001 L CNN
F 3 "~" H 7800 2900 50  0001 C CNN
	1    7800 2950
	0    1    1    0   
$EndComp
$Comp
L Transistor_FET:2N7000 Q_output_mosfet
U 1 1 61BFB588
P 7850 1700
F 0 "Q_output_mosfet" V 8192 1700 50  0000 C CNN
F 1 "2N7000" V 8101 1700 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23_Handsoldering" H 8050 1625 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/NDS7002A-D.PDF" H 7850 1700 50  0001 L CNN
	1    7850 1700
	0    -1   -1   0   
$EndComp
$Comp
L Connector:USB_A J_output_power
U 1 1 61BF8282
P 7100 1650
F 0 "J_output_power" H 6870 1547 50  0000 R CNN
F 1 "USB_A" H 6870 1638 50  0000 R CNN
F 2 "Connector_USB:USB_A_Molex_67643_Horizontal" H 7250 1600 50  0001 C CNN
F 3 " ~" H 7250 1600 50  0001 C CNN
	1    7100 1650
	-1   0    0    1   
$EndComp
Wire Wire Line
	7000 2350 7000 2150
Wire Wire Line
	7000 2150 4350 2150
Connection ~ 7000 2350
Wire Wire Line
	4350 2100 4350 2150
Wire Wire Line
	4350 2150 4350 2550
Connection ~ 4350 2150
$Comp
L Seeeduino_XIAO:SeeeduinoXIAO U?
U 1 1 61C64154
P 7750 4450
F 0 "U?" V 7679 3556 50  0000 R CNN
F 1 "SeeeduinoXIAO" V 7770 3556 50  0000 R CNN
F 2 "Seeeduino:Seeeduino XIAO" H 7400 4650 50  0001 C CNN
F 3 "" H 7400 4650 50  0001 C CNN
	1    7750 4450
	0    1    1    0   
$EndComp
Wire Wire Line
	7000 2950 6800 2950
Wire Wire Line
	6800 2950 6800 5500
Wire Wire Line
	6800 5500 8050 5500
Wire Wire Line
	8050 5500 8050 5250
Connection ~ 7000 2950
Wire Wire Line
	7900 5250 7900 5350
Wire Wire Line
	7900 5350 8800 5350
Wire Wire Line
	8800 5350 8800 2950
Wire Wire Line
	7950 2950 8800 2950
Wire Wire Line
	8200 3600 8200 1900
Wire Wire Line
	7850 1900 8200 1900
Wire Wire Line
	7650 2950 7650 3400
Wire Wire Line
	7650 3400 8050 3400
Wire Wire Line
	8050 3400 8050 3600
Connection ~ 7650 2950
Wire Wire Line
	6800 1850 4850 1850
Connection ~ 4850 1850
Wire Wire Line
	4850 1850 4850 1500
Wire Wire Line
	6800 1550 6800 1650
$EndSCHEMATC
