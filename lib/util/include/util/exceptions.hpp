#ifndef __VTX_UTIL_EXCEPTIONS__
#define __VTX_UTIL_EXCEPTIONS__

#include <iostream>
#include <string>

namespace VTX
{
	class VTXException : public std::runtime_error
	{
	  public:
		explicit VTXException( const std::string & p_err ) : std::runtime_error( "[VTX] " + p_err ) {}
	};

	class GLException : public std::runtime_error
	{
	  public:
		explicit GLException( const std::string & p_err ) : std::runtime_error( "[GL] " + p_err ) {}
	};

	class HTTPException : public std::runtime_error
	{
	  public:
		explicit HTTPException( const std::string & p_err ) : std::runtime_error( "[HTTP] " + p_err ) {}
	};

	class IOException : public std::runtime_error
	{
	  public:
		explicit IOException( const std::string & p_err ) : std::runtime_error( "[IO] " + p_err ) {}
	};

	class MathException : public std::runtime_error
	{
	  public:
		explicit MathException( const std::string & p_err ) : std::runtime_error( "[MATH] " + p_err ) {}
	};

	class LibException : public std::runtime_error
	{
	  public:
		explicit LibException( const std::string & p_err ) : std::runtime_error( "[LIB] " + p_err ) {}
	};

	class CommandException : public std::runtime_error
	{
	  public:
		explicit CommandException( const std::string & p_command, const std::string & p_err ) :
			std::runtime_error( "[Command] " + p_command + " : " + p_err )
		{
		}
	};

	class NotImplementedException : public std::runtime_error
	{
	  public:
		explicit NotImplementedException() : std::runtime_error( "[VTX] feature not implemented" ) {}
	};
} // namespace VTX

#endif
