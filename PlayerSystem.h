#pragma once
#include "WorldSystem.h"
class PlayerSystem :
	public WorldSystem
{
public:
	PlayerSystem(unique_ptr<WorldEntityManager> &  entityManager,
		vector<string> & components, 
		unsigned short updatePeriod
	);

	// Inherited via System
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	//----------------------------------------------------------------
	// Generation
	void CreatePlayer();
private:
	shared_ptr<DirectX::Mouse> m_mouse;
	shared_ptr<DirectX::Keyboard> m_keyboard;
};

