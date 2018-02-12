#pragma once
#include "WorldSystem.h"
class SystemManager;

class PlayerSystem :
	public WorldSystem
{
public:
	PlayerSystem(SystemManager * systemManager, unique_ptr<WorldEntityManager> &  entityManager,
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
	Quaternion GetPlayerQuaternion(bool ignorePitch = false);


private:
	SystemManager * SM;
	shared_ptr<DirectX::Mouse> m_mouse;
	shared_ptr<DirectX::Keyboard> m_keyboard;

	void SetMovementToNormal();
	void SetMovementToSpectator();
};

