#ifndef __VTX_APP_CORE_INPUT_KEY_MAPPING__
#define __VTX_APP_CORE_INPUT_KEY_MAPPING__

#include "keys.hpp"
#include <map>
#include <set>

namespace VTX::App::Core::Input
{
	class KeyMapping
	{
	  public:
		using action_index_t = int;

		KeyMapping() {};
		KeyMapping( const std::map<action_index_t, std::set<Key>> & p_map ) : _map( p_map ) {};

		const std::set<Key> & operator[]( const action_index_t p_keyEnum ) const { return _map.at( p_keyEnum ); }

	  private:
		std::map<action_index_t, std::set<Key>> _map = std::map<action_index_t, std::set<Key>>();
	};
} // namespace VTX::App::Core::Input
#endif
