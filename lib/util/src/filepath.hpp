#ifndef __VTX_UTIL_FILEPATH__
#define __VTX_UTIL_FILEPATH__

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <iostream>
#include <string>

namespace VTX::Util
{

	class FilePath
	{
	  public:
		FilePath() : _path( QString() ) {}
		FilePath( const std::string & p_path ) : _path( QString::fromStdString( p_path ) ) {}

		inline const std::string path() const { return _path.toStdString(); }
		inline const QString &	 qpath() const { return _path; }

		inline const std::string dirpath() const { return QFileInfo( _path ).dir().path().toStdString(); }
		inline const std::string dirname() const { return QFileInfo( _path ).dir().dirName().toStdString(); }
		inline const std::string filename() const { return QFileInfo( _path ).fileName().toStdString(); }
		inline const std::string filenameWithoutExtension() const
		{
			return QFileInfo( _path ).baseName().toStdString();
		}

		inline const std::string absolute() const { return QFileInfo( _path ).absoluteFilePath().toStdString(); }

		inline const std::string extension() const { return QFileInfo( _path ).suffix().toStdString(); }

		inline const bool exists() const { return QFile::exists( _path ); }

		inline const bool empty() const { return _path.isEmpty(); }

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
		QString _path;
	};
} // namespace VTX::Util

#endif
