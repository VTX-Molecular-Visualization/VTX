#include "path.hpp"

namespace VTX
{
	namespace IO
	{
		Path::Path( const char * p_path ) : _path( p_path ) { format(); };

		Path::Path( const std::string & p_path ) : _path( p_path ) { format(); }

		bool Path::isEmpty() const { return _path.empty(); }

		const char * Path::c_str() const { return _path.c_str(); }

		std::string Path::getDirectory() const
		{
			const size_t pos = _path.find_last_of( PATH_SEPARATOR );
			if ( pos == std::string::npos ) { return _path; }
			return _path.substr( 0, pos );
		}

		std::string Path::getFileName() const
		{
			const size_t pos = _path.find_last_of( PATH_SEPARATOR );
			if ( pos == std::string::npos ) { return _path; }
			return _path.substr( pos + 1 );
		}

		std::string Path::getExtension() const
		{
			const size_t pos = _path.find_last_of( '.' );
			if ( pos == std::string::npos || pos == 0 ) { return ""; }
			return _path.substr( pos + 1 );
		}

		std::string Path::getFileNameWithoutExtension() const
		{
			std::string	 str = getFileName();
			const size_t pos = str.find_last_of( '.' );
			if ( ( pos != std::string::npos ) && ( pos != 0 ) ) str = str.substr( 0, pos );
			return str;
		}

		Path Path::operator+( const Path & p_path ) const
		{
			if ( _path.empty() ) { return p_path; }
			else if ( p_path.isEmpty() )
			{
				return *this;
			}
			Path res( *this );
			if ( p_path._path.front() != PATH_SEPARATOR ) { res._path += PATH_SEPARATOR; }
			res._path += p_path._path;
			return res;
		}

		bool Path::operator==( const Path & p_path ) const { return _path == p_path._path; }

		bool Path::operator!=( const Path & p_path ) const { return !operator==( p_path ); }

		void Path::format()
		{
			for ( size_t i = 0; i < _path.size(); ++i )
			{
				if ( _path[ i ] == '\\' || _path[ i ] == '/' ) { _path[ i ] = PATH_SEPARATOR; }
			}
			while ( !_path.empty() && _path.back() == PATH_SEPARATOR )
			{
				_path.pop_back();
			}
		}

	} // namespace IO
} // namespace VTX
