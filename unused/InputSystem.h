#pragma once
#include "System.h"
class InputSystem :
	public System
{
public:
	InputSystem(
		shared_ptr<EntityManager> & entityManager,
		vector<string> & components,
		unsigned short updatePeriod,
		shared_ptr<DirectX::Mouse> mouse,
		shared_ptr<DirectX::Keyboard> keyboard
	);
	~InputSystem();
private:
	shared_ptr<DirectX::Mouse> m_mouse;
	shared_ptr<DirectX::Keyboard> m_keyboard;
	float movementSpeed = 50.0;
	void Move(Movement & movement, Position & position);
	void Stop(Movement & movement);
	// Inherited via System
	virtual void Update() override;

	// Inherited via System
	virtual void SyncEntities() override;
	virtual string Name() override;
};

