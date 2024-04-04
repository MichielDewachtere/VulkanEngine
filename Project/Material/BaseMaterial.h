#ifndef BASEMATERIAL_H
#define BASEMATERIAL_H

class BaseMaterial
{
public:
	virtual ~BaseMaterial() = default;

	virtual void CleanUp(const GameContext& context) = 0;

	virtual void DrawFrame(uint32_t currentFrame, VkExtent2D extent) const = 0;

	bool IsActive() const { return m_IsActive; }
	void SetIsActive(const bool isActive) { m_IsActive = isActive; }

private:
	bool m_IsActive{ true };
};

#endif // BASEMATERIAL_H