#include "python_binding/command_filter.hpp"

#include <string_view>
#include <vector>
#include <algorithm>

namespace VTX::PythonBinding
{
	FilterResult::FilterResult( bool p_pass, std::string p_why ) : _harmful( p_pass ), _why( std::move( p_why ) ) {}
	FilterResult::FilterResult( bool p_pass ) : _harmful( p_pass ) {}
	FilterResult::operator bool() const noexcept { return _harmful; }
	bool				FilterResult::isHarmful() const noexcept { return _harmful; }
	const std::string & FilterResult::why() const noexcept
	{ return _why;
	}

	namespace
	{
		struct UnwantedCommand
		{
			std::string_view str;
			std::string_view reason;
			bool			 operator==( const UnwantedCommand & p_) const { return str == p_; }
			bool			 operator==( const std::string_view & p_ ) const { return str == p_; }
		};
		const std::vector<UnwantedCommand> g_FILTERED_COMMANDS { { "help", "Interactive helper disabled in VTX. Please use help() to know what's available." } };
	}

	FilterResult				filter( const std::string & p_command) noexcept { 
		auto findResult = std::find_if(
			g_FILTERED_COMMANDS.begin(),
			g_FILTERED_COMMANDS.end(),
			[ &p_command ]( const UnwantedCommand & p_ ) { return p_ == p_command; }
		);
		if ( findResult == std::end( g_FILTERED_COMMANDS ) )
			return FilterResult( false );
		return FilterResult( true, std::string( findResult->reason.data(), findResult->reason.size() ) );

	}
} // namespace VTX::PythonBinding