#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <real_core/Singleton.h>
#include <real_core/Observer.h>
#include <real_core/SceneManager.h>

namespace real
{
	class Camera;

	class CameraManager final
		: public real::Singleton<CameraManager>
		, public real::Observer<real::SceneEvents, real::Scene*>
	{
	public:
		virtual ~CameraManager() override = default;

		CameraManager(const CameraManager&) = delete;
		CameraManager& operator=(const CameraManager&) = delete;
		CameraManager(CameraManager&&) = delete;
		CameraManager& operator=(CameraManager&&) = delete;

		virtual void HandleEvent(real::SceneEvents, real::Scene*) override;
		void OnSubjectDestroy() override {}

		uint32_t AddCamera(Camera* camera, bool setActive = false);
		void SetCameraActive(Camera* camera);
		void SetCameraActive(uint32_t camera);
		Camera* GetActiveCamera();

	private:
		friend class Singleton<CameraManager>;
		CameraManager() = default;

		//		  go id	   camera
		std::map<uint32_t, Camera*> m_pCameras{};
		Camera* m_pActiveCamera{ nullptr };
	};
}

#endif // CAMERAMANAGER_H