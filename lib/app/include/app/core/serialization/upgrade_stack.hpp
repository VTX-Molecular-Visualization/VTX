#ifndef __VTX_APP_CORE_SERIALIZATION_UPGRADE_STACK__
#define __VTX_APP_CORE_SERIALIZATION_UPGRADE_STACK__

#include "app/core/serialization/version.hpp"
#include <any>
#include <functional>
#include <list>
#include <map>
#include <typeindex>
#include <util/json/json.hpp>

namespace VTX::App::Core::Serialization
{
	template<typename T>
	using UpgradeFunc = std::function<void( Util::JSon::BasicJSon &, T & )>;

	template<typename T>
	class UpgradeStack
	{
	  public:
		UpgradeStack() {}

		void add( const Version & p_version, UpgradeFunc<T> & p_func )
		{
			typename std::list<std::pair<Version, UpgradeFunc<T>>>::const_iterator it = std::find_if(
				_upgradeList.begin(),
				_upgradeList.end(),
				[ p_version ]( const std::pair<Version, UpgradeFunc<T>> & p_item ) { return p_item.first > p_version; }
			);

			_upgradeList.insert( it, std::make_pair( p_version, p_func ) );
		}

		void applyUpgrades( Util::JSon::BasicJSon & p_json, T & p_obj, const Version & p_fromVersion ) const
		{
			for ( const std::pair<Version, UpgradeFunc<T>> & pair : _upgradeList )
			{
				if ( pair.first > p_fromVersion )
					pair.second( p_json, p_obj );
			}
		}

	  private:
		std::list<std::pair<Version, UpgradeFunc<T>>> _upgradeList = std::list<std::pair<Version, UpgradeFunc<T>>>();
	};
} // namespace VTX::App::Core::Serialization

#endif
