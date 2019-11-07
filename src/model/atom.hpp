#ifndef __VTX_ATOM__
#define __VTX_ATOM__

#include "../defines.hpp"

namespace VTX
{
	namespace Model
	{
		enum class ATOM_TYPE
		{
			H,
			C,
			N,
			O,
			NA,
			MG,
			P,
			S,
			CA,
			INVALID_TYPE,
			ATOM_TYPE_COUNT
		};

		class Atom
		{
		  public:
			uint		  getId() const { return _id; };
			void		  setId( const uint p_id ) { _id = p_id; };
			ATOM_TYPE	  getType() const { return _type; };
			void		  setType( const ATOM_TYPE p_type ) { _type = p_type; };
			const Vec3f & getPosition() const { return _position; };
			void		  setPosition( const Vec3f & p_position )
			{
				_position = p_position;
			};

		  private:
			uint	  _id		= INVALID_ID;
			ATOM_TYPE _type		= ATOM_TYPE::INVALID_TYPE;
			Vec3f	  _position = VEC3F_ZERO;
		};

	} // namespace Model
} // namespace VTX
#endif
