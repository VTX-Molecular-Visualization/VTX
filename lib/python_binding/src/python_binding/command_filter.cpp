#include "python_binding/command_filter.hpp"
#include <array>
#include <string_view>

namespace VTX::PythonBinding
{
	FilterResult::FilterResult( bool p_pass, std::string p_why ) : _harmful( p_pass ), _why( std::move( p_why ) ) {}

	FilterResult::FilterResult( bool p_pass ) : _harmful( p_pass ) {}

	FilterResult::operator bool() const noexcept { return _harmful; }

	bool FilterResult::isHarmful() const noexcept { return _harmful; }

	const std::string & FilterResult::why() const noexcept { return _why; }

	namespace
	{
		struct UnwantedCommand
		{
			std::string_view str;
			std::string_view reason;

			bool operator==( const UnwantedCommand & p_ ) const { return str == p_; }

			bool operator==( const std::string_view & p_ ) const { return str == p_; }
		};

		constexpr std::array<UnwantedCommand, 1> g_FILTERED_COMMANDS {
			{ { "help", "Interactive helper disabled in VTX. Please use help() to know what's available." } }
		};
	} // namespace

	FilterResult filter( const std::string & p_command )
	{
		for ( const UnwantedCommand & command : g_FILTERED_COMMANDS )
		{
			if ( command == p_command )
			{
				return FilterResult( true, std::string( command.reason ) );
			}
		}
		return FilterResult( false );
	}
} // namespace VTX::PythonBinding
