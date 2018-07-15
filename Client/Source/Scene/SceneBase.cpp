#include "../../Includes/Scene/SceneBase.hpp"
#include "../../Includes/Scene/SceneLogin.hpp"

SceneLogin* SceneBase::s_pSceneLogin;

SceneBase* SceneBase::s_pScenePrev;
SceneBase* SceneBase::s_pSceneActive;

void SceneBase::InitStatic()
{
	s_pSceneLogin = new SceneLogin();
}

void SceneBase::SetActiveScene(SceneBase * pActiveScene)
{
	if (NULL == pActiveScene || s_pSceneActive == pActiveScene) return;

	s_pScenePrev = s_pSceneActive; // 전의 것 포인터 기억..
	s_pSceneActive = pActiveScene;
}