#ifndef MODEL_H
#define MODEL_H

#include "Util/Structs.h"

// TODO: Finish tutorial
namespace real
{
	class Model final
	{
	public:
		explicit Model(const std::string& path, const glm::vec3& pos);
		~Model() = default;

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;
		Model(Model&&) = delete;
		Model& operator=(Model&&) = delete;

		//void CleanUp(const GameContext& context) const;

		std::vector<PosTexNorm> GetVertices() const { return m_Vertices; }
		std::vector<uint32_t> GetIndices() const { return m_Indices; }

	private:
		std::vector<PosTexNorm> m_Vertices;
		std::vector<uint32_t> m_Indices;

		void Load(const std::string& path, glm::vec3);
	};
}

#endif // MODEL_H