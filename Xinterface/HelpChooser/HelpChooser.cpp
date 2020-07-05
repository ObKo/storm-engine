#include "HelpChooser.h"
#include "..\\defines.h"

#define HCHOOSER_FVF	(D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)
struct HCHOOSER_VERTEX
{
	CVECTOR pos;
	float w;
	float tu,tv;
};

extern char * XI_ParseStr(char * inStr, char * buf, size_t bufSize, char devChar=',');

HELPCHOOSER::HELPCHOOSER()
{
	rs = null;
	m_idMouseTexture = -1;
	m_idPicTexture = -1;
	m_idBackTexture = -1;
	m_idVBuf = -1;
	m_pRectList = null;
	m_nRectQ = 0;
	m_psRectName = null;
}

HELPCHOOSER::~HELPCHOOSER()
{
	AllRelease();
}

void HELPCHOOSER::SetDevice()
{
    // �������� ������ �������
	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs){_THROW("No service: dx8render")}
}

bool HELPCHOOSER::Init()
{
	GUARD(HELPCHOOSER::Init())
	SetDevice();
	UNGUARD
	return true;
}

void HELPCHOOSER::Execute(dword Delta_Time)
{
	long newCurRect;
	CONTROL_STATE cs;

	bool bMouseMoved = MouseMove();

	api->Controls->GetControlState("HelpChooser_Cancel",cs);
	if(cs.state == CST_ACTIVATED)
	{
		api->Event("EventEndHelpChooser","s","");
		return;
	}

	api->Controls->GetControlState("HelpChooser_Action",cs);
	if(cs.state == CST_ACTIVATED)
	{
		if(m_nCurRect>=0 && m_nCurRect<m_nRectQ && m_psRectName!=null)
		{
			api->Event("EventEndHelpChooser","s",m_psRectName[m_nCurRect]);
		}
		return;
	}

	api->Controls->GetControlState("HelpChooser_Next",cs);
	if(cs.state == CST_ACTIVATED)
	{
		if(m_nCurRect<m_nRectQ-1) SetRectangle(m_nCurRect+1);
		else SetRectangle(0);
		return;
	}

	api->Controls->GetControlState("HelpChooser_Prev",cs);
	if(cs.state == CST_ACTIVATED)
	{
		if(m_nCurRect>0) SetRectangle(m_nCurRect-1);
		else SetRectangle(m_nRectQ-1);
		return;
	}

	api->Controls->GetControlState("HelpChooser_Left",cs);
	if(cs.state == CST_ACTIVATED)
	{
		SetRectangle(GetRectangleLeft());
		return;
	}

	api->Controls->GetControlState("HelpChooser_Right",cs);
	if(cs.state == CST_ACTIVATED)
	{
		SetRectangle(GetRectangleRight());
		return;
	}

	api->Controls->GetControlState("HelpChooser_Up",cs);
	if(cs.state == CST_ACTIVATED)
	{
		SetRectangle(GetRectangleUp());
		return;
	}

	api->Controls->GetControlState("HelpChooser_Down",cs);
	if(cs.state == CST_ACTIVATED)
	{
		SetRectangle(GetRectangleDown());
		return;
	}

	api->Controls->GetControlState("HelpChooser_LeftClick",cs);
	if(cs.state == CST_ACTIVATED)
	{
		newCurRect = GetRectangleFromPos(m_fCurMouseX,m_fCurMouseY);
		if(newCurRect>=0 && newCurRect<m_nRectQ)
		{
			api->Event("EventEndHelpChooser","s",m_psRectName[newCurRect]);
			return;
		}
	}

	if(!bMouseMoved) return;
	newCurRect = GetRectangleFromPos(m_fCurMouseX,m_fCurMouseY);
	if(newCurRect>=0 && newCurRect!=m_nCurRect)
	{
		SetRectangle(newCurRect);
		return;
	}

	if(m_idVBuf==-1) return;
	HCHOOSER_VERTEX * pv = (HCHOOSER_VERTEX *)rs->LockVertexBuffer(m_idVBuf);
	if(pv==null) return;
	pv[14].pos.x = pv[15].pos.x = m_fCurMouseX - m_nMouseCornerX;
	pv[16].pos.x = pv[17].pos.x = m_fCurMouseX - m_nMouseCornerX + m_nMouseWidth;
	pv[14].pos.y = pv[16].pos.y = m_fCurMouseY - m_nMouseCornerY;
	pv[15].pos.y = pv[17].pos.y = m_fCurMouseY - m_nMouseCornerY + m_nMouseHeight;
	rs->UnLockVertexBuffer(m_idVBuf);
}

