#ifndef __VTX_UTIL_EXCEPTIONS__
#define __VTX_UTIL_EXCEPTIONS__

#include <fmt/format.h>
#include <iostream>
#include <string>

namespace VTX
{
	class Exception : public std::runtime_error
	{
	  protected:
		template<typename... Args>
		explicit Exception(
			const std::string &				  p_prepend,
			const fmt::format_string<Args...> p_fmt,
			Args &&... p_args
		) :
			std::runtime_error( formatMessage( p_prepend, fmt::format( p_fmt, std::forward<Args...>( p_args... ) ) ) )
		{
		}
		explicit Exception( const std::string & p_prepend, const std::string & p_err ) :
			std::runtime_error( formatMessage( p_prepend, p_err ) )
		{
		}

	  private:
		std::string formatMessage( const std::string & p_prepend, const std::string & p_err )
		{
			return fmt::format( "[{}] {}", p_prepend, p_err );
		}
	};

	class VTXException : public Exception
	{
	  public:
		template<typename... Args>
		explicit VTXException( const fmt::format_string<Args...> p_fmt, Args &&... p_args ) :
			Exception( "VTX", p_fmt, std::forward<Args...>( p_args... ) )
		{
		}
		explicit VTXException( const std::string & p_err ) : Exception( "VTX", p_err ) {}
	};

	class GLException : public Exception
	{
	  public:
		template<typename... Args>
		explicit GLException( const fmt::format_string<Args...> p_fmt, Args &&... p_args ) :
			Exception( "GL", p_fmt, std::forward<Args...>( p_args... ) )

		{
		}
		explicit GLException( const std::string & p_err ) : Exception( "GL", p_err ) {}
	};

	class HTTPException : public Exception
	{
	  public:
		template<typename... Args>
		explicit HTTPException( const fmt::format_string<Args...> p_fmt, Args &&... p_args ) :
			Exception( "HTTP", p_fmt, std::forward<Args...>( p_args... ) )

		{
		}
		explicit HTTPException( const std::string & p_err ) : Exception( "HTTP", p_err ) {}
	};

	class IOException : public Exception
	{
	  public:
		template<typename... Args>
		explicit IOException( const fmt::format_string<Args...> p_fmt, Args &&... p_args ) :
			Exception( "IO", p_fmt, std::forward<Args...>( p_args... ) )

		{
		}
		explicit IOException( const std::string & p_err ) : Exception( "IO", p_err ) {}
	};

	class MathException : public Exception
	{
	  public:
		template<typename... Args>
		explicit MathException( const fmt::format_string<Args...> p_fmt, Args &&... p_args ) :
			Exception( "MATH", p_fmt, std::forward<Args...>( p_args... ) )

		{
		}
		explicit MathException( const std::string & p_err ) : Exception( "MATH", p_err ) {}
	};

	class LibException : public Exception
	{
	  public:
		template<typename... Args>
		explicit LibException( const fmt::format_string<Args...> p_fmt, Args &&... p_args ) :
			Exception( "LIB", p_fmt, std::forward<Args...>( p_args... ) )

		{
		}
		explicit LibException( const std::string & p_err ) : Exception( "LIB", p_err ) {}
	};

	class PythonWrapperException : public Exception
	{
	  public:
		template<typename... Args>
		explicit PythonWrapperException( const fmt::format_string<Args...> p_fmt, Args &&... p_args ) :
			Exception( "WRAPPER", p_fmt, std::forward<Args...>( p_args... ) )

		{
		}
		explicit PythonWrapperException( const std::string & p_err ) : Exception( "WRAPPER", p_err ) {}
	};
	class CommandException : public Exception
	{
	  public:
		template<typename... Args>
		explicit CommandException(
			const std::string &				  p_command,
			const fmt::format_string<Args...> p_fmt,
			Args &&... p_args
		) :
			Exception( "COMMAND", formatMessage( p_command, fmt::format( p_fmt, std::forward<Args...>( p_args... ) ) ) )

		{
		}
		explicit CommandException( const std::string & p_command, const std::string & p_err ) :
			Exception( "COMMAND", formatMessage( p_command, p_err ) )
		{
		}

	  private:
		std::string formatMessage( const std::string & p_command, const std::string & p_err )
		{
			return fmt::format( "{} : {}", p_command, p_err );
		}
	};
	class ScriptException : public Exception
	{
	  public:
		template<typename... Args>
		explicit ScriptException(
			const std::string &				  p_scriptPath,
			const fmt::format_string<Args...> p_fmt,
			Args &&... p_args
		) :
			Exception(
				"SCRIPT",
				formatMessage( p_scriptPath, fmt::format( p_fmt, std::forward<Args...>( p_args... ) ) )
			)
		{
		}
		explicit ScriptException( const std::string & p_scriptPath, const std::string & p_err ) :
			Exception( "SCRIPT", formatMessage( p_scriptPath, p_err ) )
		{
		}

	  private:
		std::string formatMessage( const std::string & p_scriptPath, const std::string & p_err )
		{
			return fmt::format( "{} : {}", p_scriptPath, p_err );
		}
	};

	class NotImplementedException : public VTXException
	{
	  public:
		template<typename... Args>
		explicit NotImplementedException() : VTXException( "feature not implemented" )

		{
		}
	};
} // namespace VTX

#endif
