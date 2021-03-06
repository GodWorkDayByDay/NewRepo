/* Generated by Together */

#ifndef MDACONVERTER_H
#define MDACONVERTER_H

#include "IDAConverter.h"

class MDAConverter : public IDAConverter
{
private:
	int m_iDAConverterID;
	CString m_strDAConverterName;
   
	/** 생성된 DAConverter의 출력 시작 Bit */
	unsigned short m_usOStarBit;
	/** 생성된 DAConverter의 출력 BitSize */
	unsigned short m_usOBitSize;
	/** 생성된 DAConverter의 출력 Bit 배열.  */
	unsigned short m_rgusOBitAddress[DEF_MAX_DACONVERTER_BIT];

	/** 생성된 DAConverter의 입력시작 Bit */
	unsigned short m_usIStarBit;
	/** 생성된 DAConverter의 입력 BitSize */

	unsigned short m_usIBitSize;
	/** 생성된 DAConverter의 입력 Bit 배열.  */
	unsigned short m_rgusIBitAddress[DEF_MAX_DACONVERTER_BIT];

	/** 현재 적용된 값	 */
	unsigned short m_usCurValue;

	/** DA Type   */
	EDAConverterType m_eDAConverterType;

	/** DAConverter Data가 저장될 File Name */
	CString m_strFileName;

	/** DAConverter Data가 저장될 File Path */
	CString m_strFilePath;

	MDAConverterData m_DAConverterParameter;
	
public:
	/** Default 생성자 */
	MDAConverter();

	/** 
	 * Constructor
	 * 
	 * @param	commonData : 공통적으로 사용할 Data
	 * @param	pIIO : 참조할 I/O Component 포인터
	 * @param	datComponent : 자체적으로 사용할 Data
	 */
	MDAConverter( SCommonAttribute commonData, IIO	*pIIO, SDAConverterData datComponent );

	/** 소멸자 */
	virtual ~MDAConverter();

	/** Regular Value return */
	int GetValue();
  

    /** RegualaValue 값으로 동작시킴  */
    int SetValue(unsigned short val);

	/** 원시 Data로 Set하는 함수*/
	int SetDigit(unsigned short val);
 
	
/*------------------------ Common Interface -----------------------*/
    /**
	 * Assign component linkage
	 * 
	 * @param	pIO	: 참조할 IO Component Pointer
	 */
	int AssignComponents(IIO *pIO);

    /** 
	 * Component Data 처리 
	 *
	 * @param	datComponent : 기록할 데이타
	 * @return	Error : 0 = Success, 그외=기타
	 */
	int SetData(SDAConverterData datComponent);

    /** 
	 * Component Data 읽어올 데이타 인터 
	 *
	 * @param	datComponent : MDAConverterData Class에 대한 포인터
	 * @return	Error : 0 = Success, 그외=기타
	 */
	int GetData(SDAConverterData *datComponent);
 
 
	/**
	 * DAConverter 관련 Data를 File에서 읽어온다.
	 *
	 * @param	pdaData (읽어올 Data가 저장될 변수)
	 * @return	int (0 = Success, Error Code = 그 외)
	 */
	int LoadParameter();


	/**
	 * DAConverter 관련 Data를 File에 저장한다.
	 *
	 * @param	pdaData (저장할 Data)
	 * @return	int (0 = Success, Error Code = 그 외)
	 */
	int SaveParameter(MDAConverterData* pdaData);

	
	/**
	 * DAConverter 관련 Data를 얻어 온다.
	 *
	 * @param	pdaData (읽어올 Data가 저장될 변수)
	 * @return	int (0 = Success, Error Code = 그 외)
	 */
	void GetParameter(MDAConverterData* pdaData);

	/**
	 * DAConverter 이름을 Return 한다.
	 * @return : DAConverter Name
	 */
	CString GetDAConverterName();

};
#endif //MDACONVERTER_H