void HELPCHOOSER::Realize(dword Delta_Time)
{
	if(m_idVBuf==-1) return;

	if(m_idBackTexture!=-1)
	{
		rs->TextureSet(0,m_idBackTexture);
		rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBuf,sizeof(HCHOOSER_VERTEX),0,8,"iHelpChooser");
	}
	if(m_idPicTexture!=-1)
	{
		rs->TextureSet(0,m_idPicTexture);
		rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBuf,sizeof(HCHOOSER_VERTEX),10,2,"iHelpChooser");
	}

	if(m_idMouseTexture!=-1)
	{
		rs->TextureSet(0,m_idMouseTexture);
		rs->DrawPrimitive(D3DPT_TRIANGLESTRIP,m_idVBuf,sizeof(HCHOOSER_VERTEX),14,2,"iHelpChooser");
	}
}

dword _cdecl HELPCHOOSER::ProcessMessage(MESSAGE & message)
{
	switch(message.Long())
	{
	case MSG_HELPCHOOSER_START:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			return RunChooser(param);
		}
		break;
	}
	return 0;
}

void HELPCHOOSER::AllRelease()
{
	TEXTURE_RELEASE(rs,m_idMouseTexture);
	TEXTURE_RELEASE(rs,m_idPicTexture);
	TEXTURE_RELEASE(rs,m_idBackTexture);
	VERTEX_BUF_RELEASE(rs,m_idVBuf);
	PTR_DELETE(m_pRectList);
	for(int i=0; i<m_nRectQ; i++)
	{
		PTR_DELETE(m_psRectName[i]);
	}
	m_nRectQ = 0;
	PTR_DELETE(m_psRectName);
}

