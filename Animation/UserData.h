//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	UserData
//--------------------------------------------------------------------------------------------
//	�������� � ������ � ���������������� ������
//============================================================================================

#ifndef _UserData_H_
#define _UserData_H_


#include "..\Common_h\d_types.h"
#include "..\Common_h\memop.h"
#include "..\Common_h\exs.h"


class UserData  
{
	struct Data
	{
		long hashCode;		//�������� �������� �����
		long name;			//������ ������ �����
		long data;			//������ ������ ������ ������
	};

//--------------------------------------------------------------------------------------------
//���������������, ������������������
//--------------------------------------------------------------------------------------------
public:
	UserData();
	virtual ~UserData();
	
//--------------------------------------------------------------------------------------------
//������ � �������
//--------------------------------------------------------------------------------------------
public:
	//�������� ������
	void AddData(const char * dname, const char * sdata);
	//���������� ������ ������ ����� ���������� ������
	void FreeExtra();
	//����� ���������������� ������
	const char * GetData(const char * dataName);

//--------------------------------------------------------------------------------------------
//������������
//--------------------------------------------------------------------------------------------
private:
	static long CalcHashString(const char * str);

private:
	Data * data;					//������� ������
	long numData;					//����� ������
	long maxData;					//������ �������
	
	char * strings;					//������ �����
	long numBytes;					//���������� ������������ ����
	long maxBytes;					//������ �������
};

#endif
