#include "pch.h"
#include "Input.h"


Input::Input()
{
}


Input::~Input()
{
}

vector<Input>& Input::GetComponents()
{
	static vector<Input> components;
	return components;
}

Component * Input::GetComponent(const unsigned int & id)
{
	for (Input & input : GetComponents()) {
		if (input.ID == id) {
			return &input;
		}
		else if (input.ID > id) {
			return nullptr;
		}
	}
}

void Input::AddComponent(const unsigned int & id)
{
	GetComponents().push_back(Input());
}

void Input::Save(string directory)
{
}

void Input::Load(string directory)
{
}
