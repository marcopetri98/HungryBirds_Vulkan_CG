#pragma once

#ifndef COLLECTIONUTILS_HPP
#define COLLECTIONUTILS_HPP

#include <vector>

using std::vector;

namespace collectionutils
{
	// TODO: implementatino should be on a different file with extension .cpp
	template<class valueT>
	bool vectorContains(vector<valueT> vectorClass, valueT value)
	{
		if (std::count(vectorClass.begin(), vectorClass.end(), value))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

#endif