bool HELPCHOOSER::RunChooser(char * ChooserGroup)
{
	int i,j;
	INIFILE * ini;
	char	param[512];
	char	param2[256];
	float	texWidth,texHeight;

	AllRelease();

	if(ChooserGroup==null) return false;
	ini = api->fio->OpenIniFile("resource\\ini\\helpchooser.ini");
	if(ini==null)
	{
		api->Trace("Can`t open INI file \"resource\\ini\\helpchooser.ini\"");
		return false;
	}

	// ������� ������ �������
	texWidth = ini->GetFloat(ChooserGroup,"TextureWidth",512.f);
	texHeight = ini->GetFloat(ChooserGroup,"TextureHeight",512.f);

	// ������� ������ ����������� ������ (������ ����)
	IDirect3DSurface8 * pRenderTarget;
	rs->GetRenderTarget(&pRenderTarget);
	D3DSURFACE_DESC dscrSurface;
	pRenderTarget->GetDesc(&dscrSurface);
	m_fScreenWidth = (float)dscrSurface.Width;
	m_fScreenHeight = (float)dscrSurface.Height;
	pRenderTarget->Release();

	// �������� ��������� ��������
	if( ini->ReadString(ChooserGroup,"FrontTexture",param,sizeof(param)-1,"") )
		m_idPicTexture = rs->TextureCreate(param);

	// �������� �����������(�������) ��������
	if( ini->ReadString(ChooserGroup,"BackTexture",param,sizeof(param)-1,"") )
		m_idBackTexture = rs->TextureCreate(param);

	// ������� ����� ��������������� ��� ������ ������
	m_nRectQ = 0;
	if( ini->ReadString(ChooserGroup,"rect",param,sizeof(param)-1,"") )
		do	m_nRectQ++;
		while( ini->ReadStringNext(ChooserGroup,"rect",param,sizeof(param)-1) );
	// ������� ������ ��������� ��������������� ������
	if(m_nRectQ>0)
	{
		m_pRectList = NEW FRECT[m_nRectQ];
		m_psRectName = NEW char*[m_nRectQ];
		if(m_pRectList==null || m_psRectName==null)	{THROW("Allocate memory error");}
	}
	// ��������� ��� ��������������
	ini->ReadString(ChooserGroup,"rect",param,sizeof(param)-1,"");
	for(i=0; i<m_nRectQ; i++)
	{
		m_psRectName[i] = null;
		char * tmpStr = param;

		tmpStr = XI_ParseStr(tmpStr,param2,sizeof(param2));
		for(j=strlen(param2)-1; j>=0; j--)
			if(param2[j] == ' ') param2[j]=0;
			else break;
		if(j>0)
		{
			m_psRectName[i] = NEW char[j+2];
			if(m_psRectName[i]==null) {_THROW("Allocate memory error");}
			strcpy(m_psRectName[i],param2);
		}

		tmpStr = XI_ParseStr(tmpStr,param2,sizeof(param2));
		m_pRectList[i].left = (float)atof(param2) / texWidth;
		tmpStr = XI_ParseStr(tmpStr,param2,sizeof(param2));
		m_pRectList[i].top = (float)atof(param2) / texHeight;
		tmpStr = XI_ParseStr(tmpStr,param2,sizeof(param2));
		m_pRectList[i].right = (float)atof(param2) / texWidth;
		tmpStr = XI_ParseStr(tmpStr,param2,sizeof(param2));
		m_pRectList[i].bottom = (float)atof(param2) / texHeight;

		ini->ReadStringNext(ChooserGroup,"rect",param,sizeof(param)-1);
	}

	// ��������� ����
	m_fCurMouseX = 0.f;		m_fCurMouseY = 0.f;
	m_nMouseWidth = ini->GetLong("COMMON","mouseWidth",32);
	m_nMouseHeight = ini->GetLong("COMMON","mouseHeight",32);
	m_nMouseCornerX = ini->GetLong("COMMON","mouseCornerX",0);
	m_nMouseCornerY = ini->GetLong("COMMON","mouseCornerY",0);
#ifndef _XBOX
	if(m_nMouseWidth>0 && m_nMouseHeight>0)
		if( ini->ReadString("COMMON","mouseTexture",param,sizeof(param)-1,"") )
			m_idMouseTexture = rs->TextureCreate(param);
#endif

	// ������� ����� ���������
	m_idVBuf = rs->CreateVertexBuffer(HCHOOSER_FVF,18*sizeof(HCHOOSER_VERTEX),D3DUSAGE_WRITEONLY);
	if(m_idVBuf==-1)	api->Trace("WARNING! Can`t create vertex buffer for help chooser");
	else
	{
		HCHOOSER_VERTEX * pv = (HCHOOSER_VERTEX *)rs->LockVertexBuffer(m_idVBuf);
		if(pv!=null)
		{
			for(i=0; i<18; i++)
			{
				pv[i].pos.z = 1.f;
				pv[i].w = 0.5f;
			}

			pv[0].pos.x = 0.f;							pv[0].pos.y = 0.f;
			pv[2].pos.x = (float)dscrSurface.Width;		pv[2].pos.y = 0.f;
			pv[4].pos.x = (float)dscrSurface.Width;		pv[4].pos.y = (float)dscrSurface.Height;
			pv[6].pos.x = 0.f;							pv[6].pos.y = (float)dscrSurface.Height;
			pv[8].pos.x = 0.f;							pv[8].pos.y = 0.f;

			pv[0].tu = 0.f;		pv[0].tv = 0.f;
			pv[2].tu = 1.f;		pv[2].tv = 0.f;
			pv[4].tu = 1.f;		pv[4].tv = 1.f;
			pv[6].tu = 0.f;		pv[6].tv = 1.f;
			pv[8].tu = 0.f;		pv[8].tv = 0.f;

			pv[14].tu = 0.f;	pv[14].tv = 0.f;
			pv[15].tu = 0.f;	pv[15].tv = 1.f;
			pv[16].tu = 1.f;	pv[16].tv = 0.f;
			pv[17].tu = 1.f;	pv[17].tv = 1.f;

			rs->UnLockVertexBuffer(m_idVBuf);
		}
	}

	m_nCurRect = -1;
	SetRectangle(0);

	delete ini;
	return true;
}

