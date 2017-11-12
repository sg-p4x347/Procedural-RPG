#pragma once
class BaseEvent
{
public:
	BaseEvent(string name);
	virtual ~BaseEvent();
	string Name();
private:
	string m_name;
};

