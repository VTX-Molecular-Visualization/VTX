#include "app/application/settings.hpp"

namespace VTX::App::Application
{
	void Settings::reset()
	{
		for ( auto & pair : _settings )
		{
			if ( pair.second.isPtr() )
			{
				*( pair.second.getPtr<std::any>() ) = *( _defaultValues[ pair.first ].getPtr<std::any>() );
			}
			else
			{
				pair.second = _defaultValues[ pair.first ];
			}
		}
	}
} // namespace VTX::App::Application