void HELPCHOOSER::SetRectangle(long newRectNum)
{
	if(newRectNum==m_nCurRect) return;
	if(newRectNum<0 || newRectNum>=m_nRectQ)
	{
		api->Trace("WARNING! Wrong rectangle number into HELPCHOOSER");
		return;
	}
	if(m_idVBuf==-1) return;
	HCHOOSER_VERTEX * pv = (HCHOOSER_VERTEX *)rs->LockVertexBuffer(m_idVBuf);
	if(pv==null) return;
	m_nCurRect = newRectNum;

	pv[1].tu = pv[7].tu = pv[9].tu = pv[10].tu = pv[11].tu = m_pRectList[newRectNum].left;
	pv[3].tu = pv[5].tu = pv[12].tu = pv[13].tu = m_pRectList[newRectNum].right;
	pv[1].tv = pv[3].tv = pv[9].tv = pv[10].tv = pv[12].tv = m_pRectList[newRectNum].top;
	pv[5].tv = pv[7].tv = pv[11].tv = pv[13].tv = m_pRectList[newRectNum].bottom;

	pv[1].pos.x = pv[7].pos.x = pv[9].pos.x = pv[10].pos.x = pv[11].pos.x = m_pRectList[newRectNum].left*m_fScreenWidth;
	pv[3].pos.x = pv[5].pos.x = pv[12].pos.x = pv[13].pos.x = m_pRectList[newRectNum].right*m_fScreenWidth;
	pv[1].pos.y = pv[3].pos.y = pv[9].pos.y = pv[10].pos.y = pv[12].pos.y = m_pRectList[newRectNum].top*m_fScreenHeight;
	pv[5].pos.y = pv[7].pos.y = pv[11].pos.y = pv[13].pos.y = m_pRectList[newRectNum].bottom*m_fScreenHeight;

	pv[14].pos.x = pv[15].pos.x = m_fCurMouseX - m_nMouseCornerX;
	pv[16].pos.x = pv[17].pos.x = m_fCurMouseX - m_nMouseCornerX + m_nMouseWidth;
	pv[14].pos.y = pv[16].pos.y = m_fCurMouseY - m_nMouseCornerY;
	pv[15].pos.y = pv[17].pos.y = m_fCurMouseY - m_nMouseCornerY + m_nMouseHeight;

	rs->UnLockVertexBuffer(m_idVBuf);
}

long HELPCHOOSER::GetRectangleLeft()
{
	if(m_nCurRect<0 || m_nCurRect>=m_nRectQ || m_pRectList==null) return 0;
	float left = m_pRectList[m_nCurRect].left;
	float top = m_pRectList[m_nCurRect].top;
	float right = m_pRectList[m_nCurRect].right;
	float bottom = m_pRectList[m_nCurRect].bottom;

	float	fdist = 1.f;
	float	ftmp;
	long	nRectNum = m_nCurRect;
	while(true)
	{
		for(int i=0; i<m_nRectQ; i++)
		{
			if(i==m_nCurRect || m_pRectList[i].top>bottom || m_pRectList[i].bottom<top) continue;

			if(m_pRectList[i].left<left)	ftmp = left - m_pRectList[i].left;
			else	ftmp = 1.f - m_pRectList[i].left + left;

			if(ftmp<fdist)
			{
				fdist = ftmp;
				nRectNum = i;
			}
		}
		if(nRectNum==i && (top>0.f || bottom<1.f) )
		{
			top=0.f;
			bottom=1.f;
		}
		else	break;
	}
	return nRectNum;
}

long HELPCHOOSER::GetRectangleRight()
{
	if(m_nCurRect<0 || m_nCurRect>=m_nRectQ || m_pRectList==null) return 0;
	float left = m_pRectList[m_nCurRect].left;
	float top = m_pRectList[m_nCurRect].top;
	float right = m_pRectList[m_nCurRect].right;
	float bottom = m_pRectList[m_nCurRect].bottom;

	float	fdist = 1.f;
	float	ftmp;
	long	nRectNum = m_nCurRect;
	while(true)
	{
		for(int i=0; i<m_nRectQ; i++)
		{
			if(i==m_nCurRect || m_pRectList[i].top>bottom || m_pRectList[i].bottom<top) continue;

			if(m_pRectList[i].right>right)	ftmp = m_pRectList[i].right - right;
			else	ftmp = 1.f - right + m_pRectList[i].right;

			if(ftmp<fdist)
			{
				fdist = ftmp;
				nRectNum = i;
			}
		}
		if(nRectNum==i && (top>0.f || bottom<1.f) )
		{
			top=0.f;
			bottom=1.f;
		}
		else	break;
	}
	return nRectNum;
}

