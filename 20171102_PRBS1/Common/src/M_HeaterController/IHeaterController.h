/* Generated by Together */

#ifndef IHEATER_CONTROLLER_H
#define IHEATER_CONTROLLER_H

#include "ICommon.h"
#include "ISerialPort.h"
#include "IComThread.h"
#include "IIO.h"
#include "DefSystem.h"

const	int		ERR_HEATER_CONTROLLER_SUCCESS					= 0;
/*
const	int		ERR_HEATER_CONTROLLER_SERIAL_COMM_TIMEOUT		= 1;
const	int		ERR_HEATER_CONTROLLER_SET_GET_VALUE_MISMATCH	= 2;
const	int		ERR_HEATER_CONTROLLER_RECEIVE_DATA_NULL			= 3;
const	int		ERR_HEATER_CONTROLLER_ARGUMENT_NULL_POINTER		= 4;
const	int		ERR_HEATER_CONTROLLER_PARITY_ERROR				= 5;
const	int		ERR_HEATER_CONTROLLER_FRAMING_ERROR				= 6;
const	int		ERR_HEATER_CONTROLLER_OVERRUN_ERROR				= 7;
const	int		ERR_HEATER_CONTROLLER_UNDEFINED_ERROR			= 8;
const	int		ERR_HEATER_CONTROLLER_FORMAT_ERROR				= 9;
const	int		ERR_HEATER_CONTROLLER_NONEXECUTABLE_COMMAND_ERROR	= 10;
const	int		ERR_HEATER_CONTROLLER_UNDEFINED_DATA_ERROR		= 11;
const	int		ERR_HEATER_CONTROLLER_UNKNOWN_ERROR_CODE		= 12;
*/

const	double	DEF_MAX_TEMPERATURE							= 470.0;	// unit-degree Celcius
const	double	DEF_MIN_TEMPERATURE							= 0.0;		// unit-degree Celcius

/** Definition for Controlling Tool's Temperature */
typedef enum
{
	DEF_HEATER_CONTROLLER_ID0	= 0,
	DEF_HEATER_CONTROLLER_ID1	= 1,
	DEF_HEATER_CONTROLLER_ID2	= 2,
	DEF_HEATER_CONTROLLER_ID3	= 3,
	DEF_HEATER_CONTROLLER_ID4	= 4,
	DEF_HEATER_CONTROLLER_ID5	= 5,
	DEF_HEATER_CONTROLLER_ID6	= 6,
	DEF_HEATER_CONTROLLER_ID7	= 7,
	DEF_HEATER_CONTROLLER_ID8	= 8,
	DEF_HEATER_CONTROLLER_ID9	= 9,
	DEF_HEATER_CONTROLLER_ID10	= 10,
	DEF_HEATER_CONTROLLER_ID11	= 11,
	DEF_HEATER_CONTROLLER_ID12	= 12,
	DEF_HEATER_CONTROLLER_ID13	= 13,
	DEF_HEATER_CONTROLLER_ID14	= 14,
	DEF_HEATER_CONTROLLER_ID15	= 15,
	DEF_HEATER_CONTROLLER_ID16	= 16,
	DEF_HEATER_CONTROLLER_ID17	= 17,
	DEF_HEATER_CONTROLLER_ID18	= 18,
	DEF_HEATER_CONTROLLER_ID19	= 19,
	DEF_HEATER_CONTROLLER_ID20	= 20,
	DEF_HEATER_CONTROLLER_ID21	= 21,
	DEF_HEATER_CONTROLLER_ID22	= 22,
	DEF_HEATER_CONTROLLER_ID23	= 23,
	DEF_HEATER_CONTROLLER_ID24	= 24,
	DEF_HEATER_CONTROLLER_ID25	= 25,
	DEF_HEATER_CONTROLLER_ID26	= 26,
	DEF_HEATER_CONTROLLER_ID27	= 27,
} EHeaterControllerID;

typedef enum 
{ 
	SEND_CMD_SET_TEMPERATURE		= 1,
	RECV_CMD_GET_TEMPERATURE		= 11,
	RECV_CMD_SET_TEMPERATURE		= 12
} ETemperatureControlCommand;


/** Heater Controller Component�� ������ H/W Component List */
typedef struct tagSHeaterControllerRefCompList
{
	/** IO component */
	IIO				*m_plnkIO;

	/** Serial Component communicating with Temperature/Pressure controller */
//    ISerialPort		*m_plnkSerial;
    IComThread		*m_plnkSerial;
	
} SHeaterControllerRefCompList, *pSHeaterControllerRefCompList;


typedef struct tagSHeaterControllerData
{
	int				m_iNumHeater;
	int				m_iNumOfOverHeaterAlarm;
	int				m_iNumOfDisconnectHeaterAlarm;
	double			m_dMaxTemperature;
	double			m_dMinTemperature;

	/** IO Address Heater Alarm*/
	unsigned short	m_usAddrHeaterAlarm[DEF_MAX_NUM_HEATER];

	/** Over Heater Alarm */
	// Added on 2004.11.29
	unsigned short	m_usAddrOverHeatAlarm[DEF_MAX_NUM_OVERHEAT_ALARM];
	
	/** Heater Disconnect Check*/
	unsigned short	m_usAddrDisconnectHeatAlarm[DEF_MAX_NUM_CONNECT_ALARM];

	/** Serial communication timeout Setting */
	double			m_dTimeoutSerialComm;	// unit - sec

} SHeaterControllerData, *pSHeaterControllerData;


