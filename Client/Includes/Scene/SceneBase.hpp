#ifndef _SCENE_BASE_H_
#define _SCENE_BASE_H_

#include "../CBase.hpp"
#include "../../Includes/Elements/CTexture.hpp"

class SceneBase : public CBase
{
public:
	SceneBase() {
		
	}

	static class SceneLogin*			s_pSceneLogin;

	static class SceneBase*			s_pScenePrev;
	static class SceneBase*			s_pSceneActive;

	static void InitStatic();

	virtual void Init() {

	};

	virtual void Tick() {

	}

	virtual void Render() {

	}

	static void TickActive() {
		if (s_pScenePrev != s_pSceneActive) {
			//s_pScenePrev->Release();
			s_pScenePrev = s_pSceneActive;
			s_pSceneActive->Init();
		}

		s_pSceneActive->Tick();
	};

	static void RenderActive() {
		if (s_pSceneActive == s_pScenePrev) s_pSceneActive->Render();
	};

	static void SetActiveScene(SceneBase * pActiveScene);
private:
	CTexture m_tMoon;
};

#endif