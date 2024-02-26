#ifndef __VTX_APP_OLD_COMPONENT_OBJECT3D_LABEL__
#define __VTX_APP_OLD_COMPONENT_OBJECT3D_LABEL__

#include "app/old/component/object3d/helper/aabb.hpp"
#include "app/old/core/model/base_model.hpp"
#include "app/old/core/scene/base_scene_item.hpp"
#include <util/color/rgba.hpp>
#include "app/old/id.hpp"
#include <util/types.hpp>

namespace VTX::App::Old::Component::Object3D
{
	class Label : public Core::Model::BaseModel, public Core::Scene::BaseSceneItem
	{
		VTX_MODEL

	  public:
		// BaseSceneItem
		const Core::Model::ID & getModelID() const override { return getId(); }

		void		  setName( const std::string & p_name );
		const Vec3f & getPosition() const;
		void		  setPosition( const Vec3f & p_position );

		const Component::Object3D::Helper::AABB & getAABB();

		bool hasAutoNaming() const { return _autoNaming; };
		void setAutoNaming( const bool p_autoNaming, const bool p_notify = true );

		virtual Vec2f getSize() const;

		const Util::Color::Rgba & getColor() const { return _color; }
		void				setColor( const Util::Color::Rgba & p_color );

	  protected:
		Label();
		Label( const VTX::App::Old::VTX_ID & p_typeId );

		virtual ~Label() {}

		virtual void  _performAutoName( const bool p_notify = true ) {};
		std::string & _getName() { return _name; }

		void		 _invalidateAABB() { _aabb.invalidate(); }
		virtual void _recomputeAABB( Component::Object3D::Helper::AABB & p_aabb );

	  private:
		std::string _name		= "";
		bool		_autoNaming = false;
		Util::Color::Rgba _color;
		Vec3f		_worldPosition = VEC3F_ZERO;

		Component::Object3D::Helper::AABB _aabb = Component::Object3D::Helper::AABB();
	};

} // namespace VTX::App::Old::Component::Object3D
#endif