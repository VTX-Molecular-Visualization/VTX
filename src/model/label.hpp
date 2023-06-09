#ifndef __VTX_MODEL_LABEL__
#define __VTX_MODEL_LABEL__

#include "color/rgba.hpp"
#include "define.hpp"
#include "generic/base_scene_item.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "object3d/helper/aabb.hpp"

namespace VTX::Model
{
	class Label : public BaseModel, public Generic::BaseSceneItem
	{
		VTX_MODEL

	  public:
		// BaseSceneItem
		const Model::ID & getModelID() const override { return getId(); }

		void		  setName( const std::string & p_name );
		const Vec3f & getPosition() const;
		void		  setPosition( const Vec3f & p_position );

		const Object3D::Helper::AABB & getAABB();

		bool hasAutoNaming() const { return _autoNaming; };
		void setAutoNaming( const bool p_autoNaming, const bool p_notify = true );

		virtual Vec2f getSize() const;

		const Color::Rgba & getColor() const { return _color; }
		void				setColor( const Color::Rgba & p_color );

	  protected:
		Label();
		Label( const VTX::ID::VTX_ID & p_typeId );

		virtual ~Label() {}

		virtual void  _performAutoName( const bool p_notify = true ) {};
		std::string & _getName() { return _name; }

		void		 _invalidateAABB() { _aabb.invalidate(); }
		virtual void _recomputeAABB( Object3D::Helper::AABB & p_aabb );

	  private:
		std::string _name		= "";
		bool		_autoNaming = false;
		Color::Rgba _color;
		Vec3f		_worldPosition = VEC3F_ZERO;

		Object3D::Helper::AABB _aabb = Object3D::Helper::AABB();
	};

} // namespace VTX::Model
#endif
