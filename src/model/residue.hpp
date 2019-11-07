#ifndef __VTX_RESIDUE__
#define __VTX_RESIDUE__

#include "../defines.hpp"

namespace VTX
{
	namespace Model
	{
		class Residue
		{
		  public:
			uint getId() const { return _id; };
			void setId( const uint p_id ) { _id = p_id; };

		  private:
			uint _id = INVALID_ID;
		};

	} // namespace Model
} // namespace VTX
#endif
