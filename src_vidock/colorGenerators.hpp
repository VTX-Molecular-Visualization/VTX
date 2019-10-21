/*
 * File: colorGenerators.hpp
 * Author: Maxime MARIA
 */

#include "math/mathUtils.hpp"

namespace vidocklab
{
	static Vec3f randomPastelColor()
	{
		return rndVec3f() * 0.5f + 0.5f;
	}

} // namespace vidocklab
