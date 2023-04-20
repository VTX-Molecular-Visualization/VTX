#include "app/model/viewpoint.hpp"

namespace VTX::Model
{
	Viewpoint::Viewpoint( Path * const p_path ) : BaseModel( VTX::ID::Model::MODEL_VIEWPOINT ), _path( p_path )
	{
		_storedName = "Viewpoint";
		_name		= &_storedName;
	}

	void Viewpoint::setPosition( const Vec3f & p_position )
	{
		_position = p_position;
		_notifyViews( new VTX::Event::VTXEvent( Event::Model::TRANSFORM_CHANGE ) );
	}
	void Viewpoint::setRotation( const Quatf & p_rotation )
	{
		_rotation = p_rotation;
		_notifyViews( new VTX::Event::VTXEvent( Event::Model::TRANSFORM_CHANGE ) );
	}
} // namespace VTX::Model
