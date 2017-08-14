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
		DirectX::Mouse::State mouse,
		DirectX::Keyboard::State keyboard
	);
	~InputSystem();
private:
	DirectX::Mouse::State m_mouse;
	DirectX::Keyboard::State m_keyboard;
	float movementSpeed = 50.0;
	void Move(Movement & movement, Position & position);
	void Stop(Movement & movement);
	// Inherited via System
	virtual void Update() override;
};

