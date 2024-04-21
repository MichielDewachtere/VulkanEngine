#ifndef COMMAND_H
#define COMMAND_H

#include "Observer.h"

namespace real
{
	enum class GameObjectEvent : char;
	class GameObject;

	class Command
	{
	public:
		Command(int id, int controllerId)
		// TODO: maybe find different solution for command deletion
			: m_Id(id) , m_ControllerId(controllerId) {}
		virtual ~Command() = default;

		Command(const Command& other) = delete;
		Command& operator=(const Command& rhs) = delete;
		Command(Command&& other) = delete;
		Command& operator=(Command&& rhs) = delete;

		virtual void Start() {}
		virtual void Execute() = 0;

	protected:
		int GetId() const { return m_Id; }
		int GetControllerId() const { return m_ControllerId; }

	private:
		int m_Id, m_ControllerId;
	};

	class GameObjectCommand
		: public Command
		, public Observer<GameObjectEvent>
	{
	public:
		explicit GameObjectCommand(int id, int controllerId, GameObject* pGameObject);
		virtual ~GameObjectCommand() override;

		GameObjectCommand(const GameObjectCommand& other) = delete;
		GameObjectCommand& operator=(const GameObjectCommand& rhs) = delete;
		GameObjectCommand(GameObjectCommand&& other) = delete;
		GameObjectCommand& operator=(GameObjectCommand&& rhs) = delete;

		void HandleEvent(GameObjectEvent) override;
		void OnSubjectDestroy() override;

	protected:
		GameObject* GetGameObject() const { return m_pGameObject; }
	private:
		GameObject* m_pGameObject;
		bool m_ObserverRemoved{ false };
	};

	class TestCommand final : public Command
	{
	public:
		explicit TestCommand(int id, int controllerId)
			: Command(id, controllerId) {}
		virtual ~TestCommand() override = default;

		virtual void Execute() override;
	};
}

#endif // COMMAND_H