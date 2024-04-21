#ifndef OBSERVER_H
#define OBSERVER_H

namespace real
{
	template<typename... Args>
	class Observer
	{
	public:
		explicit Observer() = default;
		virtual ~Observer() = default;

		Observer(const Observer& other) = delete;
		Observer& operator=(const Observer& rhs) = delete;
		Observer(Observer&& other) = delete;
		Observer& operator=(Observer&& rhs) = delete;

		virtual void HandleEvent(Args... args) = 0;
		virtual void OnSubjectDestroy() = 0;
	};
}

#endif // OBSERVER_H