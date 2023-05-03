#include "app/component/object3d/label.hpp"
#include "app/application/setting.hpp"

namespace VTX::App::Component::Object3D
{
	Label::Label( const VTX::App::VTX_ID & p_typeId ) : BaseModel( p_typeId )
	{
		const std::string *& namePtr = _getNamePtr();
		namePtr						 = &_name;
		_color						 = VTX::App::Application::Setting::DEFAULT_LABEL_COLOR;
	};
	Label::Label() : Label( App::ID::Model::MODEL_LABEL ) {};

	void Label::setName( const std::string & p_name )
	{
		_name = p_name;
		_notifyViews( App::Event::Model::DISPLAY_NAME_CHANGE );
	}

	void Label::setAutoNaming( const bool p_autoNaming, const bool p_notify )
	{
		_autoNaming = p_autoNaming;

		if ( _autoNaming )
			_performAutoName( p_notify );
	}

	void Label::setColor( const Util::Color::Rgba & p_color )
	{
		if ( _color != p_color )
		{
			_color = p_color;
			_notifyDataChanged();
		}
	}

	const App::Component::Object3D::Helper::AABB & Label::getAABB()
	{
		if ( !_aabb.isValid() )
			_recomputeAABB( _aabb );

		return _aabb;
	}

	void Label::_recomputeAABB( App::Component::Object3D::Helper::AABB & p_aabb )
	{
		p_aabb = App::Component::Object3D::Helper::AABB();
	}

	Vec2f Label::getSize() const { return Vec2f( 0, 0 ); }

	const Vec3f & Label::getPosition() const { return _worldPosition; }
	void		  Label::setPosition( const Vec3f & p_position ) { _worldPosition = p_position; }

} // namespace VTX::App::Component::Object3D
