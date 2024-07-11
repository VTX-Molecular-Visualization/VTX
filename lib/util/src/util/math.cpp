#include "util/math.hpp"

namespace VTX::Util::Math
{

	Vec3f orthogonalVector( const Vec3f & normal )
	{
		// Solve a*x + b*y + c*z = 0
		const float a = normal.x;
		const float b = normal.y;
		const float c = normal.z;
		float		x = 0.f, y = 0.f, z = 0.f;
		if ( std::abs( a ) > VTX::EPSILON && std::abs( b ) > VTX::EPSILON )
		{
			x = -glm::sign( a ) * b;
			y = glm::sign( a ) * a;
			z = 0.f;
		}
		else if ( std::abs( b ) > VTX::EPSILON && std::abs( c ) > VTX::EPSILON )
		{
			x = 0.f;
			y = -glm::sign( b ) * c;
			z = glm::sign( b ) * b;
		}
		else if ( std::abs( c ) > VTX::EPSILON && std::abs( a ) > VTX::EPSILON )
		{
			x = glm::sign( c ) * c;
			y = 0.f;
			z = -glm::sign( c ) * a;
		}
		else if ( std::abs( a ) > VTX::EPSILON )
		{
			x = 0.f;
			y = 1.f;
			z = 0.f;
		}
		else if ( std::abs( b ) > VTX::EPSILON )
		{
			x = 0.f;
			y = 0.f;
			z = 1.f;
		}
		else if ( std::abs( c ) > VTX::EPSILON )
		{
			x = 1.f;
			y = 0.f;
			z = 0.f;
		}

		// Vec3f v1 = glm::normalize( Vec3f( x, y, z ) );
		// Vec3f v2 = glm::normalize( Vec3f( b * z - c * y, c * x - a * z, a * y - b * x ) );
		return glm::normalize( Vec3f( x, y, z ) );
	}

}