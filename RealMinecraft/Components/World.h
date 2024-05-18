#ifndef	WORLD_H
#define WORLD_H

#include <map>
#include <vector>

#include <glm/vec2.hpp>

#include <real_core/Component.h>
#include <real_core/Observer.h>

class Chunk;

// Custom comparator for glm::vec3
struct Vec2Comparator {
	bool operator()(const glm::vec2& lhs, const glm::vec2& rhs) const {
		if (lhs.x < rhs.x) return true;
		if (lhs.x > rhs.x) return false;
		return (lhs.y < rhs.y);
	}
};

class World final
	: public real::Component
	, public real::Observer<const glm::ivec2&>
{
public:
	explicit World(real::GameObject* pOwner);
	virtual ~World() override = default;

	World(const World& other) = delete;
	World& operator=(const World& rhs) = delete;
	World(World&& other) = delete;
	World& operator=(World&& rhs) = delete;

	virtual void Start() override;
	virtual void Update() override;
	virtual void Kill() override;

	void HandleEvent(const glm::ivec2&) override;
	void OnSubjectDestroy() override {}

	Chunk* GetChunkAt(const glm::ivec2& chunkPos) const;

private:
	static constexpr inline int render_distance{ 4 };

	std::map<glm::ivec2, Chunk*, Vec2Comparator> m_pChunks{};
};

#endif // WORLD_H