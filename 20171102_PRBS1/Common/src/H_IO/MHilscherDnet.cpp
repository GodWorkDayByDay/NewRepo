/* Generated by Together */
/* written by jsMoon on 2004.03.17 */

#include "stdafx.h"
#include "assert.h"
#include "MHilscherDnet.h"
#include <atlconv.h>
#include <direct.h>

#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** Default ������ */
MHilscherDnet::MHilscherDnet()
{
	m_usBoardNumber = 0;
	m_bDeviceOpened = FALSE;
	m_lpthreadID = NULL;
	m_hHandle = NULL;

	InitializeCriticalSection(&m_csOutput);
}

/** ������
 * @param iObjectID : Component�� Object ID
 * @param iErrorBase : Error ���� Offset
 * @param strFullFileName : �α� ���� �̸� �� Path
 * @param ucLevel : �α� ����
	                DEF_MLOG_NONE_LOG_LEVEL			: 0x00;	// Log �� ��
					DEF_MLOG_ERROR_LOG_LEVEL		: 0x01;	// Error���� Log
					DEF_MLOG_MONITORING_LOG_LEVEL	: 0x02;	// Monitoring ���� Log
					DEF_MLOG_NORMAL_LOG_LEVEL		: 0x04;	// ���� ���� ���� Log
  * @param iDays : �α� ���� ���� �Ⱓ
  */
MHilscherDnet::MHilscherDnet(SCommonAttribute commonData, unsigned short usBoardNumber)
: ICommon(commonData)
{
	CString strLogMsg;

	m_usBoardNumber = usBoardNumber;
	m_bDeviceOpened = FALSE;
	m_lpthreadID = NULL;
	m_hHandle = NULL;

	InitializeCriticalSection(&m_csOutput);
}

/** ������ Contructor 
 * MHILSCHERDNET ��ü�����ڷν� Board Number�� parameter�� �޴´�.
 * @param usBoardNumber : Board Number
 * @return 0 = Success, �׿� = Error Number
 * @stereotype constructor
 */
MHilscherDnet::MHilscherDnet(unsigned short usBoardNumber)
{
	m_usBoardNumber = usBoardNumber;
	m_bDeviceOpened = FALSE;
	m_lpthreadID = NULL;
	m_hHandle = NULL;

	InitializeCriticalSection(&m_csOutput);
}

MHilscherDnet::~MHilscherDnet()
{
	m_bDeviceOpened = FALSE;

	if (WaitForSingleObject(m_hHandle, 10000L) == WAIT_TIMEOUT)
		TerminateThread(m_hHandle,  1L);

	CloseHandle(m_hHandle); // �ڵ��� ��ȯ�Ѵ�.

	DeleteCriticalSection(&m_csOutput);

	TRACE(_T("MHilscherDnet �Ҹ�\n"));
}

/**
 * Hilscher Board���� Communication�� ���� Driver�� Open�ϸ�,  Board�� �ʱ�ȭ�ϰ� ��� ��� ���°� �ǰ� �Ѵ�.
 * @return 0 = Success,  �׿� = Error Number
 */
int MHilscherDnet::Initialize()
{
	/* Initialize two buffers */
    for (int i = 0; i < (MAX_DEVICE * 4); i++)
    {
        m_ucOutgoingBuffer[i] = 0;
        m_ucIncomingBuffer[i] = 0;
    }
 
	memset((char*)&m_tblDnStatus, 0x0, sizeof(m_tblDnStatus));	// Devicenet ���� ���� �ʱ�ȭ

#ifndef SIMULATION
	if (DevOpenDriver(0) != DRV_NO_ERROR)	// Open a driver
    {
		//WriteErrorLog("Initialize() : DevOpenDriver() Fail ");
		SetErrorLevel(_T("HILSCHERDNET initialized"),3, __FILE__, __LINE__);
		// 100003 = Driver Open �۾��� �����߽��ϴ�.
		return generateErrorCode(100003);
    }
    else if (DevInitBoard(m_usBoardNumber,  NULL) != DRV_NO_ERROR)	// Initialize a board
	{
		//WriteErrorLog("Initialize() : DevInitBoard() Fail");
		SetErrorLevel(_T("HILSCHERDNET initialized"),3, __FILE__, __LINE__);
		// 100004 = Board �ʱ�ȭ �۾��� �����߽��ϴ�.
        return generateErrorCode(100004);
	}
	// Inform the board that the application is running
    else if (DevSetHostState(m_usBoardNumber,  HOST_READY, 0L) != DRV_NO_ERROR)
	{
		//WriteErrorLog("Initialize() : DevSetHostState() Fail");
		SetErrorLevel(_T("HILSCHERDNET initialized"),3, __FILE__, __LINE__);
		// 100005 = Board Ready ���� ���� �۾��� �����߽��ϴ�.
        return generateErrorCode(100005);
	}
#endif
	m_bDeviceOpened = TRUE;
	return ERR_IO_SUCCESS;
}

/**
 * Hilscher Board���� Communication�� �����ϰ� Device Driver�� Close�Ѵ�.
 * @return 0 = Success,  �׿� = Error Number
 */
int MHilscherDnet::Terminate()
{
#ifdef SIMULATION
	return ERR_IO_SUCCESS;
#endif

    if (DevSetHostState(0, HOST_NOT_READY, 0L) != DRV_NO_ERROR) 
		// 100006 = Board Not Ready ���� ���� �۾��� �����߽��ϴ�.
		return generateErrorCode(100006);

    if (DevExitBoard(0) != DRV_NO_ERROR) 
		// 100007 = Board Exit �۾��� �����߽��ϴ�.
        return generateErrorCode(100007);

    if (DevCloseDriver(0) != DRV_NO_ERROR) 
	{
		// 100008 = Dirver Close �۾��� �����߽��ϴ�.
        return generateErrorCode(100008);
    }

	return ERR_IO_SUCCESS;
}

/**
 * I/O Device�� Digital Status (Bit) ��  �о�帰��.
 * @precondition �� �Լ��� �����ϱ� ���� initialize �Լ��� �̸� ����Ǿ���� �Ѵ�.
 * @param usIOAddr : IO Address
 * @param pbVal    : IO ��
 * @return  0      : SUCCESS
 *	        else   : Device \Error �ڵ� 
 */
