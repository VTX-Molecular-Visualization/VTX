#include "app/component/object3d/viewpoint.hpp"

namespace VTX::App::Component::Object3D
{
	Viewpoint::Viewpoint( Video::Path * const p_path ) : BaseModel( App::ID::Model::MODEL_VIEWPOINT ), _path( p_path )
	{
		_storedName = "Viewpoint";
		_name		= &_storedName;
	}

	void Viewpoint::setPosition( const Vec3f & p_position )
	{
		_position = p_position;
		_notifyViews( App::Event::Model::TRANSFORM_CHANGE );
	}
	void Viewpoint::setRotation( const Quatf & p_rotation )
	{
		_rotation = p_rotation;
		_notifyViews( App::Event::Model::TRANSFORM_CHANGE );
	}
} // namespace VTX::App::Component::Object3D
