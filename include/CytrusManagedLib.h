// CytrusManagedLib.h

#pragma once
#include "CytrusAlgLib.h"

using namespace System;

namespace CytrusManagedLib {

	public ref class Class1
	{
	public:
		int u;
		Class1(){
			cytrus::alg::AddClass c;
			u=c.Add(2,7);
		}

		int getU(){
			return u;
		}
	};
}
