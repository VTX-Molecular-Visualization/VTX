#ifndef __VTX_MODEL_LABEL__
#define __VTX_MODEL_LABEL__

#include "define.hpp"
#include "id.hpp"
#include "math/aabb.hpp"
#include "model/base_model.hpp"

namespace VTX::Model
{
	class Label : public BaseModel
	{
		VTX_MODEL

	  public:
		void		  setName( const std::string & p_name );
		const Vec3f & getPosition() const;
		void		  setPosition( const Vec3f & p_position );

		const Math::AABB & getAABB();

		bool hasAutoNaming() const { return _autoNaming; };
		void setAutoNaming( const bool p_autoNaming, const bool p_notify = true );

		virtual Vec2f getSize() const;

	  protected:
		Label();
		Label( const VTX::ID::VTX_ID & p_typeId );

		virtual ~Label() {}

		virtual void  _performAutoName( const bool p_notify = true ) {};
		std::string & _getName() { return _name; }

		void		 _invalidateAABB() { _aabb.invalidate(); }
		virtual void _recomputeAABB( Math::AABB & p_aabb );

	  private:
		std::string _name		   = "";
		bool		_autoNaming	   = false;
		Vec3f		_worldPosition = VEC3F_ZERO;

		Math::AABB _aabb = Math::AABB();
	};

} // namespace VTX::Model
#endif