int MHilscherDnet::GetBit(unsigned short usIOAddr,  BOOL* pbVal)
{
	UINT rel; 
	int port,  bitnum;
	unsigned short usOffset;

	ASSERT(usIOAddr >= INPUT_ORIGIN && usIOAddr <= OUTPUT_END);

	// Check IO Status
	int iRet = returnDnetStatus();
	if (iRet != ERR_IO_SUCCESS) 
		return iRet;

	if (usIOAddr < OUTPUT_ORIGIN)
	{
		// get a bit from the incoming buffer
		usOffset = usIOAddr - INPUT_ORIGIN;
		port = usOffset / 8;
		bitnum = usOffset % 8;
		rel = (UINT)m_ucIncomingBuffer[port];
		rel = rel & 0x000000FF;
	}
	else 
	{ 
		// get a bit from the outgoing buffer
		usOffset = usIOAddr - OUTPUT_ORIGIN;
		port = usOffset / 8;
		bitnum = usOffset % 8;
		rel = (UINT)m_ucOutgoingBuffer[port];
		rel = rel & 0x000000FF;
	}
	
	if (rel & bitOnMask[bitnum])
		*pbVal = TRUE;
	else
		*pbVal = FALSE;

	return ERR_IO_SUCCESS;
}

/**
 * I/O Device�� Digital Status (Bit) �� �о�鿩 �����Ѵ�.
 * @precondition �� �Լ��� �����ϱ� ���� initialize �Լ��� �̸� ����Ǿ���� �Ѵ�.
 * @param usIOAddr : IO Address
 * @return  TRUE   : ON
			FALSE  : ON�� �ƴ�
 */
BOOL MHilscherDnet::IsOn(unsigned short usIOAddr)
{
	BOOL bVal;

	if (GetBit(usIOAddr, &bVal) != 0)
		return FALSE;

	return bVal;
}

/**
 * I/O Device�� Digital Status (Bit) �� �о�鿩 �����Ѵ�.
 * @precondition �� �Լ��� �����ϱ� ���� initialize �Լ��� �̸� ����Ǿ���� �Ѵ�.
 * @param usIOAddr : IO Address
 * @return  TRUE   : OFF
	        FALSE  : OFF�� �ƴ� 
 */
int MHilscherDnet::IsOff(unsigned short usIOAddr)
{
	BOOL	bVal;

	if (GetBit(usIOAddr, &bVal) != 0)
		return FALSE;

	return !bVal;
}

