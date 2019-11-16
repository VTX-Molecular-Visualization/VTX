#ifndef __VTX_EXCEPTIONS__
#define __VTX_EXCEPTIONS__

#include <iostream>
#include <string>

namespace VTX
{
	namespace Exception
	{
		class SDLException : public std::runtime_error
		{
		  public:
			explicit SDLException( const std::string & p_err ) : std::runtime_error( "[SDL] " + p_err ) {}
		};

		class GLException : public std::runtime_error
		{
		  public:
			explicit GLException( const std::string & p_err ) : std::runtime_error( "[GL] " + p_err ) {}
		};

		class IMGUIException : public std::runtime_error
		{
		  public:
			explicit IMGUIException( const std::string & p_err ) : std::runtime_error( "[IMGUI] " + p_err ) {}
		};

		class VTXException : public std::runtime_error
		{
		  public:
			explicit VTXException( const std::string & p_err ) : std::runtime_error( "[VTX] " + p_err ) {}
		};
	} // namespace Exception
} // namespace VTX

#endif
