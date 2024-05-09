#ifndef TESTSCENE_H
#define TESTSCENE_H

#include <real_core/Scene.h>

class TestScene final : public real::Scene
{
public:
	explicit TestScene(std::string name, std::string inputMap);
	~TestScene() override = default;

	TestScene(const TestScene& other) = delete;
	TestScene& operator=(const TestScene& rhs) = delete;
	TestScene(TestScene&& other) = delete;
	TestScene& operator=(TestScene&& rhs) = delete;

	virtual void Load() override;
};

#endif // TESTSCENE_H