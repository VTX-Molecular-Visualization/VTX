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

			const std::string str() const { return _path; }
			const char *	  c_str() const { return _path.c_str(); }

			operator std::string() const { return _path; }

			bool		isEmpty() const;
			std::string getDirectory() const;
			std::string getFileName() const;
			std::string getExtension() const;
			std::string getFileNameWithoutExtension() const;

			Path				  operator+( const Path & ) const;
			bool				  operator==( const Path & ) const;
			bool				  operator!=( const Path & ) const;
			friend std::ostream & operator<<( std::ostream &, const Path & );
			friend std::istream & operator>>( std::istream &, Path & );

			static std::string read( const Path & );

		  private:
			std::string _path = "";

			void format();
		};
	} // namespace IO
} // namespace VTX

#endif
