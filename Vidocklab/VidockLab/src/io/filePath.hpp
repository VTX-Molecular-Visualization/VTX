/*
 * File: filePath.hpp
 * Author: Maxime MARIA
 */

#ifndef __FILE_PATH_HPP__
#define __FILE_PATH_HPP__

#include <istream>
#include <string>

#include "defines.hpp"

namespace vidocklab
{
	class FilePath
	{
	public:
#ifdef _WIN32
		static const char PATH_SEPARATOR = '\\';
#else
		static const char PATH_SEPARATOR = '/';
#endif
		FilePath() = default;

		FilePath(const char *filePath) : m_filePath(filePath)
		{
			format();
		}
		FilePath(const std::string &filePath) : m_filePath(filePath)
		{
			format();
		}

		operator std::string() const
		{
			return m_filePath;
		}
		const std::string &str() const
		{
			return m_filePath;
		}
		const char *c_str() const
		{
			return m_filePath.c_str();
		}

		bool empty() const
		{
			return m_filePath.empty();
		}

		// Get directory path
		FilePath dirPath() const
		{
			const size_t pos = m_filePath.find_last_of(PATH_SEPARATOR);
			if (pos == std::string::npos)
			{
				return FilePath();
			}
			return m_filePath.substr(0, pos);
		}

		// Get file name with extension
		std::string file() const
		{
			const size_t pos = m_filePath.find_last_of(PATH_SEPARATOR);
			if (pos == std::string::npos)
			{
				return m_filePath;
			}
			return m_filePath.substr(pos + 1);
		}

		// Get file extension without .
		std::string ext() const
		{
			const size_t pos = m_filePath.find_last_of('.');
			if (pos == std::string::npos || pos == 0)
			{
				return "";
			}
			return m_filePath.substr(pos + 1);
		}

		// Get file name without extension
		std::string name() const
		{
			std::string str = file();

			const size_t pos = str.find_last_of('.');
			if ((pos != std::string::npos) && (pos != 0))
				str = str.substr(0, pos);

			return str;
		}

		// Concatenation
		FilePath operator+(const FilePath &fp) const
		{
			if (m_filePath.empty())
			{
				return fp;
			}
			else if (fp.empty())
			{
				return *this;
			}
			FilePath res(*this);
			if (fp.m_filePath.front() != PATH_SEPARATOR)
			{
				res.m_filePath += PATH_SEPARATOR;
			}
			res.m_filePath += fp.m_filePath;

			return res;
		}

		bool operator==(const FilePath &fp) const
		{
			return m_filePath == fp.m_filePath;
		}
		bool operator!=(const FilePath &fp) const
		{
			return !operator==(fp);
		}

		friend std::ostream &operator<<(std::ostream &os, const FilePath &fp)
		{
			return (os << fp.m_filePath);
		}
		friend std::istream &operator>>(std::istream &is, FilePath &fp)
		{
			is >> fp.m_filePath;
			fp.format();
			return (is);
		}

	private:
		void format()
		{
			for (size_t i = 0; i < m_filePath.size(); ++i)
			{
				if (m_filePath[i] == '\\' || m_filePath[i] == '/')
				{
					m_filePath[i] = PATH_SEPARATOR;
				}
			}
			while (!m_filePath.empty() && m_filePath.back() == PATH_SEPARATOR)
			{
				m_filePath.pop_back();
			}
		}

		std::string m_filePath = UNDEFINED_STRING;
	};
} // namespace vidocklab

#endif // __FILE_PATH_HPP__
