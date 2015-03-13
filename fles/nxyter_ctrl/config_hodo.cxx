#include <sstream>
#include <stdint.h>
#include <vector>
#include "fles/ctrl/control/ControlProtocol.hpp"
#include "roc/roclib/roc/defines_roc.h"
#include "roc/roclib/roc/defines_i2c.h"
#include "defines_gpio.h"
#include "roc/roclib/roc/defines_optic.h"
#include "defines_spi.h"
#include "roc/roclib/nxyter/defines_nxyter.h"


// Symbols from ControlClient.hpp are in libcbmnetcntlclientroot.so, loaded in rootlogon.C
// This is to let your IDE know about CbmNet::ControlClient class, and thus make
// autocompletion possible
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "fles/ctrl/control/libclient/ControlClient.hpp"
#endif

using std::ostringstream;

const uint32_t kNxI2CSlaveAddr = 0x8; // 0;

enum adcRegs {
	kReg_chip_port_config =  0x00,
	kReg_chip_id          =  0x01,
	kReg_chip_grade       =  0x02,
	kReg_device_index_A   =  0x05,
	kReg_modes            =  0x08,
	kReg_clock            =  0x09,
	kReg_test_io          =  0x0d,
	kReg_output_mode      =  0x14,
	kReg_output_adjust    =  0x15,
	kReg_output_phase     =  0x16,
	kReg_user_patt1_lsb   =  0x19,
	kReg_user_patt1_msb   =  0x1a,
	kReg_user_patt2_lsb   =  0x1b,
	kReg_user_patt2_msb   =  0x1c,
	kReg_serial_control   =  0x21,
	kReg_serial_ch_stat   =  0x22,
	kReg_device_update    =  0xff
};

enum nxRegs {
	kNxRegMask00  =  0,
	kNxRegMask01  =  1,
	kNxRegMask02  =  2,
	kNxRegMask03  =  3,
	kNxRegMask04  =  4,
	kNxRegMask05  =  5,
	kNxRegMask06  =  6,
	kNxRegMask07  =  7,
	kNxRegMask08  =  8,
	kNxRegMask09  =  9,
	kNxRegMask10  = 10,
	kNxRegMask11  = 11,
	kNxRegMask12  = 12,
	kNxRegMask13  = 13,
	kNxRegMask14  = 14,
	kNxRegMask15  = 15,
	kNxRegVcg     = 16,
	kNxRegIcgfoll = 17,
	kNxRegVth     = 18,
	kNxRegVbfb    = 19,
	kNxRegVbiasF  = 20,
	kNxRegVbiasS  = 21,
	kNxRegVbiasS2 = 22,
	kNxRegVcm     = 23,
	kNxRegcal     = 24,
	kNxRegiCOMP   = 25,
	kNxRegiDUR    = 26,
	kNxRegiINV    = 27,
	kNxRegiPDH    = 28,
	kNxRegiTWC    = 29,
	kNxRegSpare30 = 30,
	kNxRegSpare31 = 31,
	kNxRegConfig0 = 32,
	kNxRegConfig1 = 33,
	kNxRegOverflowLSB   = 34,
	kNxRegOverflowMSB   = 35,
	kNxRegMissTokenLSB  = 36,
	kNxRegMissTokenMSB  = 37,
	kNxRegdelayTestPuls = 38,
	kNxRegdelayTestTrig = 39,
	kNxRegSpare40 = 40,
	kNxRegSpare41 = 41,
	kNxRegTrimDAQPower  = 42,
	kNxRegdelayClock1   = 43,
	kNxRegdelayClock2   = 44,
	kNxRegdelayClock3   = 45
};


//static const uint8_t kNxC0TestPulsEnable    = 1<<0;
//static const uint8_t kNxC0TestPulsSync      = 1<<1;
static const uint8_t kNxC0TestPulsPolarity  = 1<<2;
//static const uint8_t kNxC0TestTrigEnable    = 1<<3;
//static const uint8_t kNxC0TestTrigSync      = 1<<4;
//static const uint8_t kNxC0Disable32MHz      = 1<<5;
//static const uint8_t kNxC0Disable128MHz     = 1<<6;
//static const uint8_t kNxC0TsLsbClockSelect  = 1<<7;
//static const uint8_t kNxC1CalibSelectMask   = 0x3;
static const uint8_t kNxC1FrontEndPolarity  = 1<<2;
static const uint8_t kNxC1ReadClockSelect   = 1<<3;