/**
 * Output Device�� On Command (Bit = 1) �� ������.
 * @param strIOAddr : IO Address String (ex, "1000:START_SW")
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::OutputOn(unsigned short usIOAddr)
{
	BYTE data;
	int port,  bitnum;
	unsigned short usOffset;

	assert(usIOAddr >= INPUT_ORIGIN && usIOAddr <= OUTPUT_END);

	// Check IO Status
	int iRet = returnDnetStatus();
	if (iRet != ERR_IO_SUCCESS) 
		return iRet;

	if (usIOAddr < OUTPUT_ORIGIN)
		return ERROR_ID;	// Don't touch the incoming buffer

	usOffset = usIOAddr - OUTPUT_ORIGIN;
	port = usOffset / 8;
	bitnum = usOffset % 8;
	
	EnterCriticalSection(&m_csOutput);

	data = m_ucOutgoingBuffer[port];
	data = data | bitOnMask[bitnum];
	
	m_ucOutgoingBuffer[port] = data;

	LeaveCriticalSection(&m_csOutput);

	return ERR_IO_SUCCESS;
}

/**
 * Output Device�� Off Command (Bit = 0) �� ������.
 * @param strIOAddr : IO Address String (ex, "1000:START_SW")
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::OutputOff(unsigned short usIOAddr)
{
	BYTE data;
	int port,  bitnum;
	unsigned short usOffset;

	assert(usIOAddr >= INPUT_ORIGIN && usIOAddr <= OUTPUT_END);

	// Check IO Status
	int iRet = returnDnetStatus();
	if (iRet != ERR_IO_SUCCESS)
		return iRet;

	if (usIOAddr < OUTPUT_ORIGIN)
		return ERROR_ID;	// Don't touch the incoming buffer

	usOffset = usIOAddr - OUTPUT_ORIGIN;
	port = usOffset / 8;
	bitnum = usOffset % 8;
	
	EnterCriticalSection(&m_csOutput);

	data = m_ucOutgoingBuffer[port];
	data = data & bitOffMask[bitnum];
	
	m_ucOutgoingBuffer[port] = data;

	LeaveCriticalSection(&m_csOutput);

	return ERR_IO_SUCCESS;
}

/**
 * Output Device�� Digital Status�� Set�̸� (Bit = 0), Output Device�� On Command (Bit = 1) �� ������,
 * Output Device�� Digital Status�� Clear�̸� (Bit = 1), Output Device�� Off Command (Bit = 0) �� ������.
 * @param strIOAddr : IO Address String (ex, "1000:START_SW")
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::OutputToggle(unsigned short usIOAddr)
{
	BYTE data;
	int port, bitnum;
	unsigned short usOffset;
	BOOL bVal;
	
	assert(usIOAddr >= INPUT_ORIGIN && usIOAddr <= OUTPUT_END);

	// Check IO Status
	int iRet = returnDnetStatus();
	if (iRet != ERR_IO_SUCCESS)
		return iRet;

	if (usIOAddr < OUTPUT_ORIGIN)
		return ERROR_ID;

	usOffset = usIOAddr - OUTPUT_ORIGIN;
	port = usOffset / 8;
	bitnum = usOffset % 8;
	
	EnterCriticalSection(&m_csOutput);

	iRet = GetBit(usIOAddr, &bVal);
	if (iRet != ERR_IO_SUCCESS) 
		return iRet;

	if (bVal == 1) 
	{
		data = m_ucOutgoingBuffer[port];
		data = data & bitOffMask[bitnum];
	}
	else
	{
		data = m_ucOutgoingBuffer[port];
		data = data | bitOnMask[bitnum];
	}
	
	m_ucOutgoingBuffer[port] = data;

	LeaveCriticalSection(&m_csOutput);

	return ERR_IO_SUCCESS;
}

/**
 * ���ӵ� 8���� IO Address�� ������ Input Device ���� Digital Status�� �о�鿩 pcValue pointer�� �Ѱ��ش�.
 * @param strIOAddr : ���ӵ� 8���� IO Address�� �����ϴ� IO Address�� String Type (ex, "1000:START_SW")
 * @param pcValuse : ���ӵ� 8���� IO Address�� ������ Input Device ���� Digital Status�� �о�鿩 pcValue�� �����Ѵ�.
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::GetByte(unsigned short usIOAddr,  BYTE & pcValue)
{
	BYTE lowdata,  highdata; 
	int port,  bitnum;
	unsigned short usOffset;
	
	assert(usIOAddr >= INPUT_ORIGIN && usIOAddr <= OUTPUT_END);

	// Check IO Status
	int iRet = returnDnetStatus();
	if (iRet != ERR_IO_SUCCESS)
		return iRet;

	if (usIOAddr < OUTPUT_ORIGIN)	
	{	// get a byte from the incoming buffer
		usOffset = usIOAddr - INPUT_ORIGIN;	
		port = usOffset / 8;
		bitnum = usOffset % 8;	
		if (bitnum == 0) 
		{
			pcValue = m_ucIncomingBuffer[port];
		}
		else
		{
			lowdata = m_ucIncomingBuffer[port];
			highdata = m_ucIncomingBuffer[port + 1];
			
			lowdata = (lowdata >> bitnum);
			highdata = (highdata << (8 - bitnum));
			pcValue = lowdata | highdata;
		}
	}
	else 
	{ // get a byte from the outgoing buffer
		usOffset = usIOAddr - OUTPUT_ORIGIN;
		port = usOffset / 8;
		bitnum = usOffset % 8;	
		if (bitnum == 0)	
		{
			pcValue = m_ucOutgoingBuffer[port];
		}
		else
		{
			lowdata = m_ucOutgoingBuffer[port];
			highdata = m_ucOutgoingBuffer[port + 1];

			lowdata = (lowdata >> bitnum);
			highdata = (highdata << (8 - bitnum));
			pcValue = lowdata | highdata;
		}
	}
	return ERR_IO_SUCCESS;
}

/**
 * ���ӵ� 8���� IO Address�� ������ Output Device�鿡 On or Off Command�� ������.
 * @param strIOAddr : ���ӵ� 8���� IO Address�� �����ϴ� IO Address�� String Type (ex, "1000:START_SW")
 * @param pcValuse : Output Device�� ���� Command�� �����ϰ� �ִ� �����̴�.
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::PutByte(unsigned short usIOAddr,  BYTE pcValue)
{
	BYTE data,  lowdata,  highdata; 
	int port,  bitnum; 
	unsigned short usOffset;

	assert(usIOAddr >= INPUT_ORIGIN && usIOAddr <= OUTPUT_END);

	// Check IO Status
	int iRet= returnDnetStatus() ;
	if (iRet != ERR_IO_SUCCESS) 
		return iRet;

	data = pcValue;
	
	if (usIOAddr < OUTPUT_ORIGIN)	
	{
		return ERROR_ID;
	}
	
	usOffset = usIOAddr - OUTPUT_ORIGIN;
	port = usOffset / 8;
	bitnum = usOffset % 8;	

	if (bitnum == 0)	
	{
		m_ucOutgoingBuffer[port] = data;
	}
	else
	{
		lowdata = m_ucOutgoingBuffer[port];
		lowdata = (lowdata & highbitsMask[bitnum]) | (data << bitnum);
		m_ucOutgoingBuffer[port] = lowdata;

		highdata = m_ucOutgoingBuffer[port + 1];
		highdata = (highdata & lowbitsMask[bitnum - 1]) | (data >> (8-bitnum));
		m_ucOutgoingBuffer[port + 1] = highdata;
	}
	return ERR_IO_SUCCESS;
}

/**
 * ���ӵ� 16���� IO Address�� ������ Input Device ���� Digital Status�� �о�鿩 pcValue pointer�� �Ѱ��ش�.
 * @param strIOAddr : ���ӵ� 16���� IO Address�� �����ϴ� IO Address�� String Type (ex, "1000:START_SW")
 * @param pwValuse : ���ӵ� 16���� IO Address�� ������ Input Device ���� Digital Status�� �о�鿩 pcValue�� �����Ѵ�.
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::GetWord(unsigned short usIOAddr,  WORD & pwValue)
{
	WORD lowdata,  middledata,  highdata; 
	int port,  bitnum;
	unsigned short usOffset;
	
	assert(usIOAddr >= INPUT_ORIGIN && usIOAddr <= OUTPUT_END);

	// Check IO Status
	int iRet = returnDnetStatus();
	if (iRet != ERR_IO_SUCCESS) 
		return iRet;

	if (usIOAddr < OUTPUT_ORIGIN)	
	{ // get a word from the incoming buffer
		usOffset = usIOAddr - INPUT_ORIGIN;	
		port = usOffset / 8;
		bitnum = usOffset % 8;	
		if (bitnum == 0)
		{
			lowdata = (WORD) m_ucIncomingBuffer[port];
			highdata = (WORD) m_ucIncomingBuffer[port+1];
			
			highdata = highdata << 8;
			pwValue = lowdata | highdata;
		}
		else
		{ 
			lowdata = (WORD) m_ucIncomingBuffer[port];
			middledata = (WORD) m_ucIncomingBuffer[port + 1];
			highdata = (WORD) m_ucIncomingBuffer[port + 2];

			lowdata = lowdata >> bitnum;
			middledata = middledata << (8 - bitnum);
			highdata = highdata << (16 - bitnum);
			pwValue = lowdata | middledata | highdata;
		}
	}
	else 
	{ // get a word from the outgoing buffer
		usOffset = usIOAddr - OUTPUT_ORIGIN;	
		port = usOffset / 8;
		bitnum = usOffset % 8;	
		if (bitnum == 0) 
		{
			lowdata = (WORD) m_ucOutgoingBuffer[port];
			highdata = (WORD) m_ucOutgoingBuffer[port + 1];
			highdata = highdata << 8;
			pwValue = lowdata | highdata;
		}
		else
		{
			lowdata = (WORD) m_ucOutgoingBuffer[port];
			middledata = (WORD) m_ucOutgoingBuffer[port + 1];
			highdata = (WORD) m_ucOutgoingBuffer[port + 2];

			lowdata = lowdata >> bitnum;
			middledata = middledata << (8 - bitnum);
			highdata = highdata << (16 - bitnum);
			pwValue = lowdata | middledata | highdata;
		}
	}
	return ERR_IO_SUCCESS;
}

/**
 * ���ӵ� 16���� IO Address�� ������ Output Device�鿡 On or Off Command�� ������.
 * @param strIOAddr : ���ӵ� 16���� IO Address�� �����ϴ� IO Address�� String Type (ex, "1000:START_SW")
 * @param pwValuse : Output Device�� ���� Command�� �����ϰ� �ִ� �����̴�.
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::PutWord(unsigned short usIOAddr,  WORD pwValue)
{
	WORD data;
	BYTE lowdata,  middledata,  highdata,  tmpdata; 
	int port,  bitnum;
    unsigned short usOffset;
	
	assert(usIOAddr >= INPUT_ORIGIN && usIOAddr <= OUTPUT_END);

	// Check IO Status
	int iRet = returnDnetStatus();
	if (iRet != ERR_IO_SUCCESS) 
		return iRet;

	data = pwValue;
	
	if (usIOAddr < OUTPUT_ORIGIN)	
	{
		return ERROR_ID;
	}
	else
	{
		usOffset = usIOAddr - OUTPUT_ORIGIN;	
		port = usOffset / 8;
		bitnum = usOffset % 8;	
		if (bitnum == 0)
		{
			lowdata = (BYTE) (data & 0x00FF);
			highdata = (BYTE) (data >> 8);
			m_ucOutgoingBuffer[port] = lowdata;
			m_ucOutgoingBuffer[port + 1] = highdata;
		}
		else
		{
			lowdata = (BYTE) ((data << bitnum) & 0x00FF);
			middledata = (BYTE) ((data >> (8 - bitnum)) & 0x00FF);
			highdata = (BYTE) ((data >> (16 - bitnum)) & 0x00FF);

			tmpdata = m_ucOutgoingBuffer[port];
			lowdata = lowdata | (tmpdata & highbitsMask[bitnum]);
			tmpdata = m_ucOutgoingBuffer[port + 2];
			highdata = highdata | (tmpdata & lowbitsMask[bitnum - 1]);

			m_ucOutgoingBuffer[port] = lowdata;
			m_ucOutgoingBuffer[port + 1] = middledata;
			m_ucOutgoingBuffer[port + 2] = highdata;
		}
	}
	return ERR_IO_SUCCESS;
}

/**
 * String Type I/O Address�� unsigned short type���� ��ȯ �Ͽ� usIOAddr�� I/O Address�� return.
 * @param strIOAddr : String Type I/O Address (ex, "1000:START_SW")
 * @param usIOAddr : unsigned short type I/O Address
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::IOAddrInterpreter(CString strIOAddr,  unsigned short & usIOAddr)
{
	int cnt = strIOAddr.Find(':');
	if (cnt == -1)
		return ERROR_ID;
	
	CString strTmp = strIOAddr.Left(cnt);
	//usIOAddr = (unsigned short) atoi(strTmp);
	usIOAddr = (unsigned short) _wtoi(strTmp);

	return ERR_IO_SUCCESS;
}

/**
 * I/O Device�� Digital Status (Bit) ��  �о�帰��.
 * @precondition �� �Լ��� �����ϱ� ���� initialize �Լ��� �̸� ����Ǿ���� �Ѵ�.
 * @param usIOAddr : IO Address
 * @param pbVal    : IO ��
 * @return  0      : SUCCESS
            else   : Device \Error �ڵ� 
 */
