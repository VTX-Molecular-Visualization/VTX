// #include "ui/internal/animation/orient_animation_trackball.hpp"
// #include <app/component/scene/transform_component.hpp>
// #include <app/vtx_app.hpp>
// #include <util/math.hpp>
//
// namespace VTX::UI::QT::Controller
//{
//	OrientAnimationTrackball::OrientAnimationTrackball(
//		VTX::App::Component::Render::Camera & p_camera,
//		const Vec3f &						  p_currentTarget,
//		const Util::Math::AABB &			  p_aabb
//	) :
//		_camera( p_camera )
//	{
//		_orientStartingPosition = p_currentTarget;
//		_orientTargetPosition	= p_aabb.centroid();
//
//		const App::Component::Scene::Transform & cameraTransform
//			= App::MAIN_REGISTRY().getComponent<App::Component::Scene::Transform>( p_camera );
//
//		_orientStartingRotation = cameraTransform.getRotation();
//		_orientTargetRotation	= cameraTransform.getRotation();
//
//		_orientStartingDistance = Util::Math::distance( cameraTransform.getPosition(), p_currentTarget );
//		_orientTargetDistance
//			= p_aabb.radius() / (float)( tan( Util::Math::radians( _camera.getFov() ) * ORIENT_ZOOM_FACTOR ) );
//
//		_orientTime = 0.f;
//
//		App::VTXApp::get().onUpdate().addCallback(
//			this, [ this ]( const float p_deltaTime ) { update( p_deltaTime ); }
//		);
//
//		const bool needAnimation
//			= Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD
//			  || abs( _orientTargetDistance - _orientStartingDistance ) > ORIENT_THRESHOLD;
//
//		if ( !needAnimation )
//		{
//			_finish();
//		}
//	}
//
//	OrientAnimationTrackball::OrientAnimationTrackball(
//		VTX::App::Component::Render::Camera & p_camera,
//		const Vec3f &						  p_target,
//		const Vec3f &						  p_position,
//		const Quatf &						  p_orientation
//	) :
//		_camera( p_camera )
//	{
//		_orientStartingDistance = Util::Math::distance( _camera.getPosition(), p_target );
//		_orientTargetDistance	= Util::Math::distance( p_position, p_target );
//
//		_orientStartingPosition = _camera.getPosition() + _camera.getFront() * _orientStartingDistance;
//		_orientTargetPosition	= p_target;
//
//		_orientStartingRotation = _camera.getRotation();
//		_orientTargetRotation	= p_orientation;
//
//		App::VTXApp::get().onUpdate().addCallback(
//			this, [ this ]( const float p_deltaTime ) { update( p_deltaTime ); }
//		);
//		_orientTime = 0.f;
//
//		const bool needAnimation
//			= Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD
//			  || abs( _orientTargetDistance - _orientStartingDistance ) > ORIENT_THRESHOLD;
//
//		if ( !needAnimation )
//		{
//			_finish();
//		}
//	}
//
//	void OrientAnimationTrackball::update( const float & p_deltaTime )
//	{
//		const Vec3f currentTarget
//			= Util::Math::easeInOutInterpolation( _orientStartingPosition, _orientTargetPosition, p_deltaTime );
//
//		const float distance
//			= Util::Math::easeInOutInterpolation( _orientStartingDistance, _orientTargetDistance, p_deltaTime );
//
//		_camera.rotateAround( QUATF_ID, currentTarget, distance );
//		_camera.setRotation(
//			Util::Math::easeInOutInterpolation( _orientStartingRotation, _orientTargetRotation, p_deltaTime )
//		);
//	}
//
//	void OrientAnimationTrackball::_finish()
//	{
//		App::VTXApp::get().onUpdate().removeCallback( this );
//		// setTarget( currentTarget );
//		//_velocity = VEC3F_ZERO;
//	}
//
// } // namespace VTX::UI::QT::Controller
