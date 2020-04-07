#ifndef __VTX_EXCEPTIONS__
#define __VTX_EXCEPTIONS__

#ifdef _MSC_VER
#pragma once

#endif
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

		class HTTPException : public std::runtime_error
		{
		  public:
			explicit HTTPException( const std::string & p_err ) : std::runtime_error( "[HTTP] " + p_err ) {}
		};

		class NotImplementedException : public std::runtime_error
		{
		  public:
			explicit NotImplementedException() : std::runtime_error( "[VTX] feature not implemented" ) {}
		};
	} // namespace Exception
} // namespace VTX

#endif