int MHilscherDnet::GetBit(CString strIOAddr, BOOL* pbVal)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	return GetBit(usIOAddr, pbVal);
}

/**
 * I/O Device�� Digital Status (Bit) �� �о�鿩 �����Ѵ�.
 * @precondition �� �Լ��� �����ϱ� ���� initialize �Լ��� �̸� ����Ǿ���� �Ѵ�.
 * @param usIOAddr : IO Address
 * @return  TRUE   : ON
            FALSE  : ON�� �ƴ�
 */
int MHilscherDnet::IsOn(CString strIOAddr, BOOL* pbVal)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	*pbVal = IsOn(usIOAddr);
	return ERR_IO_SUCCESS;
}

/**
  * I/O Device�� Digital Status (Bit) �� �о�鿩 �����Ѵ�.
  * @precondition �� �Լ��� �����ϱ� ���� initialize �Լ��� �̸� ����Ǿ���� �Ѵ�.
  * @param usIOAddr : IO Address
  * @return  TRUE   : OFF
	            FALSE  : OFF�� �ƴ� 
  */
int MHilscherDnet::IsOff(CString strIOAddr, BOOL* pbVal)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	*pbVal = IsOff(usIOAddr);
	return ERR_IO_SUCCESS;
}

/**
 * Output Device�� On Command (Bit = 1) �� ������.
 * @param usIOAddr : IO Address
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::OutputOn(CString strIOAddr)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	return OutputOn(usIOAddr);
}

/**
 * Output Device�� Off Command (Bit = 0) �� ������.
 * @param usIOAddr : IO Address
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::OutputOff(CString strIOAddr)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	return OutputOff(usIOAddr);
}

/**
 * Output Device�� Digital Status�� Set�̸� (Bit = 0), Output Device�� On Command (Bit = 1) �� ������,
 * Output Device�� Digital Status�� Clear�̸� (Bit = 1), Output Device�� Off Command (Bit = 0) �� ������.
 * @param usIOAddr : IO Address
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::OutputToggle(CString strIOAddr)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	return OutputToggle(usIOAddr);
}

/**
 * ���ӵ� 8���� IO Address�� ������ Input Device ���� Digital Status�� �о�鿩 pcValue pointer�� �Ѱ��ش�.
 * @param usIOAddr : ���ӵ� 8���� IO Address�� �����ϴ� IO Address
 * @param pcValuse : ���ӵ� 8���� IO Address�� ������ Input Device ���� Digital Status�� �о�鿩 pcValue�� �����Ѵ�.
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::GetByte(CString strIOAddr,  BYTE & pcValue)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	return GetByte(usIOAddr,  pcValue);
}

/**
 * ���ӵ� 8���� IO Address�� ������ Output Device�鿡 On or Off Command�� ������.
 * @param usIOAddr : ���ӵ� 8���� IO Address�� �����ϴ� IO Address
 * @param pcValuse : Output Device�� ���� Command�� �����ϰ� �ִ� �����̴�.
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::PutByte(CString strIOAddr,  BYTE pcValue)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	return PutByte(usIOAddr,  pcValue);
}

/**
 * ���ӵ� 16���� IO Address�� ������ Input Device ���� Digital Status�� �о�鿩 pcValue pointer�� �Ѱ��ش�.
 * @param usIOAddr : ���ӵ� 16���� IO Address�� �����ϴ� IO Address
 * @param pwValuse : ���ӵ� 16���� IO Address�� ������ Input Device ���� Digital Status�� �о�鿩 pcValue�� �����Ѵ�.
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::GetWord(CString strIOAddr,  WORD & pwValue)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	return GetWord(usIOAddr,  pwValue);
}

/**
 * ���ӵ� 16���� IO Address�� ������ Output Device�鿡 On or Off Command�� ������.
 * @param usIOAddr : ���ӵ� 16���� IO Address�� �����ϴ� IO Address
 * @param pwValuse : Output Device�� ���� Command�� �����ϰ� �ִ� �����̴�.
 * @return 0 = Success, �׿� = Error Number
 */
