/*
 * File: exceptions.hpp
 * Author: Maxime MARIA
 */

#ifndef __EXCEPTIONS_HPP__
#define __EXCEPTIONS_HPP__

#include <iostream>
#include <string>

namespace vidocklab
{
	class GLException : public std::runtime_error
	{
	public:
		explicit GLException(const std::string &err)
			: std::runtime_error(std::string("GL exception: ") + err)
		{
		}
	};

	class GLSLException : public std::runtime_error
	{
	public:
		explicit GLSLException(const std::string &err) 
			: std::runtime_error(std::string("GLSL exception: ") + err)
		{
		}
	};

	class SDLException : public std::runtime_error
	{
	public:
		explicit SDLException(const std::string &err)
			: std::runtime_error(std::string("SDL exception: ") + err)
		{
		}
	};

	class PDBFileException : public std::runtime_error
	{
	public:
		explicit PDBFileException(const std::string &err)
			: std::runtime_error(std::string("PDBFile exception: ") + err)
		{
		}
	};

	class MMTFIOException : public std::runtime_error
	{
	public:
		explicit MMTFIOException(const std::string &err)
			: std::runtime_error(std::string("MMTF I/O exception: ") + err)
		{
		}
	};

	class SingletonException : public std::runtime_error
	{
	public:
		explicit SingletonException(const std::string &err)
			: std::runtime_error(std::string("Singleton exception: ") + err)
		{
		}
	};

} // namespace vidocklab

#endif // __EXCEPTIONS_HPP__
