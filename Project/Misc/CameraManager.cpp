#include "CameraManager.h"

#include "Camera.h"

void CameraManager::HandleEvent(real::SceneEvents events, real::Scene*)
{
	if (events == real::SceneEvents::exit)
		m_pCameras.clear();
}

uint32_t CameraManager::AddCamera(Camera* camera, bool setActive)
{
	if (camera == nullptr)
	{
		real::Logger::LogWarning({ "Could not add camera because camera was NULL" });
		return 0;
	}

	if (m_pCameras.contains(camera->GetOwner()->GetId()) == false)
		m_pCameras.insert({ camera->GetOwner()->GetId(), camera });

	if (setActive)
		m_pActiveCamera = camera;

	return camera->GetOwner()->GetId();
}

void CameraManager::SetCameraActive(Camera* camera)
{
	AddCamera(camera);

	m_pActiveCamera = camera;
}

void CameraManager::SetCameraActive(uint32_t camera)
{
	if (m_pCameras.contains(camera))
		m_pActiveCamera = m_pCameras.at(camera);

	real::Logger::LogWarning({ "No camera found with id {}" }, std::to_string(camera));
}

Camera* CameraManager::GetActiveCamera()
{
	if (m_pActiveCamera == nullptr)
	{
		if (m_pCameras.empty())
		{
			real::Logger::LogError({ "No cameras registered in camera manager" });
			throw std::runtime_error("No cameras registered in camera manager");
		}

		m_pActiveCamera = m_pCameras.begin()->second;
	}

	return m_pActiveCamera;
}