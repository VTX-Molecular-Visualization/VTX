#ifndef __EXCEPTIONS__
#define __EXCEPTIONS__

#include <iostream>
#include <string>

namespace VTX
{
	namespace Exception
	{
		class SDLException : public std::runtime_error
		{
		  public:
			explicit SDLException( const std::string & p_err ) :
				std::runtime_error( std::string( "[SDL] " ) + p_err )
			{
			}
		};

		class GLException : public std::runtime_error
		{
		  public:
			explicit GLException( const std::string & p_err ) :
				std::runtime_error( std::string( "[GL] " ) + p_err )
			{
			}
		};

		class IMGUIException : public std::runtime_error
		{
		  public:
			explicit IMGUIException( const std::string & p_err ) :
				std::runtime_error( std::string( "[IMGUI] " ) + p_err )
			{
			}
		};

		class VTXException : public std::runtime_error
		{
		  public:
			explicit VTXException( const std::string & p_err ) :
				std::runtime_error( std::string( "[VTX] " ) + p_err )
			{
			}
		};
	} // namespace Exception
} // namespace VTX

#endif