/** @interface */
class IHeaterController : public virtual ICommon
{

public:

    /**
     * Tool�� �ʱ�ȭ �Ѵ�. 
	 *
     * ��  Motor�� Cylinder�� �ʱ���ġ�� �̵���Ų��.
     * @return  Error Code : 0 = Success
     *                        �׿�= Error
     */
    virtual int Initialize() = 0;

/*---------- Controlling Temperature Operations --------------------*/
	/**
	* Setting Temperature of the given indexed Heater
	*
	* @param	iHeaterIndex : heater index - 1,2,3 (1-Based)
	* @param	dTemperature : temperature to be set( Min/Max definded in DefHeaterController.h )
	* @return	Error Code : 0 = SUCCESS, etc = Error
	*/
	virtual int	SetHeaterTemperature( EHeaterControllerID eHeaterIndex, double dTemperature ) = 0;

	/**
	* Getting Temperature of the given indexed Heater
	*
	* @param	iHeaterIndex : heater index - 1,2,3 (1-Based)
	* @param	dTemperature : temperature to get( Min/Max definded in DefHeaterController.h )
	* @return	Error Code : 0 = SUCCESS, etc = Error
	*/
	virtual int	GetHeaterTemperature( EHeaterControllerID eHeaterIndex, double *dTemperature ) = 0;


	/**
	* Getting Setting Temperature of the given indexed Heater
	*
	* @param	eHeaterIndex : heater index - 1,2,3 (1-Based)
	* @param	dTemperature : temperature to get( Min/Max definded in DefHeaterController.h )
	* @return	Error Code : 0 = SUCCESS, etc = Error
	*/
	virtual int	GetSettingHeaterTemperature( EHeaterControllerID eHeaterIndex, double *dTemperature ) = 0;

	/**
	* Setting Temperature of all Heaters
	*
	* @param	iNumHeaters : array size
	* @param	eHeaterIndex : Heater indexes
	* @param	rgdTemperature : temperature array to be set( Min/Max definded in DefHeaterController.h )
	* @return	Error Code : 0 = SUCCESS, etc = Error
	*/
	virtual int	SetHeaterTemperature(	int iNumHeaters, 
										EHeaterControllerID *eHeaterIndex, 
										double *rgdTemperature ) = 0;

	/**
	* Getting Temperature of all Heaters
	*
	* @param	dTemperature : temperature array to get( Min/Max definded in DefHeaterController.h )
	* @return	Error Code : 0 = SUCCESS, etc = Error
	*/
	virtual int	GetHeaterTemperature( double *dTemperature, int *iNumHeaters ) = 0;

	/**
	* Getting Setting Temperature of all Heaters
	*
	* @param	dTemperature : temperature array to get( Min/Max definded in DefHeaterController.h )
	* @return	Error Code : 0 = SUCCESS, etc = Error
	*/
	virtual int	GetSettingHeaterTemperature( double *dTemperature, int *iNumHeaters ) = 0;

	/**
	* Check Heater Alarms
	// Added on 2004.11.29
	*
	* @param	alarm io index : return value of the alarm i/o 
	* @return	boolean value : TRUE - Alarm State, FALSE - no alarm state 
	*/
	virtual BOOL IsHeaterInAlarmState(int *iAlarmIndex = NULL ) = 0;

	/**
	* Check Heater Alarms of the given indexed Heater
	*
	* @param	iHeaterIndex : heater index - 1,2,3 ...(1-Based) - defined in a definition file
	* @param	HeaterAlarm : FALSE = No Alram, TURE = Alarm
	* @return	Error : 0 - Success, etc. = error
	*/
	virtual int	CheckHeaterAlarms( EHeaterControllerID iHeaterIndex, BOOL *pbStates ) = 0;

	/**
	* Check Heater Alarms of all Heaters
	*
	* @param	HeaterAlarm : FALSE = No Alram, TURE = Alarm
	* @return	Error : 0 - Success, etc. = error
	*/
	virtual int	CheckHeaterAlarms( BOOL *rgbStaes, int *iNumHeaters ) = 0;

	/**
	* Assign component linkage
	* 
	* @param	listRefComponent	: SAcfToolRefCompList information
	*/
	virtual int AssignComponents(SHeaterControllerRefCompList listRefComponent) = 0;

	/** 
	* Component Data ó�� 
	*
	* @param	pData : SAcfToolData����ü�� ���� ������
	*/
	virtual int SetData(SHeaterControllerData datComponent) = 0;

	/** 
	* Component Data ó�� 
	*
	* @param	pData : SDrawerData����ü�� ���� ������
	*/
	virtual int GetData(SHeaterControllerData *pData) = 0; 

};
#endif //IHEATER_CONTROLLER_H