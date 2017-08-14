#include "pch.h"
#include "RenderSystem.h"

RenderSystem::RenderSystem(
	shared_ptr<EntityManager>& entityManager, 
	vector<string>& components, 
	unsigned short updatePeriod, 
	Microsoft::WRL::ComPtr<ID3D11Device> device, 
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	std::shared_ptr<DirectX::CommonStates> states
) : System(entityManager,components, updatePeriod)
{

}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Update()
{
	
}
