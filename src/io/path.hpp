#ifndef __VTX_PATH__
#define __VTX_PATH__

#include <istream>
#include <string>

namespace VTX
{
	namespace IO
	{
		class Path
		{
		  public:
#ifdef _WIN32
			static const char PATH_SEPARATOR = '\\';
#else
			static const char PATH_SEPARATOR = '/';
#endif

			Path() = default;
			Path( const char * );
			Path( const std::string & );

			const char * Path::c_str() const;
			bool		 isEmpty() const;
			std::string	 getDirectory() const;
			std::string	 getFileName() const;
			std::string	 Path::getExtension() const;
			std::string	 Path::getFileNameWithoutExtension() const;

			Path operator+( const Path & p_path ) const;
			bool operator==( const Path & ) const;
			bool operator!=( const Path & ) const;

		  private:
			std::string _path = "";

			void Path::format();
		};
	} // namespace IO
} // namespace VTX

#endif
