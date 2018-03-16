/* Generated by Together */

#ifndef ICYLINDER_H
#define ICYLINDER_H

#include "ICommon.h"

const	int	DEF_CYLINDER_SUCCESS				= 0;		// Success
const	int DEF_MAX_CYLINDER_SOLENOID			= 4;
const	int DEF_MAX_CYLINDER_SENSOR				= 4;

const	int	ERR_CYLINDER_SUCCESS				= 0;

class IIO;
class MCylinderData;

/** Cylinder Timer�� ���� ����
 * @stereotype enumeration */
enum ECylinderTime
{
    MOVING_TIME,
	NOSEN_MOVING_TIME,
	SETTLING_TIME1,
	SETTLING_TIME2
};

/*
 * Cylinder Ÿ���� �����Ѵ�.
 */
enum ECylinderType
{
	UNKOWN_CYLINDER,
    UP_DOWN,
	LEFT_RIGHT,
    FRONT_BACK,
    UPSTREAM_DOWNSTREAM,
    CW_CCW,
    OPEN_CLOSE,
    UP_MID_DOWN,
	EXPAND_SHRINK,
	LEFT_MIDE_RIGHT,
    FRONT_MID_BACK,		

    UPSTREAM_MID_DOWNSTREAM,
    UPSTREAM_DOWNSTREAM_VARIOUS_VELOCITY,
    UPSTREAM_MID_DOWNSTREAM_VARIOUS_VELOCITY,
};

/*
 * Solenoid Ÿ���� �����Ѵ�.
 */
enum ESolenoidType
{
	UNKOWN_SOLENOID,
    SINGLE_SOLENOID,
    REVERSE_SINGLE_SOLENOID,
    DOUBLE_SOLENOID,
    DOUBLE_3WAY_SOLENOID,
    DOUBLE_SOLENOID_VARIOUS_VELOCITY,
};

typedef struct
{
	/** ������ Cylinder ��ü�� ID */
	int m_iCylID;
	/** ������ Cylinder ��ü�� �̸� */
//	char m_szName[256];
	WCHAR m_szName[256];
	/** ������ Cylinder ��ü�� ������ Solenoid �ܵ����϶��� �ϳ� ���, �������϶��� 2�� ���  */
	unsigned short m_rgusSolenoid[2];	 // ���� Solenoid
	/** ������ Cylinder ��ü�� ������ ������ Solenoid  +,- ���� 1����   */
	unsigned short m_rgusAccSolenoid[2]; // ������ Solenoid
	/** Up Sensor  : üũ�ϰ��� �ϴ� ���� ��ŭ ���� �ϰ� �������� NULL�� �Ѵ�. */
	unsigned short m_rgusUpSensor[DEF_MAX_CYLINDER_SENSOR]; // 1���� ���� ����
	/** Down Sensor : üũ�ϰ��� �ϴ� ���� ��ŭ ���� �ϰ� �������� NULL�� �Ѵ�. */
	unsigned short m_rgusDownSensor[DEF_MAX_CYLINDER_SENSOR];// 2���� ���� ����
	/** Middle Sensor : ��ϵ� Sensor���� ���� üũ  */
	unsigned short m_rgusMiddleSensor[DEF_MAX_CYLINDER_SENSOR];// �̵� ����
	/** ������ ���� : +����, - ���� 2�� �ۿ� ������ �� ����. */
	unsigned short m_rgusAccSensor[2]; // ������ ����	
	/** @link aggregation Cylinder Ÿ�� */
	ECylinderType m_lnkECylinderType;// �Ǹ��� Ÿ��
	/** @link aggregation Solenoid Ÿ�� */
	ESolenoidType m_lnkESolenoidType;// Solenoid Ÿ��

} SCylinderVal;

typedef struct tagSCylinderData
{		
	SCylinderVal m_sCylVal;

	/** Cylinder Data�� ����� File Name */
	CString m_strFileName;
	
} SCylinderData, *pSCylinderData;

/**
 * @interface
 */
class ICylinder : public virtual ICommon
{

public:
    IIO* m_gpIIO;

/*----------- �Ķ���� ����  -----------------------*/

    virtual int GetCylinderTime(
		double & dMovingTime,
		double & dSettlingTime1,
		double & dSettlingTime2,
		double & dNoSenMovingTime) = 0;

    virtual int GetCylinderTime(ECylinderTime eCylinderTime, double & dCylinderTime) = 0;

/*----------- �Ǹ��� ���� Ȯ��  -----------------------*/
    virtual BOOL IsUp() = 0;

