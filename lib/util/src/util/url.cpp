#include "util/url.hpp"
#include <array>
#include <fmt/format.h>
#include <string>
namespace VTX::Util::Url
{
	namespace
	{
		constexpr const char   g_systemReplacementToken[] = "[SYSTEM_ID]";
		constexpr const char   g_systemExtension[]		  = ".pdb";
		constexpr const char   g_rcsbUrl[]				  = "https://files.rcsb.org/download/";
		constexpr const size_t g_urlSize
			= sizeof( g_rcsbUrl ) - 1 + sizeof( g_systemReplacementToken ) - 1 + sizeof( g_systemExtension );

		constexpr const std::array<char, g_urlSize> _compileUrl() noexcept
		{
			using namespace std;
			array<char, g_urlSize> ret { '\0' };
			string				   buf { g_rcsbUrl };

			buf += g_systemReplacementToken;
			buf += g_systemExtension;
			for ( int idx = 0; idx < buf.size(); idx++ )
				ret[ idx ] = buf[ idx ];
			return ret;
		}
		constexpr const std::array<char, g_urlSize> g_fullRcsbUrl { _compileUrl() };

		inline void replace( std::string & p_base, const std::string & p_pattern, const std::string & subst )
		{
			p_base.replace( p_base.find( p_pattern ), p_pattern.size(), subst );
		}
	} // namespace

	const char * systemReplacementToken() { return g_systemReplacementToken; }
	const char * rcsbPdbDownloadUrlTemplate() { return g_fullRcsbUrl.data(); }
	const char * rcsbPdbDownloadBaseUrl() { return g_rcsbUrl; }
	const char * rcsbPdbDownloadFileExtension() { return g_systemExtension; }

	bool UrlTemplate::hasReplacementToken() const noexcept
	{
		return str.find( g_systemReplacementToken ) != std::string::npos;
	}

	UrlFull::UrlFull( const SystemId & p_systemId ) { replace( str, g_systemReplacementToken, p_systemId ); }
	UrlFull::UrlFull( const UrlTemplate & p_template, const SystemId & p_systemId ) : str( p_template )
	{
		replace( str, g_systemReplacementToken, p_systemId );
	}

} // namespace VTX::Util::Url
