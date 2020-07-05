//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	WdmDateLabel
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _WdmDateLabel_H_
#define _WdmDateLabel_H_

#include "WdmInterfaceObject.h"

class WdmDateLabel : public WdmInterfaceObject
{


//--------------------------------------------------------------------------------------------
//���������������, ������������������
//--------------------------------------------------------------------------------------------
public:
	WdmDateLabel();
	virtual ~WdmDateLabel();

	void SetAttributes(ATTRIBUTES * apnt);
	//���������
	virtual void LRender(VDX8RENDER * rs);

//--------------------------------------------------------------------------------------------
//������������
//--------------------------------------------------------------------------------------------
private:
	long txTextBar;
	long txSkyClock;
	long txSkyClockFrame;
	long dateFont;
	char month[12][32];
};

#endif
