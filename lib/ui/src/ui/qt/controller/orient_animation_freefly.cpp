#include "ui/qt/controller/orient_animation_freefly.hpp"
#include <app/vtx_app.hpp>

namespace VTX::UI::QT::Controller
{
	OrientAnimationFreefly::OrientAnimationFreefly(
		VTX::App::Component::Render::Camera & p_camera,
		const Util::Math::AABB &			  p_aabb
	) :
		_camera( p_camera )
	{
		_orientStartingPosition = _camera.getPosition();

		const float targetDistance
			= p_aabb.radius() / (float)( tan( Util::Math::radians( _camera.getFov() ) * ORIENT_ZOOM_FACTOR ) );
		_orientTargetPosition = p_aabb.centroid() - _camera.getFront() * targetDistance;

		_orientStartingRotation = _camera.getRotation();
		_orientTargetRotation	= _orientStartingRotation;

		App::VTXApp::get().onUpdate().addCallback(
			this, [ this ]( const float p_deltaTime ) { update( p_deltaTime ); }
		);

		_orientTime = 0.f;

		const bool needAnimation
			= Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD;

		if ( !needAnimation )
		{
			_finish();
		}
	}

	OrientAnimationFreefly::OrientAnimationFreefly(
		VTX::App::Component::Render::Camera & p_camera,
		const Vec3f &						  p_position,
		const Quatf &						  p_orientation
	) :
		_camera( p_camera )
	{
		_orientStartingPosition = p_camera.getPosition();
		_orientTargetPosition	= p_position;

		_orientStartingRotation = p_camera.getRotation();
		_orientTargetRotation	= p_orientation;

		App::VTXApp::get().onUpdate().addCallback(
			this, [ this ]( const float p_deltaTime ) { update( p_deltaTime ); }
		);
		_orientTime = 0.f;

		const bool needAnimation
			= Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD;

		if ( !needAnimation )
		{
			_finish();
		}
	}

	void OrientAnimationFreefly::update( const float & p_deltaTime )
	{
		_camera.setPosition(
			Util::Math::easeInOutInterpolation( _orientStartingPosition, _orientTargetPosition, p_deltaTime )
		);

		_camera.setRotation(
			Util::Math::easeInOutInterpolation( _orientStartingRotation, _orientTargetRotation, p_deltaTime )
		);
	}

	void OrientAnimationFreefly::_finish() { App::VTXApp::get().onUpdate().removeCallback( this ); }

} // namespace VTX::UI::QT::Controller
