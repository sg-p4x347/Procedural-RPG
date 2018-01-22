#include "pch.h"
#include "Delegate.h"

namespace Components {


	Delegate::Delegate(string delegateName) : DelegateName(delegateName)
	{
	}
	string Delegate::GetName()
	{
		return GetDiscreteName() + '_' + DelegateName;
	}
}