    virtual BOOL IsDown() = 0;

    virtual BOOL IsLeft() = 0;

    virtual BOOL IsRight() = 0;

    virtual BOOL IsFront() = 0;

    virtual BOOL IsBack() = 0;

    virtual BOOL IsDownstr() = 0;

    virtual BOOL IsUpstr() = 0;

    virtual BOOL IsCW() = 0;

    virtual BOOL IsCCW() = 0;

    virtual BOOL IsOpen() = 0;

    virtual BOOL IsClose() = 0;

    virtual BOOL IsMiddle() = 0;

    virtual BOOL IsExpand() = 0;

    virtual BOOL IsShrink() = 0;

/*----------- �Ǹ��� �̵�   -----------------------*/
    virtual int Up(int skip_sensor = FALSE) = 0;

    virtual int Down(int skip_sensor = FALSE) = 0;

    virtual int Left(int skip_sensor = FALSE) = 0;

    virtual int Right(int skip_sensor = FALSE) = 0;

    virtual int Front(int skip_sensor = FALSE) = 0;

    virtual int Back(int skip_sensor = FALSE) = 0;

    virtual int Downstr(int skip_sensor = FALSE) = 0;

    virtual int Upstr(int skip_sensor = FALSE) = 0;

    virtual int CW(int skip_sensor = FALSE) = 0;

    virtual int CCW(int skip_sensor = FALSE) = 0;

    virtual int Open(int skip_sensor = FALSE) = 0;

    virtual int Close(int skip_sensor = FALSE) = 0;

    virtual int Middle(int skip_sensor = FALSE) = 0;	//�߰�����..3way sv�����...

    virtual int Expand(int skip_sensor = FALSE) = 0;

    virtual int Shrink(int skip_sensor = FALSE) = 0;

	virtual int OutputToggle() = 0;

    virtual int StartUp() = 0;

    virtual int StartDown() = 0;

    virtual int StartLeft() = 0;

    virtual int StartRight() = 0;

    virtual int StartFront() = 0;

    virtual int StartBack() = 0;

    virtual int StartDownstr() = 0;

    virtual int StartUpstr() = 0;

    virtual int StartCW() = 0;

    virtual int StartCCW() = 0;

    virtual int StartOpen() = 0;

    virtual int StartClose() = 0;

    virtual int StartMiddle() = 0;	//�߰�����..3way sv�����...

    virtual int StartExpand() = 0;

    virtual int StartShrink() = 0;

/*----------- �Ǹ��� ���ۿϷᶧ���� Sleep   -----------------------*/

    virtual int Wait4UpComplete() = 0;

    virtual int Wait4DownComplete() = 0;

    virtual int Wait4LeftComplete() = 0;

    virtual int Wait4RightComplete() = 0;

    virtual int Wait4FrontComplete() = 0;

    virtual int Wait4BackComplete() = 0;
 
    virtual int Wait4CWComplete() = 0;

    virtual int Wait4CCWComplete() = 0;

    virtual int Wait4OpenComplete() = 0;

    virtual int Wait4CloseComplete() = 0;

    virtual int Wait4DownstrComplete() = 0;

    virtual int Wait4UpstrComplete() = 0;

    virtual int Wait4MiddleComplete(BOOL bDir) = 0;

    virtual int Wait4ExpandComplete() = 0;

    virtual int Wait4ShrinkComplete() = 0;

	virtual void OffSolenoid() = 0;

	virtual void OffSolenoid(BOOL bDir) = 0;

	virtual BOOL IsMoveComplete(BOOL bDir) = 0;

/*------------------------ Common Interface -----------------------*/

	virtual int AssignComponents(IIO *pIO) = 0;

	virtual int SetData(SCylinderData datComponent) = 0;

	virtual int GetData(SCylinderData *pData) = 0;
 
	virtual int LoadParameter() = 0;

	virtual int SaveParameter(MCylinderData* pcylTimerData) = 0;
	
	virtual void GetParameter(MCylinderData* pcylTimerData) = 0;

	virtual CString GetCylinderName() = 0;

	virtual double GetUpElapseTime() = 0;

	virtual double GetDownElapseTime() = 0;

	virtual unsigned short* GetSolAddr() = 0;

	virtual unsigned short* GetUpSensorAddr() = 0;

	virtual unsigned short* GetDownSensorAddr() = 0;
};
#endif //ICYLINDER_H