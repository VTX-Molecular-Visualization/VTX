#ifndef __VTX_APP_VTX_UPDATER__
#define __VTX_APP_VTX_UPDATER__

#include <util/network.hpp>

namespace VTX::App::Updater
{
	void check()
	{
		// TODO: use network manager?
		const std::string_view url = "https://api.github.com/repos/VTX-Molecular-Visualization/VTX/releases/latest";

		Util::Network::httpRequestGetAsync( url, []( const std::string & p_text ) { VTX_INFO( "{}", p_text ); } );
	}

} // namespace VTX::App::Updater

#endif
