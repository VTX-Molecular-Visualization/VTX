#include "app/old/action/viewpoint.hpp"
#include "app/old/component/video/path.hpp"
#include "app/old/mvc.hpp"
#include <set>

namespace VTX::App::Old::Action::Viewpoint
{
	// void AddAction::execute() { _viewpoint.addAction( _action ); }
	// void DeleteAction::execute() { _viewpoint.removeAction( _action ); }

	void ChangeDuration::execute()
	{
		_viewpoint.setDuration( Util::Math::max<float>( _duration, 0.f ) );
		_viewpoint.getPathPtr()->refreshAllDurations();
	}

	void Relocate::execute()
	{
		std::set<App::Old::Component::Video::Path *> paths = std::set<App::Old::Component::Video::Path *>();

		for ( App::Old::Component::Object3D::Viewpoint * const viewpoint : _viewpoints )
		{
			viewpoint->setPosition( _position );
			viewpoint->setRotation( _rotation );
			paths.emplace( viewpoint->getPathPtr() );
		}

		for ( App::Old::Component::Video::Path * const path : paths )
			path->refreshAllDurations();
	}

	void Translate::execute()
	{
		std::set<App::Old::Component::Video::Path *> paths = std::set<App::Old::Component::Video::Path *>();

		for ( App::Old::Component::Object3D::Viewpoint * const viewpoint : _viewpoints )
		{
			viewpoint->setPosition( viewpoint->getPosition() + _translation );
			paths.emplace( viewpoint->getPathPtr() );
		}

		for ( App::Old::Component::Video::Path * const path : paths )
			path->refreshAllDurations();
	}

	void Rotate::execute()
	{
		for ( App::Old::Component::Object3D::Viewpoint * const viewpoint : _viewpoints )
		{
			switch ( _rotationType )
			{
			case RotationType::Axis_Angle:
			{
				Quatf newRotation = Util::Math::rotate( viewpoint->getRotation(), _angle, _axis );
				viewpoint->setRotation( newRotation );
				break;
			}
			case RotationType::Euler:
			{
				Quatf deltaRotation = Util::Math::eulerToQuaternion( _axis );
				viewpoint->setRotation( viewpoint->getRotation() + deltaRotation );
				break;
			}
			default: break;
			}
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Rename::execute() { _viewpoint.setName( _name ); }

} // namespace VTX::App::Old::Action::Viewpoint
