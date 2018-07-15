#ifndef _SCENE_LOGIN_H_
#define _SCENE_LOGIN_H_

#include "../Main.hpp"
#include "SceneBase.hpp"

class SceneLogin : private SceneBase
{
public:
	SceneLogin();
	void Init();
	void Render();
private:
	class CTexture * m_tMoon;
};

#endif