long HELPCHOOSER::GetRectangleUp()
{
	if(m_nCurRect<0 || m_nCurRect>=m_nRectQ || m_pRectList==null) return 0;
	float left = m_pRectList[m_nCurRect].left;
	float top = m_pRectList[m_nCurRect].top;
	float right = m_pRectList[m_nCurRect].right;
	float bottom = m_pRectList[m_nCurRect].bottom;

	float	fdist = 1.f;
	float	ftmp;
	long	nRectNum = m_nCurRect;
	while(true)
	{
		for(int i=0; i<m_nRectQ; i++)
		{
			if(i==m_nCurRect || m_pRectList[i].left>right || m_pRectList[i].right<left) continue;

			if(m_pRectList[i].top<top)	ftmp = top - m_pRectList[i].top;
			else	ftmp = top + 1.f - m_pRectList[i].top;

			if(ftmp<fdist)
			{
				fdist = ftmp;
				nRectNum = i;
			}
		}
		if(nRectNum==i && (left>0.f || left<1.f) )
		{
			left=0.f;
			right=1.f;
		}
		else	break;
	}
	return nRectNum;
}

long HELPCHOOSER::GetRectangleDown()
{
	if(m_nCurRect<0 || m_nCurRect>=m_nRectQ || m_pRectList==null) return 0;
	float left = m_pRectList[m_nCurRect].left;
	float top = m_pRectList[m_nCurRect].top;
	float right = m_pRectList[m_nCurRect].right;
	float bottom = m_pRectList[m_nCurRect].bottom;

	float	fdist = 1.f;
	float	ftmp;
	long	nRectNum = m_nCurRect;
	while(true)
	{
		for(int i=0; i<m_nRectQ; i++)
		{
			if(i==m_nCurRect || m_pRectList[i].left>right || m_pRectList[i].right<left) continue;

			if(m_pRectList[i].bottom>bottom)	ftmp = m_pRectList[i].bottom - bottom;
			else	ftmp = m_pRectList[i].bottom + 1.f - bottom;

			if(ftmp<fdist)
			{
				fdist = ftmp;
				nRectNum = i;
			}
		}
		if(nRectNum==i && (left>0.f || left<1.f) )
		{
			left=0.f;
			right=1.f;
		}
		else	break;
	}
	return nRectNum;
}

bool HELPCHOOSER::MouseMove()
{
	float oldX = m_fCurMouseX;
	float oldY = m_fCurMouseY;

	CONTROL_STATE csv,csh;
	_CORE_API->Controls->GetControlState("ITurnV",csv);
	_CORE_API->Controls->GetControlState("ITurnH",csh);
	if(csv.lValue==0 && csh.lValue==0) return false;
	m_fCurMouseX += csh.fValue;
	m_fCurMouseY -= csv.fValue;

	if(m_fCurMouseX<0.f) m_fCurMouseX = 0.f;
	if(m_fCurMouseX>m_fScreenWidth) m_fCurMouseX = m_fScreenWidth;
	if(m_fCurMouseY<0.f) m_fCurMouseY = 0.f;
	if(m_fCurMouseY>m_fScreenHeight) m_fCurMouseY = m_fScreenHeight;

	oldX-=m_fCurMouseX;
	oldY-=m_fCurMouseY;
	if(oldX>.1f || oldX<-.1f || oldY>.1f || oldY<-.1f) return true;
	else	return false;
}

long HELPCHOOSER::GetRectangleFromPos(float x, float y)
{
	if(m_pRectList==null) return m_nCurRect;
	x /= m_fScreenWidth;
	y /= m_fScreenHeight;
	for(long i=0; i<m_nRectQ; i++)
	{
		if( m_pRectList[i].left<=x && m_pRectList[i].right>=x &&
			m_pRectList[i].top<=y && m_pRectList[i].bottom>=y )
			return i;
	}
	return -1;
}