int SetI2CRegister( CbmNet::ControlClient & conn, uint32_t nodeid, uint32_t port, uint8_t reg, uint8_t val, bool veri = false )
{
	CbmNet::ListSeq list;
	list.AddWrite(ROC_NX_I2C_SWITCH, port);
	list.AddWrite(ROC_NX_I2C_SLAVEADDR, kNxI2CSlaveAddr);
	list.AddWrite(ROC_NX_I2C_REGISTER, reg);
	list.AddWrite(ROC_NX_I2C_DATA, val);
	list.AddRead(ROC_NX_I2C_ERROR);
	if( veri ) {
		list.AddRead(ROC_NX_I2C_DATA);
		list.AddRead(ROC_NX_I2C_ERROR);
	}
	int nSucc = conn.DoListSeq(nodeid, list);

	if( nSucc != list.Size() ) {
		printf("Error in SetI2CRegister: errors during r/w SysCore registers. Number of successfully executed commands is %d out of %d\n", nSucc, list.Size() );
		return 1;
	}

	if( list[4].value != 0 ) {
		printf("Error in SetI2CRegister: I2C bus error on put. Error code is %d\n", list[4].value );
		return 1;
	}

	if( veri ) {
		if( ( list[5].value & 0xff ) != val ) {
			printf("Error in SetI2CRegister: readback value is %d. It is different from put value %d\n", list[6].value, val );
			return 1;
		}

		if( list[6].value != 0 ) {
			printf("Error in SetI2CRegister: I2C bus error on readback. Error code is %d\n", list[6].value );
			return 1;
		}
	}

	return 0;
}



int GetI2CRegister( CbmNet::ControlClient & conn, uint32_t nodeid, uint32_t port, uint8_t reg, uint8_t &val )
{
	CbmNet::ListSeq list;
	list.AddWrite( ROC_NX_I2C_SWITCH, port );
	list.AddWrite( ROC_NX_I2C_SLAVEADDR, kNxI2CSlaveAddr );
	list.AddWrite( ROC_NX_I2C_REGISTER, reg );
	list.AddRead( ROC_NX_I2C_DATA );
	list.AddRead( ROC_NX_I2C_ERROR );

	int nSucc = conn.DoListSeq( nodeid, list );

	if( nSucc != list.Size() ) {
		printf("Error in GetI2CRegister: errors during r/w SysCore registers. Number of successfully executed commands is %d out of %d\n", nSucc, list.Size() );
		return 1;
	}

	if( list[4].value != 0 ) {
		printf("Error in GetI2CRegister: I2C bus error %d\n", list[4].value );
		return 1;
	}

	val = uint8_t( list[3].value );

	return 0;
}



int SetADCRegister( CbmNet::ControlClient & conn, uint32_t nodeid, uint32_t port, uint8_t reg, uint8_t val )
{
	int adc_addr = (port==CON19) ? ROC_NX_ADC_ADDR   : ROC_NX_ADC_ADDR2;
	int adc_put  = (port==CON19) ? ROC_NX_ADC_REG    : ROC_NX_ADC_REG2;
	int adc_get  = (port==CON19) ? ROC_NX_ADC_ANSWER : ROC_NX_ADC_ANSWER2;

	CbmNet::ListSeq list;
	list.AddWrite( adc_addr, reg);
	list.AddWrite( adc_put, val);
	list.AddWrite( adc_addr, 255);
	list.AddWrite( adc_put, 1);
	list.AddWrite( adc_addr, reg);
	list.AddRead( adc_get );

	int nSucc = conn.DoListSeq( nodeid, list );

	if( nSucc != list.Size() ) {
		printf("Error in SetADCRegister: errors during r/w SysCore registers. Number of successfully executed commands is %d out of %d\n", nSucc, list.Size() );
		return 1;
	}

	if( (list[5].value & 0xff) != val ) {
		printf("Error in SetADCRegister: readback value is %d. It is different from put value %d\n", list[5].value, val );
		return 1;
	}

	return 0;
}



int GetADCRegister( CbmNet::ControlClient & conn, uint32_t nodeid, uint32_t port, uint8_t reg, uint8_t & val )
{
	int adc_addr = (port==CON19) ? ROC_NX_ADC_ADDR   : ROC_NX_ADC_ADDR2;
	int adc_get  = (port==CON19) ? ROC_NX_ADC_ANSWER : ROC_NX_ADC_ANSWER2;

	CbmNet::ListSeq list;
	list.AddWrite( adc_addr, reg );
	list.AddRead( adc_get );
	int nSucc = conn.DoListSeq( nodeid, list );

	if( nSucc != list.Size() ) {
		printf("Error in GetADCRegister: errors during r/w SysCore registers. Number of successfully executed commands is %d out of %d\n", nSucc, list.Size() );
		return 1;
	}

	val = list[1].value & 0xff;

	return 0;
}