int MHilscherDnet::PutWord(CString strIOAddr,  WORD pwValue)
{
	unsigned short usIOAddr;

	if (IOAddrInterpreter(strIOAddr,  usIOAddr) == ERROR_ID)
	{
		return ERROR_ID;
	}

	return PutWord(usIOAddr,  pwValue);
}

/**
 * Incoming Buffer�� Update�ϰ�, Outgoing Buffer�� ������ Physical I/O�� �����ϴ� IOThread�� Run�Ѵ�.
 */
void MHilscherDnet::RunIOThread()
{
	ULONG	StartTime,  EndTime;
	int		iRet;
	BOOL	bVal;
	double	ScanTime;

	StartTime = GetTickCount();
	while (TRUE)
	{
		iRet = GetBit(INPUT_ORIGIN, &bVal); // Must Check
		if (iRet == ERR_IO_SUCCESS)
			break;

		EndTime = GetTickCount();
		EndTime -= StartTime;
		ScanTime = (double)EndTime / (double)CLK_TCK;

		if (ScanTime >= 10.0)
		{
			//17500 = Device Net�� Message�� Reading�ϴµ� Error�� �߻��Ͽ����ϴ�.
//			MyMessageBox(17500,  "Device Net Message Reading Error",  M_ICONERROR);
//			return ERROR_ID;
		}
	}

	Sleep(500);

	m_hHandle = CreateThread(NULL, 0, EntryPoint, (LPVOID)this, 0, m_lpthreadID);
}

/**
 * Thread�ν� InputData[]�� Update�ϰ�, OutputData[]�� Physical I/O�� �����Ѵ�.
 */	
UINT MHilscherDnet::IOThread(LPVOID lParam)
{
	MHilscherDnet* This = (MHilscherDnet*) lParam;
	
	/** ó���� �ѹ� Devicenet ��⿡�� Output ������ �о�´�. */
	/** ó���� �߸��� ����� ������ ���� ���� �Ѵ�. */
	DevReadSendData(This->m_usBoardNumber, 0, MAX_DEVICE * 4, &This->m_ucOutgoingBuffer[0]);

	while (This->m_bDeviceOpened)
	{
#ifndef SIMULATION
		dnStatusCheck();

		/*110612.KKY_____
		DevExchangeIO(This->m_usBoardNumber, 
					  0, 							// usSendOffset
					  MAX_DEVICE * 4, 				// usSendSize
					  &This->m_ucOutgoingBuffer[0], // *pvSendData
					  0, 							// usReceiveOffset
					  MAX_DEVICE * 4, 				// usReceiveSize
					  &This->m_ucIncomingBuffer[0], // *pvReceiveData
					  100L);						// ulTimeout
		*/
		UpdateIO();
		//_______________
#endif

		Sleep(10);
	}

	TRACE(_T("IO Thread Stop\n"));
	return ERR_IO_SUCCESS;
}

/**
 * Master ��� �� Slave ��� ���¸� ��ȯ�Ѵ�
 *
 * @param ubMacID : ��� Slave Mac Address
          -1      : ��� Slave üũ (Default)
 * @return 0		= SUCCESS
           others	= ERROR Code
 */
