#pragma once
#include "System.h"
class PlayerSystem :
	public System
{
public:
	PlayerSystem(shared_ptr<EntityManager> & entityManager, 
		vector<string> & components, 
		unsigned short updatePeriod,
		shared_ptr<DirectX::Mouse> mouse,
		shared_ptr<DirectX::Keyboard> keyboard
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

