#include "label.hpp"

namespace VTX::Model
{
	Label::Label( const VTX::ID::VTX_ID & p_typeId ) : BaseModel( p_typeId )
	{
		const std::string *& namePtr = _getNamePtr();
		namePtr						 = &_name;
	};
	Label::Label() : Label( VTX::ID::Model::MODEL_LABEL ) {};

	void Label::setName( const std::string & p_name )
	{
		_name = p_name;
		_notifyViews( new Event::VTXEvent( Event::Model::DISPLAY_NAME_CHANGE ) );
	}

	void Label::setAutoNaming( const bool p_autoNaming, const bool p_notify )
	{
		_autoNaming = p_autoNaming;

		if ( _autoNaming )
			_performAutoName( p_notify );
	}

	const Math::AABB & Label::getAABB()
	{
		if ( !_aabb.isValid() )
			_recomputeAABB( _aabb );

		return _aabb;
	}

	void Label::_recomputeAABB( Math::AABB & p_aabb ) { p_aabb = Math::AABB(); }

	Vec2f Label::getSize() const { return Vec2f( 0, 0 ); }

	const Vec3f & Label::getPosition() const { return _worldPosition; }
	void		  Label::setPosition( const Vec3f & p_position ) { _worldPosition = p_position; }

} // namespace VTX::Model
