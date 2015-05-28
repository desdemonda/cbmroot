#ifndef CBMRICHEPICSDATA_H
#define CBMRICHEPICSDATA_H

#include "TObject.h"



class CbmRichEpicsData: public TObject
{
public:

	CbmRichEpicsData():
		fCBM_BMP180_GetTemp(0.),
		fCBM_BMP180_GetPressure(0.),
		fDataFormatVersion(0),
		fCBM_PWRSWITCH_GetCurrent00(0.),
		fCBM_PWRSWITCH_GetCurrent01(0.),
		fCBM_PWRSWITCH_GetCurrent02(0.),
		fCBM_PWRSWITCH_GetCurrent03(0.),
		fCBM_PWRSWITCH_GetCurrent04(0.),
		fCBM_PWRSWITCH_GetCurrent05(0.),
		fCBM_PWRSWITCH_GetCurrent06(0.),
		fCBM_PWRSWITCH_GetCurrent07(0.),
		fCBM_PWRSWITCH_GetCurrent08(0.),
		fCBM_PWRSWITCH_GetCurrent09(0.),
		fCBM_PWRSWITCH_GetCurrent0A(0.),
		fCBM_PWRSWITCH_GetCurrent0B(0.),
		fCBM_PWRSWITCH_GetCurrent0C(0.),
		fCBM_PWRSWITCH_GetCurrent0D(0.),
		fCBM_PWRSWITCH_GetCurrent0E(0.),
		fCBM_PWRSWITCH_GetCurrent0F(0.),
		fTimestampThresholdFile(0),
		fOUTPUT_TERMINAL_VOLTAGE_U0(0.),
		fMEASUREMENT_CURRENT_U0(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U1(0.),
		fMEASUREMENT_CURRENT_U1(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U2(0.),
		fMEASUREMENT_CURRENT_U2(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U3(0.),
		fMEASUREMENT_CURRENT_U3(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U4(0.),
		fMEASUREMENT_CURRENT_U4(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U5(0.),
		fMEASUREMENT_CURRENT_U5(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U6(0.),
		fMEASUREMENT_CURRENT_U6(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U7(0.),
		fMEASUREMENT_CURRENT_U7(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U8(0.),
		fMEASUREMENT_CURRENT_U8(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U9(0.),
		fMEASUREMENT_CURRENT_U9(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U10(0.),
		fMEASUREMENT_CURRENT_U10(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U11(0.),
		fMEASUREMENT_CURRENT_U11(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U12(0.),
		fMEASUREMENT_CURRENT_U12(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U13(0.),
		fMEASUREMENT_CURRENT_U13(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U14(0.),
		fMEASUREMENT_CURRENT_U14(0.),
		fOUTPUT_TERMINAL_VOLTAGE_U15(0.),
		fMEASUREMENT_CURRENT_U15(0.),
		fCBM_RICH_Mirror_Pos_ActualPosition1(0.),
		fCBM_RICH_Mirror_Pos_ActualPosition2(0.),
		fCBM_RICH_Gas_O2(0.),
		fCBM_RICH_Gas_H2O(0.),
		fCBM_RICH_Gas_PT_1(0.),
		fCBM_RICH_Gas_PTB(0.),
		fCBM_RICH_Gas_PT_2(0.),
		fCBM_RICH_Gas_PT_3(0.),
		fCBM_RICH_Gas_TT_1(0.),
		fCBM_RICH_Gas_TT_2(0.),
		fCBM_RICH_Gas_PT_4(0.),
		fCBM_RICH_Gas_FM_1(0.),
		fCBM_RICH_Gas_RefrIndex(0.),
		fPadiwaThresholdOffset(0)
	{
		;
	}

	/**
	 * \brief Print EPICS data.
	 */
	void Print()
	{
		cout << "---EPICS DATA---" << endl;
		cout << "fCBM_BMP180_GetTemp=" << fCBM_BMP180_GetTemp << " degrees C" << endl;
		cout << "fCBM_BMP180_GetPressure=" << fCBM_BMP180_GetPressure << " Pa" <<endl;
		cout << "fDataFormatVersion=" << fDataFormatVersion << endl;
		cout << "fCBM_PWRSWITCH_GetCurrent [mA]: 00=" << fCBM_PWRSWITCH_GetCurrent00 << " 01=" << fCBM_PWRSWITCH_GetCurrent01 <<
				" 02=" << fCBM_PWRSWITCH_GetCurrent02 << " 03=" << fCBM_PWRSWITCH_GetCurrent03 << " 04=" << fCBM_PWRSWITCH_GetCurrent04 <<
				" 05=" << fCBM_PWRSWITCH_GetCurrent05 << " 06=" << fCBM_PWRSWITCH_GetCurrent06 << " 07=" << fCBM_PWRSWITCH_GetCurrent07 <<
				" 08=" << fCBM_PWRSWITCH_GetCurrent08 << " 08=" << fCBM_PWRSWITCH_GetCurrent08 << " 09=" << fCBM_PWRSWITCH_GetCurrent09 <<
				" 0A=" << fCBM_PWRSWITCH_GetCurrent0A << " 0B=" << fCBM_PWRSWITCH_GetCurrent0B << " 0C=" << fCBM_PWRSWITCH_GetCurrent0C <<
				" 0D=" << fCBM_PWRSWITCH_GetCurrent0D << " 0E=" << fCBM_PWRSWITCH_GetCurrent0E << " 0F=" << fCBM_PWRSWITCH_GetCurrent0F<< endl;
		cout << "fTimestampThresholdFile=" << fTimestampThresholdFile << endl;

		cout <<	"(fOUTPUT_TERMINAL_VOLTAGE_U [V],fMEASUREMENT_CURRENT_U [uA]) " <<
				"U0=(" << fOUTPUT_TERMINAL_VOLTAGE_U0 << "," << fMEASUREMENT_CURRENT_U0 << ") " <<
				"U1=(" << fOUTPUT_TERMINAL_VOLTAGE_U1 << "," << fMEASUREMENT_CURRENT_U1 << ") " <<
				"U2=(" << fOUTPUT_TERMINAL_VOLTAGE_U2 << "," << fMEASUREMENT_CURRENT_U2 << ") " <<
				"U3=(" << fOUTPUT_TERMINAL_VOLTAGE_U3 << "," << fMEASUREMENT_CURRENT_U3 << ") " <<
				"U4=(" << fOUTPUT_TERMINAL_VOLTAGE_U4 << "," << fMEASUREMENT_CURRENT_U4 << ") " <<
				"U5=(" << fOUTPUT_TERMINAL_VOLTAGE_U5 << "," << fMEASUREMENT_CURRENT_U5 << ") " <<
				"U6=(" << fOUTPUT_TERMINAL_VOLTAGE_U6 << "," << fMEASUREMENT_CURRENT_U6 << ") " <<
				"U7=(" << fOUTPUT_TERMINAL_VOLTAGE_U7 << "," << fMEASUREMENT_CURRENT_U7 << ") " <<
				"U8=(" << fOUTPUT_TERMINAL_VOLTAGE_U8 << "," << fMEASUREMENT_CURRENT_U8 << ") " <<
				"U9=(" << fOUTPUT_TERMINAL_VOLTAGE_U9 << "," << fMEASUREMENT_CURRENT_U9 << ") " <<
				"U10=(" << fOUTPUT_TERMINAL_VOLTAGE_U10 << "," << fMEASUREMENT_CURRENT_U10 << ") " <<
				"U11=(" << fOUTPUT_TERMINAL_VOLTAGE_U11 << "," << fMEASUREMENT_CURRENT_U11 << ") " <<
				"U12=(" << fOUTPUT_TERMINAL_VOLTAGE_U12 << "," << fMEASUREMENT_CURRENT_U12 << ") " <<
				"U13=(" << fOUTPUT_TERMINAL_VOLTAGE_U13 << "," << fMEASUREMENT_CURRENT_U13 << ") " <<
				"U14=(" << fOUTPUT_TERMINAL_VOLTAGE_U14 << "," << fMEASUREMENT_CURRENT_U14 << ") " <<
				"U15=(" << fOUTPUT_TERMINAL_VOLTAGE_U15 << "," << fMEASUREMENT_CURRENT_U15 << ") " << endl;

		cout << "fCBM_RICH_Mirror_Pos_ActualPosition1=" << fCBM_RICH_Mirror_Pos_ActualPosition1 << " degrees" << endl;
		cout << "fCBM_RICH_Mirror_Pos_ActualPosition2=" << fCBM_RICH_Mirror_Pos_ActualPosition2 << " degrees" <<endl;
		cout << "fCBM_RICH_Gas_O2=" << fCBM_RICH_Gas_O2 << " ppm" << endl;
		cout << "fCBM_RICH_Gas_H2O=" << fCBM_RICH_Gas_H2O << " ppm" << endl;
		cout << "fCBM_RICH_Gas_PT_1=" << fCBM_RICH_Gas_PT_1 << endl;
		cout << "fCBM_RICH_Gas_PTB=" << fCBM_RICH_Gas_PTB << endl;
		cout << "fCBM_RICH_Gas_PT_2=" << fCBM_RICH_Gas_PT_2 << endl;
		cout << "fCBM_RICH_Gas_PT_3=" << fCBM_RICH_Gas_PT_3 << endl;
		cout << "fCBM_RICH_Gas_TT_1=" << fCBM_RICH_Gas_TT_1 << endl;
		cout << "fCBM_RICH_Gas_TT_2=" << fCBM_RICH_Gas_TT_2 << endl;
		cout << "fCBM_RICH_Gas_PT_4=" << fCBM_RICH_Gas_PT_4 << endl;
		cout << "fCBM_RICH_Gas_FM_1=" << fCBM_RICH_Gas_FM_1 << endl;
		cout << "fCBM_RICH_Gas_RefrIndex=" << fCBM_RICH_Gas_RefrIndex << endl;
		cout << "fPadiwaThresholdOffset=" << fPadiwaThresholdOffset << endl;
	}

	Double_t fCBM_BMP180_GetTemp; //Gas Temperatur in °C measured in Box
	Double_t fCBM_BMP180_GetPressure; //Gas Pressure in Pa measured in Box

	Int_t fDataFormatVersion; //Data Format version: 0x00

	Double_t fCBM_PWRSWITCH_GetCurrent00; //Current measured for Padiwa-Supply  1 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent01; //Current measured for Padiwa-Supply  2 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent02; //Current measured for Padiwa-Supply  3 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent03; //Current measured for Padiwa-Supply  4 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent04; //Current measured for Padiwa-Supply  5 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent05; //Current measured for Padiwa-Supply  6 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent06; //Current measured for Padiwa-Supply  7 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent07; //Current measured for Padiwa-Supply  8 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent08; //Current measured for Padiwa-Supply  9 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent09; //Current measured for Padiwa-Supply 10 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent0A; //Current measured for Padiwa-Supply 11 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent0B; //Current measured for Padiwa-Supply 12 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent0C; //Current measured for Padiwa-Supply 13 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent0D; //Current measured for Padiwa-Supply 14 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent0E; //Current measured for Padiwa-Supply 15 in mA
	Double_t fCBM_PWRSWITCH_GetCurrent0F; //Current measured for Padiwa-Supply 16 in mA

	Int_t fTimestampThresholdFile; //Unix-Timestamp of Threshold file used (based on the time information found in the first line of the Threshold-Log file)

	Double_t fOUTPUT_TERMINAL_VOLTAGE_U0; //HV Voltage of PMT  1 in V
	Double_t fMEASUREMENT_CURRENT_U0; //HV Current of PMT  1 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U1; //HV Voltage of PMT  2 in V
	Double_t fMEASUREMENT_CURRENT_U1; //HV Current of PMT  2 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U2; //HV Voltage of PMT  3 in V
	Double_t fMEASUREMENT_CURRENT_U2; //HV Current of PMT  3 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U3; //HV Voltage of PMT  4 in V
	Double_t fMEASUREMENT_CURRENT_U3; //HV Current of PMT  4 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U4; //HV Voltage of PMT  5 in V
	Double_t fMEASUREMENT_CURRENT_U4; //HV Current of PMT  5 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U5; //HV Voltage of PMT  6 in V
	Double_t fMEASUREMENT_CURRENT_U5; //HV Current of PMT  6 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U6; //HV Voltage of PMT  7 in V
	Double_t fMEASUREMENT_CURRENT_U6; //HV Current of PMT  7 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U7; //HV Voltage of PMT  8 in V
	Double_t fMEASUREMENT_CURRENT_U7; //HV Current of PMT  8 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U8; //HV Voltage of PMT  9 in V
	Double_t fMEASUREMENT_CURRENT_U8; //HV Current of PMT  9 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U9; //HV Voltage of PMT 10 in V
	Double_t fMEASUREMENT_CURRENT_U9; //HV Current of PMT 10 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U10; //HV Voltage of PMT 11 in V
	Double_t fMEASUREMENT_CURRENT_U10; //HV Current of PMT 11 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U11; //HV Voltage of PMT 12 in V
	Double_t fMEASUREMENT_CURRENT_U11; //HV Current of PMT 12 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U12; //HV Voltage of PMT 13 in V
	Double_t fMEASUREMENT_CURRENT_U12; //HV Current of PMT 13 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U13; //HV Voltage of PMT 14 in V
	Double_t fMEASUREMENT_CURRENT_U13; //HV Current of PMT 14 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U14; //HV Voltage of PMT 15 in V
	Double_t fMEASUREMENT_CURRENT_U14; //HV Current of PMT 15 in uA
	Double_t fOUTPUT_TERMINAL_VOLTAGE_U15; //HV Voltage of PMT 16 in V
	Double_t fMEASUREMENT_CURRENT_U15; //HV Current of PMT 16 in uA

	Double_t fCBM_RICH_Mirror_Pos_ActualPosition1; //Mirror Position 1 in °
	Double_t fCBM_RICH_Mirror_Pos_ActualPosition2; //Mirror Position 2 in °

	Double_t fCBM_RICH_Gas_O2; //O2 concentration of gas in ppm
	Double_t fCBM_RICH_Gas_H2O; //H2O concentration of gas in ppm

	Double_t fCBM_RICH_Gas_PT_1; // ?
	Double_t fCBM_RICH_Gas_PTB; // ?

	Double_t fCBM_RICH_Gas_PT_2; // ?
	Double_t fCBM_RICH_Gas_PT_3; // ?

	Double_t fCBM_RICH_Gas_TT_1; // ?
	Double_t fCBM_RICH_Gas_TT_2; // ?

	Double_t fCBM_RICH_Gas_PT_4; // ?
	Double_t fCBM_RICH_Gas_FM_1; //?

	Double_t fCBM_RICH_Gas_RefrIndex; //?

	Int_t fPadiwaThresholdOffset; // Padiwa Threshold offset +32768

private:
	CbmRichEpicsData(const CbmRichEpicsData&);
	CbmRichEpicsData operator=(const CbmRichEpicsData&);
};

#endif // CBMRICHEPICSDATA_H
