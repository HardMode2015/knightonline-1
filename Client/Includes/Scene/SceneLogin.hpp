#ifndef _SCENE_LOGIN_H_
#define _SCENE_LOGIN_H_

#include "../Main.hpp"
#include "SceneBase.hpp"

enum eUI_TYPE {
	UI_TYPE_BASE = 0,		// none
	UI_TYPE_BUTTON,			// button
	UI_TYPE_STATIC,			// static (배경그림과 글자가 나오는 클래스)
	UI_TYPE_PROGRESS,		// progress
	UI_TYPE_IMAGE,			// image
	UI_TYPE_SCROLLBAR,		// scroll bar
	UI_TYPE_STRING,			// string
	UI_TYPE_TRACKBAR,		// track bar
	UI_TYPE_EDIT,			// edit

							//cerberus 01,12,29
							UI_TYPE_AREA,			// area
							UI_TYPE_TOOLTIP,		// tooltip

													// ecli666
													UI_TYPE_ICON,			// icon
													UI_TYPE_ICON_MANAGER,	// icon manager.. 

																			// repent 전용
																			UI_TYPE_ICONSLOT,		// icon slot
																			UI_TYPE_LIST,			// Text List...
};

class SceneLogin : private SceneBase
{
public:
	SceneLogin();
	void Init();
	void Render();
private:
	class CTexture * m_tMoon;
	class CTexture * m_ptLogin;

	int m_iOgrePartCount;
	class CCharPart * m_pOgreParts = nullptr;
	__Matrix44 m_mOgreWorld;
};

#endif