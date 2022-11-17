#ifndef __VTX_UTIL_FILEPATH__
#define __VTX_UTIL_FILEPATH__

#include <filesystem>
#include <iostream>
#include <string>

namespace VTX::Util
{

	class FilePath
	{
	  public:
		FilePath() : _path( std::string() ) {}
		FilePath( const std::string & p_path ) : _path( p_path ) {}

		inline const std::string path() const { return _path; }

		inline const std::string dirpath() const { return std::filesystem::path( _path ).parent_path().string(); }
		inline const std::string filename() const { return std::filesystem::path( _path ).filename().string(); }
		inline const std::string filenameWithoutExtension() const
		{
			return std::filesystem::path( _path ).stem().string();
		}

		inline const std::string absolute() const { return std::filesystem::absolute( _path ).string(); }

		inline const std::string extension() const { return std::filesystem::path( _path ).extension().string(); }

		inline const bool exists() const { return std::filesystem::exists( _path ); }

		inline const bool empty() const { return std::filesystem::is_empty( _path ); }

		inline friend bool operator==( const FilePath & p_lhs, const FilePath & p_rhs )
		{
			return p_lhs._path == p_rhs._path;
		}
		inline friend bool operator!=( const FilePath & p_lhs, const FilePath & p_rhs ) { return !( p_lhs == p_rhs ); }

		FilePath & operator+=( const FilePath & p_filepath )
		{
			_path += p_filepath._path;
			return *this;
		}

		friend FilePath operator+( FilePath p_lhs, const FilePath & p_rhs )
		{
			p_lhs += p_rhs;
			return p_lhs;
		}

		friend FilePath operator+( FilePath p_lhs, const std::string & p_rhs )
		{
			p_lhs += Util::FilePath( p_rhs );
			return p_lhs;
		}

		FilePath & operator/=( const FilePath & p_filepath )
		{
			_path += "/";
			_path += p_filepath._path;
			return *this;
		}

		friend FilePath operator/( FilePath p_lhs, const FilePath & p_rhs )
		{
			p_lhs /= p_rhs;
			return p_lhs;
		}

		friend FilePath operator/( FilePath p_lhs, const std::string & p_rhs )
		{
			p_lhs /= Util::FilePath( p_rhs );
			return p_lhs;
		}

		inline friend bool operator<( const FilePath & p_lhs, const FilePath & p_rhs )
		{
			return p_lhs._path < p_rhs._path;
		}

		friend std::ostream & operator<<( std::ostream & p_os, const FilePath & p_filename )
		{
			p_os << p_filename.path();
			return p_os;
		}

	  private:
		std::string _path;
	};
} // namespace VTX::Util

#endif
