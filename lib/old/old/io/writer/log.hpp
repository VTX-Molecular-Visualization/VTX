#ifndef __VTX_WRITER_LOG__
#define __VTX_WRITER_LOG__

#include "base_writer.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			class Log : public BaseWriter<std::string>
			{
			  public:
				Log( const std::string & );
				~Log();

				void writeFile( const Util::FilePath & p_path, const std::string & p_data ) override;

			  private:
				std::ofstream _os;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