int SetRocDef(CbmNet::ControlClient & conn, uint32_t nodeid)
{
	CbmNet::ListSeq initList;
	initList.AddWrite(ROC_NX_NXACTIVE,           0);
	initList.AddWrite(ROC_NX_FEB4NX,             0);
	initList.AddWrite(ROC_NX_PARITY_CHECK,       1);
	initList.AddWrite(ROC_NX_ADC_PORT_SELECT1,   1);
	initList.AddWrite(ROC_NX_ADC_PORT_SELECT2,   0);
	initList.AddWrite(ROC_NX_ADC_PORT_SELECT3,   1);
	initList.AddWrite(ROC_NX_ADC_PORT_SELECT4,   0);
	initList.AddWrite(ROC_NX_SR_INIT,       0xfc03);
	initList.AddWrite(ROC_NX_BUFG_SELECT,        0);
	initList.AddWrite(ROC_NX_SR_INIT2,      0xfc03);
	initList.AddWrite(ROC_NX_BUFG_SELECT2,       0);
	initList.AddWrite(ROC_NX_ADC_LATENCY1,      65);
	initList.AddWrite(ROC_NX_ADC_LATENCY2,      65);
	initList.AddWrite(ROC_NX_ADC_LATENCY3,      65);
	initList.AddWrite(ROC_NX_ADC_LATENCY4,      65);
	initList.AddWrite(ROC_NX_DELAY_LTS,        512);
	initList.AddWrite(ROC_NX_DELAY_NX0,        288);
	initList.AddWrite(ROC_NX_DELAY_NX1,        288);
	initList.AddWrite(ROC_NX_DELAY_NX2,        288);
	initList.AddWrite(ROC_NX_DELAY_NX3,        288);
	initList.AddWrite(ROC_NX_DEBUG_MODE,         0);
	return conn.DoListSeq(nodeid, initList);
}



int SetADCDef(CbmNet::ControlClient & conn, uint32_t nodeid, uint32_t port )
{
	int rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_chip_port_config, 0x18 );
	if( rc ) return rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_device_index_A,   0x0f );
	if( rc ) return rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_modes,            0x00 );
	if( rc ) return rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_clock,            0x01 );
	if( rc ) return rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_test_io,          0x00 );
	if( rc ) return rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_output_mode,      0x00 );
	if( rc ) return rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_output_adjust,    0x00 );
	if( rc ) return rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_output_phase,    0x03 );
	if( rc ) return rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_serial_control,  0x03 ); // 12 bit mode !!
	if( rc ) return rc;
	rc = SetADCRegister( conn, nodeid, port, kReg_serial_ch_stat,  0x00 );
	return rc;
}



int SetNxTrimDACRegister(CbmNet::ControlClient & conn, uint32_t nodeid, uint32_t port, const std::vector< uint8_t > & val )
{
	if( val.size() != 129 ) {
		printf("Error in SetNxTrimDACRegister: value array size is not 129\n");
		return 1;
	}

	int rc = SetI2CRegister( conn, nodeid, port, kNxRegTrimDAQPower, val.at( 128 ) );
	if( rc ) return rc;

	for (int iChan=0; iChan<128; iChan++ ) {
		rc = SetI2CRegister( conn, nodeid, port, kNxRegTrimDAQPower, val.at( iChan ) );
		if( rc ) return rc;
	}

	return 0;
}



int GetNxTrimDACRegister(CbmNet::ControlClient & conn, uint32_t nodeid, uint32_t port, std::vector< uint8_t > & val )
{
	if( val.size() != 129 ) {
		val.resize( 129, 0 );
	}

	int rc;

	rc = GetI2CRegister( conn, nodeid, port, kNxRegTrimDAQPower, val.at( 128 ) );
	if( rc ) return rc;

	rc = SetI2CRegister( conn, nodeid, port, kNxRegTrimDAQPower, val.at( 128 ) );
	if( rc ) return rc;

	for (int iChan=0; iChan<128; iChan++ ) {
		rc = GetI2CRegister( conn, nodeid, port, kNxRegTrimDAQPower, val.at( iChan ) );
		if( rc ) return rc;

		rc = SetI2CRegister( conn, nodeid, port, kNxRegTrimDAQPower, val.at( iChan ) );
		if( rc ) return rc;
	}

	return 0;
}



