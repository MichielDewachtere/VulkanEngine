#ifndef GUI_H
#define GUI_H

#include <real_core/Component.h>

#include "Mesh/MeshIndexed.h"
#include "Util/GameStructs.h"

class World;

class Gui final : public real::Component
{
public:
	explicit Gui(real::GameObject* pOwner);
	~Gui() override = default;

	Gui(const Gui& other) = delete;
	Gui& operator=(const Gui& rhs) = delete;
	Gui(Gui&& other) = delete;
	Gui& operator=(Gui&& rhs) = delete;

	virtual void Start() override;
	//virtual void Update() override;

private:
	real::MeshIndexed<PosTex, WorldMatrix>* m_pMeshComponent{ nullptr };
};


#endif // GUI_H