int MHilscherDnet::dnStatusCheck(unsigned char ucMacID)
{
#define IO_STS_OFFSET	8

	int				nSlaveArray;			// Slave �ּ� 0 ~ 7�׷�
	int				nSlaveLocate;			// Slave �ּ� �׷�� 0 ~ 7��
	unsigned char	cCheckbyte;				// Slave�� Bit üũ ����	
	unsigned char	cBitMask;				// Bit Mask 
	int				iRet = ERR_IO_SUCCESS;	// ���� Value
	DNM_DIAGNOSTICS	stDnmDiagonstics;

	if (ucMacID == 255) // All check
	{
		// DeviceNet�� �����ϴ� API ȣ��
		if (DevGetTaskState(m_usBoardNumber, 2, sizeof(DNM_DIAGNOSTICS), &stDnmDiagonstics)
			 != DRV_NO_ERROR)
		{
			m_tblDnStatus.dnm_status = STS_FAIL;
			// 100009 = Update �۾��� �����߽��ϴ�.
			return generateErrorCode(100009);
		}
		else
		{
			m_tblDnStatus.dnm_status = STS_NORMAL;
			for (nSlaveArray = 0; nSlaveArray < 8; nSlaveArray++)
			{
				if (stDnmDiagonstics.abDv_cfg[nSlaveArray] 
					!= stDnmDiagonstics.abDv_state[IO_STS_OFFSET + nSlaveArray])
				{
					cCheckbyte = (stDnmDiagonstics.abDv_cfg[nSlaveArray] 
									^ stDnmDiagonstics.abDv_state[IO_STS_OFFSET + nSlaveArray]);
					for (nSlaveLocate = 0; nSlaveLocate < 8; nSlaveLocate++)
					{
						cBitMask = 0x1 << (nSlaveLocate);
						if (cCheckbyte & cBitMask)	// �ش� ��Ʈ�� ������
						{
							m_tblDnStatus.dns_status[nSlaveArray * 8 + nSlaveLocate] = STS_FAIL;
							iRet = ERR_IO_MODULE_FAIL;
						}
						else
						{
							m_tblDnStatus.dns_status[nSlaveArray * 8 + nSlaveLocate] = STS_NORMAL;
						}
					}
				}
				else
				{
					for (nSlaveLocate = 0; nSlaveLocate < 8; nSlaveLocate++)
					{
						m_tblDnStatus.dns_status[nSlaveArray*8+nSlaveLocate] = STS_NORMAL;
					}
				}
			}
		}
	}
	else // MacID check
	{
		nSlaveArray = ucMacID / 8;
		nSlaveLocate = ucMacID % 8;
		
		if (DevGetTaskState(m_usBoardNumber, 2, sizeof(DNM_DIAGNOSTICS), &stDnmDiagonstics) != DRV_NO_ERROR)
		{	
			m_tblDnStatus.dnm_status = STS_FAIL;
			// 100009 = Update �۾��� �����߽��ϴ�.
			return generateErrorCode(100009);
		}
		else if (stDnmDiagonstics.abDv_cfg[nSlaveArray] != stDnmDiagonstics.abDv_state[IO_STS_OFFSET + nSlaveArray])
		{
			cBitMask = 0x1 << (nSlaveLocate);
			cCheckbyte = (stDnmDiagonstics.abDv_cfg[nSlaveArray] 
						^ stDnmDiagonstics.abDv_state[IO_STS_OFFSET+nSlaveArray]);
			if (cCheckbyte & cBitMask)
				iRet = ERR_IO_MODULE_FAIL;
		}
		m_tblDnStatus.dnm_status = STS_NORMAL;
	}
	return iRet;
}

/**
 * Master ��� �� Slave ��� ���� ������ ���´�.
 *
 * @param  DnStatus : �����Ϳ� 64���� Slave�� ���� ���� ���� ����ü
 * @return 0		= ��� ����
           others	= �ϳ��� ����
 */
int MHilscherDnet::DnStatusGet(DN_STATUS DnStatus)
{
	memcpy(&DnStatus, &m_tblDnStatus, sizeof(m_tblDnStatus));

	if (m_tblDnStatus.dnm_status == STS_FAIL)
	{
		// 100009 = Update �۾��� �����߽��ϴ�.
		return generateErrorCode(100009);
	}
	else
	{
		for (int nSlave = 0; nSlave < MAX_DEVICE; nSlave++)
		{
			if (m_tblDnStatus.dns_status[nSlave] == STS_FAIL)
			{
				// 100010 = Module Fail �� �߻��߽��ϴ�.
				return generateErrorCode(100010);
			}
		}
	}

	return ERR_IO_SUCCESS;
}

DWORD WINAPI MHilscherDnet::EntryPoint(LPVOID pParam)
{
	MHilscherDnet* pSelf = (MHilscherDnet*)pParam;
	return pSelf->IOThread(pParam);
}

/**
 * Master ��� �� Slave ��� ���� üũ
 *
 * @return 0		= SUCCESS
		   others	= ERROR Code
 */
int MHilscherDnet::returnDnetStatus()
{
	if (m_tblDnStatus.dnm_status == STS_FAIL)
	{
		// 100009 = Update �۾��� �����߽��ϴ�.
		return generateErrorCode(100009);
	}

#if FALSE		// 2004.9.3 Slave Module ���� Ȯ���ϸ鼭 ���� �߻��Ͽ� �ϴ� ����. (PCB ��� ���� ���� ���� ������)
	else
	{
		for (int nSlave = 0; nSlave < MAX_DEVICE; nSlave++)
		{
			if (m_tblDnStatus.dns_status[nSlave] == STS_FAIL)
			{
				SetErrorLevel(_T("HILSCHERDNET return Module State"),3, __FILE__, __LINE__);
				// 100010 = Module Fail �� �߻��߽��ϴ�.
				return generateErrorCode(100010);
			}
		}
	}
#endif

	return ERR_IO_SUCCESS;
}

