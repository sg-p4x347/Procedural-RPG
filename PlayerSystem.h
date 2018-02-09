#pragma once
#include "WorldSystem.h"
#include "ActionSystem.h"
class SystemManager;

class PlayerSystem :
	public WorldSystem
{
public:
	PlayerSystem(shared_ptr<SystemManager> systemManager, unique_ptr<WorldEntityManager> &  entityManager,
		vector<string> & components, 
		unsigned short updatePeriod
	);

	// Inherited via System
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
	//----------------------------------------------------------------
	// Generation
	void CreatePlayer();

	//----------------------------------------------------------------
	// Utility
	Quaternion GetPlayerQuaternion();


private:
	shared_ptr<SystemManager> SM;
	shared_ptr<DirectX::Mouse> m_mouse;
	shared_ptr<DirectX::Keyboard> m_keyboard;
};

