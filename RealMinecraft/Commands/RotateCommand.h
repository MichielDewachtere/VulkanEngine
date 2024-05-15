#ifndef ROTATECOMMAND_H
#define ROTATECOMMAND_H

#include <real_core/Command.h>

class RotateCommand final : public real::GameObjectCommand
{
public:
	explicit RotateCommand(int id, int controllerId, real::GameObject* pOwner);
	virtual ~RotateCommand() override = default;

	RotateCommand(const RotateCommand&) = delete;
	RotateCommand operator=(const RotateCommand&) = delete;
	RotateCommand(RotateCommand&&) = delete;
	RotateCommand operator=(RotateCommand&&) = delete;

	virtual void Execute() override;

private:
	float m_RotationSpeed{ 5.f }, m_Yaw{ 0.f }, m_Pitch{ 0.f };
};

#endif // ROTATECOMMAND_H