BOOL MHilscherDnet::LoadIOMap(CString FileName)
{
	WCHAR buffer[_MAX_PATH];
	CString strFileName;
	FILE* InFile;
	int numclosed;
	int ch;
	CString	FormattedMessage;
	CString	Type;
	CString	Name;
	CString	Address;
	static int	inputAddressCount = 0;
	static int	outputAddressCount = 0;
	
	memset(InputIOTable, 0x00,  sizeof(InputIOTable));
	memset(OutputIOTable, 0x00,  sizeof(OutputIOTable));
	
	/* Get the current working directory: */
//	if (_getcwd(buffer,  _MAX_PATH) == NULL)
	if (_wgetcwd(buffer,  _MAX_PATH) == NULL)
		perror("_getcwd error");
	else
		printf("%s\n", LPSTR(LPCTSTR(buffer)));
	
	// IO Map File Name
//	strFileName.Format("%s/Component/System/%s", buffer, FileName) ;
	strFileName.Format(_T("%s/Info/%s"), buffer, FileName) ;
	
	/* Open for read (will fail if file "data" does not exist) */
	//if ((InFile  = fopen(strFileName,  "r")) == NULL)
	if ((InFile  = _tfopen(strFileName,  _T("r"))) == NULL)
//	if ((InFile  = _wfopen(strFileName,  _T("r"))) == NULL)
	{
		printf("The file 'data' was not opened\n");
	}
	else
		printf("The file 'data' was opened\n");
	
	while (!feof(InFile))
	{
		memset(buffer,  0x00,  sizeof(buffer));
		
		ch = fgetc(InFile);
		for (int i = 0; (ch != '\n') & (feof(InFile) == 0); i++)
		{
			buffer[i] = (char)ch;
			ch = fgetc(InFile);
		}
		
		FormattedMessage = buffer;
		FormattedMessage.TrimLeft();
		FormattedMessage.TrimRight();
		
		int n = FormattedMessage.Find(';', 0);
		if (n != -1)
			continue;
		else
		{
			int iType = FormattedMessage.Find(',', 0);
			Type = FormattedMessage.Left(iType);
			Type.TrimLeft();
			Type.TrimRight();
			
			int iName = FormattedMessage.Find(',', iType + 1);
			
			Name = FormattedMessage.Mid(iType + 1,  iName - iType - 1);
			Name.TrimLeft();
			Name.TrimRight();
			
			
			Address = FormattedMessage.Mid(iName + 1);
			Address.TrimLeft();
			Address.TrimRight();
			
			
			//if (!strcmp(Type, "INPUT"))
			if (!_tcscmp(Type, _T("INPUT")))
			{
				InputIOTable[inputAddressCount].Type = INPUT_ADDRESS;
				//strcpy(InputIOTable[inputAddressCount].Name, Name);
				_tcscpy(InputIOTable[inputAddressCount].Name, Name);
				//InputIOTable[inputAddressCount].Address = atoi(Address);
				InputIOTable[inputAddressCount].Address = _wtoi(Address);
				//InputIOTable[inputAddressCount].Index = (atoi(Address) - INPUT_BASE_ADDRESS) % (CHAR_BIT * 2);
				InputIOTable[inputAddressCount].Index = (_wtoi(Address) - INPUT_BASE_ADDRESS) % (CHAR_BIT * 2);
				
				inputAddressCount++;
				
			}
			//else if (!strcmp(Type, "OUTPUT"))
			else if (!_tcscmp(Type, _T("OUTPUT")))
			{
				OutputIOTable[outputAddressCount].Type = OUTPUT_ADDRESS;
				//strcpy(OutputIOTable[outputAddressCount].Name, Name);
				//OutputIOTable[outputAddressCount].Address = atoi(Address);
				//OutputIOTable[outputAddressCount].Index = (atoi(Address) - OUTPUT_BASE_ADDRESS) % (CHAR_BIT * 2);
				_tcscpy(OutputIOTable[outputAddressCount].Name, Name);
				OutputIOTable[outputAddressCount].Address = _wtoi(Address);
				OutputIOTable[outputAddressCount].Index = (_wtoi(Address) - OUTPUT_BASE_ADDRESS) % (CHAR_BIT * 2);
				
				outputAddressCount++;
			}
		}
    }
	
	/* Close stream */
	if (fclose(InFile))
		printf("The file 'data' was not closed\n");
	
	/* All other files are closed: */
	numclosed = _fcloseall();
	printf("Number of files closed by _fcloseall: %u\n", numclosed);
	return ERR_IO_SUCCESS;
}

int MHilscherDnet::GetIOTable(SIOTABLE* Input, SIOTABLE* Output)
{
	for (int i = 0 ; i < INPUT_MAX_NUM_ADDRESS; i++)
		*(Input + i) = InputIOTable[i];
	for (int i = 0 ; i < OUTPUT_MAX_NUM_ADDRESS ; i++)
		*(Output + i) = OutputIOTable[i];

	return ERR_IO_SUCCESS;
}

