#pragma once

#ifndef LIGHTENUMS_HPP_
#define LIGHTENUMS_HPP_

namespace graphics
{
	enum class DiffuseModel
	{
		LAMBERT,
		TOON,
		OREN_NAYAR
	};

	enum class SpecularModel
	{
		PHONG,
		TOON,
		BLINN
	};
}

#endif
