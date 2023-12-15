#include "app/application/settings.hpp"

namespace VTX::App::Application
{
	void Settings::reset()
	{
		for ( auto & pair : _settings )
		{
			pair.second->reset();
		}
	}

} // namespace VTX::App::Application
