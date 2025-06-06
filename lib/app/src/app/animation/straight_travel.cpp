#include "app/animation/straight_travel.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/vtx_app.hpp"
#include <util/math.hpp>

namespace VTX::App::Animation
{
	StraightTravel::StraightTravel(
		const Vec3f &		p_finalPostion,
		const Quatf &		p_finalRotation,
		const float &		p_duration,
		const TravelRythm & p_rythm
	) :
		_animation(
			SCENE().getCamera().getTransform().getTransform(),
			p_finalPostion,
			p_finalRotation,
			p_finalPostion,
			p_duration
		)
	{
		VTX::App::Core::Animation::TravelManager::InterpolationPositionFunc posFunc
			= &Util::Math::linearInterpolation<Vec3f, float>;
		VTX::App::Core::Animation::TravelManager::InterpolationRotationFunc rotFunc
			= &Util::Math::linearInterpolation<Quatf, float>;
		switch ( p_rythm )
		{
		case TravelRythm::easeInOut:
			posFunc = &Util ::Math ::easeInOutInterpolation<Vec3f, float>;
			rotFunc = &Util ::Math ::easeInOutInterpolation<Quatf, float>;
		default: break;
		}

		_animation.setPositionFunc( [ posFunc ]( const Vec3f & lhs, const Vec3f & rhs, float value )
									{ return posFunc( lhs, rhs, value ); } );
		_animation.setRotationFunc( [ rotFunc ]( const Quatf & lhs, const Quatf & rhs, float value )
									{ return rotFunc( lhs, rhs, value ); } );
	}
} // namespace VTX::App::Animation