int SetFebDef( CbmNet::ControlClient & conn, uint32_t nodeid, uint32_t port, bool pospol = false )
{
	int rc;
	rc = SetADCDef(conn, nodeid, 0);
	if( rc ) return rc;

	// mask registers
	for( int i = 0; i < 16; ++i ) {
		rc = SetI2CRegister( conn, nodeid, port, i, 0, 1 );
		if( rc ) return rc;
	}

	std::vector< uint8_t > trimDAC;
	trimDAC.resize( 129, 16 );
	rc = SetNxTrimDACRegister( conn, nodeid, port, trimDAC );
	if( rc ) return rc;

	rc = SetI2CRegister( conn, nodeid, port, kNxRegVcg,     160);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegIcgfoll, 255);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegVth,      35);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegVbfb,     30);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegVbiasF,   95);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegVbiasS,   87);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegVbiasS2, 100);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegVcm,     137);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegcal,     255);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegiCOMP,    69);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegiDUR,     15);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegiINV,     54);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegiPDH,     92);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegiTWC,     69);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegConfig0,   0);
	if( rc ) return rc;
	rc = SetI2CRegister( conn, nodeid, port, kNxRegConfig1,  kNxC1ReadClockSelect | 0x3);
	if( rc ) return rc;

	// now handle what is different for positive polarity
	// 1. Front end polarity bit set
	// 2. test puls polarity bit set
	// 3. VBiasF default 65 rather than 95
	if (pospol) {
		uint8_t val;
		rc= GetI2CRegister( conn, nodeid, port, kNxRegConfig1, val);
		if( rc ) return rc;
		rc = SetI2CRegister( conn, nodeid, port, kNxRegConfig1, val | kNxC1FrontEndPolarity);
		if( rc ) return rc;
		rc = SetI2CRegister( conn, nodeid, port, kNxRegConfig0, kNxC0TestPulsPolarity);
		if( rc ) return rc;
		rc = SetI2CRegister( conn, nodeid, port, kNxRegVbiasF,   65);
		if( rc ) return rc;
	}

	return 0;
}




void config_hodo( int link )
{
	//TODO: check if the srd over optics is correctly emulated
	//resetrocnx ts need not to be executed when operated over optics. Instead a dlm has to be sent


	// Custom settings:
	int FlibLink = link;
	const uint32_t kNodeId = 0;
	const uint32_t kRocId = 0;
	const uint32_t kNxPort = 0; // 0 if nX is connected to CON19 connector; 1 for CON20 connector.
	const uint32_t kVBiasS = 142; // 142 messured 21.11. for Hodo2
	const uint32_t kVth = 90; // 90 messured 21.11. for Hodo2
	const uint32_t kVbfb= 50;

	if( kNxPort != 0 && kNxPort != 1 ) { printf("Error! invalid value kNxPort = %d\n", kNxPort ); return; }

	CbmNet::ControlClient conn;
	ostringstream dpath;

	dpath << "tcp://" << "localhost" << ":" << CbmNet::kPortControl + FlibLink;
	conn.Connect(dpath.str());

	SetRocDef( conn, kNodeId );
	conn.Write( kNodeId, ROC_NX_DELAY_LTS, 504 );
	conn.Write( kNodeId, ROC_ROCID, kRocId );

	SetFebDef( conn, kNodeId, kNxPort, 0 );
	SetI2CRegister( conn, kNodeId, kNxPort, kNxRegVbiasS,   kVBiasS );
	SetI2CRegister( conn, kNodeId, kNxPort, kNxRegVth, kVth );
	SetI2CRegister( conn, kNodeId, kNxPort, kNxRegVbfb, kVbfb );

	conn.Write( kNodeId, ROC_NX_FEB4NX, 0 );
	conn.Write( kNodeId, ROC_NX_ADC_PORT_SELECT1, 1 );
	conn.Write( kNodeId, ROC_NX_NXACTIVE, 1 << (kNxPort * 2) );

	//	rocutil> ssm 2
	conn.Write( kNodeId, base::GPIO_SYNCM_SCALEDOWN, 2 );
	conn.Write( kNodeId, base::GPIO_SYNCM_BAUD_START, 1 );
	conn.Write( kNodeId, base::GPIO_SYNCM_BAUD1, 3 );
	conn.Write( kNodeId, base::GPIO_SYNCM_BAUD2, 3 );

//	rocutil> setsyncs 0,1 loop=1
	conn.Write( kNodeId, base::GPIO_CONFIG, 900 );
	conn.Write( kNodeId, base::GPIO_SYNCS0_BAUD_START, 1 );
	conn.Write( kNodeId, base::GPIO_SYNCS0_BAUD1, 3 );
	conn.Write( kNodeId, base::GPIO_SYNCS0_BAUD2, 3 );

//	rocutil> setaux 2,re=1,fe=0
	conn.Write( kNodeId, base::GPIO_CONFIG, 5000900 );


	// StartDAQ
	conn.Write( kNodeId, ROC_NX_FIFO_RESET, 1);
	conn.Write( kNodeId, ROC_NX_FIFO_RESET, 0);
	conn.Write( kNodeId, ROC_OPTICS_START_DAQ, 1);

	uint32_t ret;
	const uint32_t ROC_OPTICS_LINK_STATUS = 0x201008;
	conn.Read( kNodeId, ROC_OPTICS_LINK_STATUS, ret );
	printf("ROC_OPTICS_LINK_STATUS  = %d\n", ret);


}





