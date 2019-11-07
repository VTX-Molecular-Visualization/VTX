#ifndef __VTX_ATOM__
#define __VTX_ATOM__

#include "../defines.hpp"

namespace VTX
{
	namespace Model
	{
		enum ATOM_TYPE
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
			uint getId() const { return _id; };
			void setId( const uint p_id ) { _id = p_id; };

		  private:
			uint	  _id	= INVALID_ID;
			ATOM_TYPE _type = ATOM_TYPE::INVALID_TYPE;
		};

	} // namespace Model
} // namespace VTX
#endif
