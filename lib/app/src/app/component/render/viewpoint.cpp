#include "app/component/render/viewpoint.hpp"

namespace VTX::App::Component::Render
{
	Viewpoint::Viewpoint() {}

	void Viewpoint::setPosition( const Vec3f & p_position )
	{
		_position = p_position;
		//_notifyViews( App::Old::Event::Model::TRANSFORM_CHANGE );
	}
	void Viewpoint::setRotation( const Quatf & p_rotation )
	{
		_rotation = p_rotation;
		//_notifyViews( App::Old::Event::Model::TRANSFORM_CHANGE );
	}
} // namespace VTX::App::Component::Render