int MHilscherDnet::SendExplicitMsg(MSG_ID_STRUC* psMsgID, MSG_STRUC* psReceiveMsg)
{
	unsigned short usSize = 288;

	if (psMsgID->srvLen > MAX_EXPLICIT_LEN)
	{
		// 100011 = Explicit Message Data ���̰� ����(239 Byte) ������ �ʰ��մϴ�.
		return	generateErrorCode(100011);
	}

	if (psMsgID->p_srvBuffer == NULL)
	{
		// 100012 = Explicit Message �� ������ Buffer Pointer �� Null Pointer �Դϴ�.
		return generateErrorCode(100012);
	}

	MSG_STRUC tSendMessage;
	//********************************
	//* service data
	for (unsigned int data_len= 0; data_len<psMsgID->srvLen; data_len++)
	{
		tSendMessage.data[8+ data_len] = psMsgID->p_srvBuffer[data_len];
	}

	//***************************************
	//* Message Header
	tSendMessage.rx = 3;
	tSendMessage.tx = 16;
	tSendMessage.ln = 8 + psMsgID->srvLen;		
	tSendMessage.nr = getMessageId();
	tSendMessage.a = 0;
	tSendMessage.f = 0;
	tSendMessage.b = 79;	//command = DNM_Get_Set_Attribute	
	tSendMessage.e = 0;
	//***************************************
	//* extended message header
	tSendMessage.data[0] = psMsgID->mac;					// MAC_ID	
	tSendMessage.data[1] = psMsgID->clss;					// Class_ID
	tSendMessage.data[2] = (BYTE)psMsgID->instance;			// Instance_ID low
	tSendMessage.data[3] = (BYTE)(psMsgID->instance >> 8);	// Instance_ID high
	tSendMessage.data[4] = 0;
	tSendMessage.data[5] = psMsgID->srvLen;					// service data length
	tSendMessage.data[6] = 0;								// data type
	tSendMessage.data[7] = psMsgID->service_code;

	//****************************
	//* send message box
	//****************************
	unsigned short usDevMBX, usHostMBX;
	double dStartTime = GetTickCount();
	do
	{
		DevGetMBXState(m_usBoardNumber, &usDevMBX, &usHostMBX);
		if (usDevMBX == 0)	// empty
			break;
	} while ((GetTickCount() - dStartTime) < 3000);

	DevGetMBXState(m_usBoardNumber, &usDevMBX, &usHostMBX);
	if (usDevMBX != 0)		// empty
	{
		// 100017 = Tx Time Out �� �߻��߽��ϴ�.
		return generateErrorCode(100017);
	}

	short sRet = DevPutMessage(m_usBoardNumber, &tSendMessage, 1000);

	//****************************
	//* receive message box
	//****************************
	bool bTimeoutFlag = true;
	do
	{
		DevGetMBXState(m_usBoardNumber, &usDevMBX, &usHostMBX);
		if (usHostMBX == 1)	// full
		{
			sRet = DevGetMessage(m_usBoardNumber, usSize, psReceiveMsg, 1000);
			if (sRet == DRV_NO_ERROR)
			{
				if (psReceiveMsg->nr == tSendMessage.nr)
				{
					bTimeoutFlag = false;
					break;
				}
			}
		}
	} while (GetTickCount()-dStartTime < 50000);

	if (bTimeoutFlag == true)
	{
		// 100018 = Rx Time Out �� �߻��߽��ϴ�.
		return generateErrorCode(100018);
	}

	//****************************
	//* check received message
	if (sRet != DRV_NO_ERROR)
	{
		// 100013 = Message Receive �۾��� ���������� �Ϸ���� �ʾҽ��ϴ�.
		return generateErrorCode(100013);
	}

	if (psReceiveMsg->f)
		return setExplicitErrMessage(psReceiveMsg->f);

	if ((psReceiveMsg->a != 79) 
		|| (psReceiveMsg->data[0] != tSendMessage.data[0])	// MAC ID
		|| (psReceiveMsg->data[1] != tSendMessage.data[1])	// Class ID
		|| (psReceiveMsg->data[2] != tSendMessage.data[2])	// Instance ID low
		|| (psReceiveMsg->data[3] != tSendMessage.data[3])	// Instance ID high
		|| (psReceiveMsg->data[7] != tSendMessage.data[7]))	// function code
	{
		// 100014 = �о���� Receive Message �� Send Message �� ���� Response �� �ƴմϴ�.
		return generateErrorCode(100014);
	}

	if (psReceiveMsg->data[5] > MAX_EXPLICIT_LEN)
	{
		// 100015 = Received Message Data ���̰� ����(239 Byte) ������ �ʰ��մϴ�.
		return generateErrorCode(100015);
	}

	if (psReceiveMsg->ln < 8)
	{
		// 100016 = Received Message ��ü ���̰� 8 Byte ���� �۽��ϴ�.
		return generateErrorCode(100016);
	}

	psMsgID->srvLen = psReceiveMsg->ln - 8;
	for (unsigned int i = 0; i < psMsgID->srvLen; i++)
		 psMsgID->p_srvBuffer[i] = psReceiveMsg->data[8+i];

	return DRV_NO_ERROR;
}

BYTE MHilscherDnet::getMessageId(void)
{
	++m_bMsgId;
	if (m_bMsgId == 255)
		m_bMsgId = 0;

	return m_bMsgId;
}

int MHilscherDnet::setExplicitErrMessage(unsigned char revF)
{
	int iErrorCode = 0;

	switch (revF)
	{	
	case 0:		// No Error
		return	0;
	case 2:
		// 100021 = resources unavailable [IO Error: 2]
		iErrorCode = 100021;
		break;
	case 8:
		// 100022 = service not avaliable [IO Eerror: 8]
		iErrorCode = 100022;
		break;
	case 9:
		// 100023 = invalid attribute value [IO Error: 9] 
		iErrorCode = 100023;
		break;
	case 11:
		// 100024 = already in request mode [IO Error: 11]
		iErrorCode = 100024;
		break;
	case 12:
		// 100025 = object state conflict [IO Error: 12]
		iErrorCode = 100025;
		break;
	case 14:
		// 100026 = attribute not settable [IO Error: 14]
		iErrorCode = 100026;
		break;
	case 15:
		// 100027 = a permission check failed [IO Error:  15]
		iErrorCode = 100027;
		break;
	case 16:
		// 100028 = state conflict [IO Error: 16]
		iErrorCode = 100028;
		break;
	case 17:
		// 100029 = no response from the network device [IO Error: 17]
		iErrorCode = 100029;
		break;
	case 19:
		// 100030 = not enough data [IO Error: 19]
		iErrorCode = 100030;
		break;
	case 20:
		// 100031 = attribute not supported [IO Error: 20]
		iErrorCode = 100031;
		break;
	case 21:
		// 100032 = too much data [IO Error: 21] 
		iErrorCode = 100032;
		break;
	case 22:
		// 100033 = object does not exists [IO Error: 22] 
		iErrorCode = 100033;
		break;
	case 23:
		// 100034 = reply data too large [IO Error: 23] 
		iErrorCode = 100034;
		break;
	case 48:
		// 100035 = device not configured, Explicit not established [IO Error: 48]
		iErrorCode = 100035;
		break;
	case 50:
		// 100036 = format error in response telegram [IO Error: 50] 
		iErrorCode = 100036;
		break;
	case 54:
		// 100037 = another get or set command still active [IO Error: 54] 
		iErrorCode = 100037;
		break;
	case 55:
		// 100038 = MAC ID out of range [IO Error: 55] 
		iErrorCode = 100038;
		break;
	case 57:
		// 100039 = sequence error in fragmented [IO Error: 57] 
		iErrorCode = 100039;
		break;
	case 200:
		// 100040 = not confiqured no data base found [IO Error: 200]
		iErrorCode = 100040;
		break;
	default:
		// 100041 = unknown error
		iErrorCode = 100041;
		break;
	}

	return	generateErrorCode(iErrorCode);
}

//110613.KKY_____
int MHilscherDnet::UpdateIO()
{
	DevExchangeIO(m_usBoardNumber, 
				  0, 							// usSendOffset
				  MAX_DEVICE * 4, 				// usSendSize
				  &m_ucOutgoingBuffer[0],		// *pvSendData
				  0, 							// usReceiveOffset
				  MAX_DEVICE * 4, 				// usReceiveSize
				  &m_ucIncomingBuffer[0],		// *pvReceiveData
				  100L);						// ulTimeout
	return ERR_IO_SUCCESS;
}
//_______________