#ifndef __VTX_PATH_FAKE__
#define __VTX_PATH_FAKE__

#ifdef _MSC_VER
#pragma once
#endif

#include "path.hpp"

namespace VTX
{
	namespace IO
	{
		class PathFake : public Path
		{
		  public:
			PathFake( const char * p_path ) : Path( p_path ) {}
			PathFake( const std::string & p_path ) : Path( p_path ) {}

			virtual std::string read() const override { return _data; }
			virtual void		write( const std::string & p_data ) override { _data = p_data; }

		  private:
			std::string _data = "";
		};
	} // namespace IO
} // namespace VTX

#endif
