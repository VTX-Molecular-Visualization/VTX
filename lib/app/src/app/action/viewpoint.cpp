#include "app/action/viewpoint.hpp"
#include "app/mvc.hpp"
#include "app/model/path.hpp"
#include <set>

namespace VTX::App::Action::Viewpoint
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
		std::set<Model::Path *> paths = std::set<Model::Path *>();

		for ( Model::Viewpoint * const viewpoint : _viewpoints )
		{
			viewpoint->setPosition( _position );
			viewpoint->setRotation( _rotation );
			paths.emplace( viewpoint->getPathPtr() );
		}

		for ( Model::Path * const path : paths )
			path->refreshAllDurations();
	}

	void Translate::execute()
	{
		std::set<Model::Path *> paths = std::set<Model::Path *>();

		for ( Model::Viewpoint * const viewpoint : _viewpoints )
		{
			viewpoint->setPosition( viewpoint->getPosition() + _translation );
			paths.emplace( viewpoint->getPathPtr() );
		}

		for ( Model::Path * const path : paths )
			path->refreshAllDurations();
	}

	void Rotate::execute()
	{
		for ( Model::Viewpoint * const viewpoint : _viewpoints )
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

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Rename::execute() { _viewpoint.setName( _name ); }

} // namespace VTX::App::Action::Viewpoint
