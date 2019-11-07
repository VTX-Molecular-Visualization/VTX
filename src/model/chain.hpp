#ifndef __VTX_CHAIN__
#define __VTX_CHAIN__

#include "../defines.hpp"
#include <iostream>

namespace VTX
{
	namespace Model
	{
		class Chain
		{
		  public:
			uint		getId() const { return _id; };
			void		setId( const uint p_id ) { _id = p_id; };
			std::string getName() const { return _name; };
			void setName( const std::string & p_name ) { _name = p_name; };

		  private:
			uint		_id	  = INVALID_ID;
			std::string _name = "";
		};

	} // namespace Model
} // namespace VTX
#endif
