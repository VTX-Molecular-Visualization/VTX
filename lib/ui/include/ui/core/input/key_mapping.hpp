#ifndef __VTX_UI_CORE_INPUT_KEY_MAPPING__
#define __VTX_UI_CORE_INPUT_KEY_MAPPING__

#include "ui/core/input/keys.hpp"
#include <map>
#include <set>

namespace VTX::UI::Core::Input
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
} // namespace VTX::UI::Core::Input
#endif
