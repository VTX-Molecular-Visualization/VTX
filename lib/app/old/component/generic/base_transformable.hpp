#ifndef __VTX_APP_OLD_COMPONENT_GENERIC_BASE_TRANSFORMABLE__
#define __VTX_APP_OLD_COMPONENT_GENERIC_BASE_TRANSFORMABLE__

#include "app/old/internal/math/enum_transform.hpp"
#include "app/old/internal/math/transform.hpp"
#include <util/generic/enum_flag.hpp>

namespace VTX::App::Old::Component::Generic
{
	class BaseTransformable
	{
	  public:
		inline virtual const App::Old::Internal::Math::Transform & getTransform() const { return _transform; };

		void rotate( const float, const Vec3f & );
		void setRotation( const Vec3f & );
		void setScale( const float & );
		void setScale( const Vec3f & );
		void setScale( const Mat4f & );
		void setTranslation( const Vec3f & );
		void setTranslation( const Mat4f & );
		void applyTransform( const App::Old::Internal::Math::Transform & );
		void applyTransform( const App::Old::Internal::Math::Transform &, const Internal::Math::TRANSFORM_COMPOSANT_MASK );

	  protected:
		App::Old::Internal::Math::Transform _transform;
		virtual void				   _transformModifiedEvent() {};
	};
} // namespace VTX::App::Old::Component::Generic
#endif
