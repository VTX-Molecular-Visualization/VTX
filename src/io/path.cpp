#include "path.hpp"
#include "define.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace VTX
{
	namespace IO
	{
		Path::Path( const char * p_path ) : _path( p_path ) { format(); };

		Path::Path( const std::string & p_path ) : _path( p_path ) { format(); }

		bool Path::isEmpty() const { return _path.empty(); }

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

		std::ostream & operator<<( std::ostream & p_os, const Path & p_path ) { return ( p_os << p_path._path ); }
		std::istream & operator>>( std::istream & p_is, Path & p_path )
		{
			p_is >> p_path._path;
			p_path.format();
			return ( p_is );
		}

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

		std::string Path::read() const
		{
			std::ifstream ifs( _path, std::ifstream::in );

			if ( ifs.is_open() == false )
			{
				VTX_ERROR( "Could not open file: " + _path );
				return "";
			}

			std::stringstream s;
			s << ifs.rdbuf();
			ifs.close();
			return s.str();
		}

	} // namespace IO
} // namespace VTX
