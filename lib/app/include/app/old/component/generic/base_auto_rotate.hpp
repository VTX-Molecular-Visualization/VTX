#ifndef __VTX_APP_OLD_COMPONENT_GENERIC_BASE_AUTO_ROTATE__
#define __VTX_APP_OLD_COMPONENT_GENERIC_BASE_AUTO_ROTATE__

#include <util/math.hpp>
#include <util/types.hpp>

namespace VTX::App::Old::Component::Generic
{
	class BaseAutoRotate
	{
	  public:
		BaseAutoRotate();

		inline const Vec3f & getAutoRotationVector() const { return _autoRotationVector; }
		inline Vec3f &		 getAutoRotationVector() { return _autoRotationVector; }
		inline virtual void	 setAutoRotationVector( const Vec3f p_value ) { _autoRotationVector = p_value; }

		inline Vec3f getNormalizedAutoRotationVector() const { return Util::Math::normalize( _autoRotationVector ); }
		inline virtual void setAutoRotationNormalizedVector( const Vec3f p_value )
		{
			_autoRotationVector = p_value * getAutoRotationMagnitude();
		}
		inline float		getAutoRotationMagnitude() const { return Util::Math::length( _autoRotationVector ); }
		inline virtual void setAutoRotationMagnitude( const float p_speed )
		{
			_autoRotationVector = getNormalizedAutoRotationVector() * p_speed;
		}

		inline bool			isAutoRotationPlaying() const { return _isPlaying; }
		inline virtual void setAutoRotationPlaying( const bool p_play ) { _isPlaying = p_play; }

	  protected:
		Vec3f _autoRotationVector;
		bool  _isPlaying = false;
	};
} // namespace VTX::App::Old::Component::Generic
#endif
