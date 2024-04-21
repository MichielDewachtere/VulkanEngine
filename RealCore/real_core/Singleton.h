#ifndef SINGLETON_H
#define SINGLETON_H

namespace real
{
	template <typename T>
	class Singleton
	{
	public:
		static T& GetInstance()
		{
			static T instance{};
			return instance;
		}

		virtual ~Singleton() = default;
		Singleton(const Singleton& other) = delete;
		Singleton(Singleton&& other) = delete;
		Singleton& operator=(const Singleton& other) = delete;
		Singleton& operator=(Singleton&& other) = delete;

	protected:
		virtual void Init() {}
		virtual void Update() {}

		Singleton() = default;
	};
}

#endif // SINGLETON_H