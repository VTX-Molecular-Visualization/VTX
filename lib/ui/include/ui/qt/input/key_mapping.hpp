#ifndef __VTX_UI_QT_INPUT_KEY_MAPPING__
#define __VTX_UI_QT_INPUT_KEY_MAPPING__

#include "keys.hpp"
#include <map>
#include <set>

namespace VTX::UI::QT::Input
{
	class KeySequence
	{
	  public:
		KeySequence( const Key & p_key, const ModifierFlag & p_modifier = ModifierEnum::None );
		bool match( const Key & p_key, const ModifierFlag & p_modifier ) const;

	  private:
		Key			 _key;
		ModifierFlag _modifier;
	};

	class KeyMapping
	{
	  public:
		KeyMapping() {};
		KeyMapping( const std::map<int, std::set<Qt::Key>> & p_map ) : _map( p_map ) {};

		const std::set<Qt::Key> & operator[]( int p_keyEnum ) const { return _map.at( p_keyEnum ); }

	  private:
		std::map<int, std::set<Qt::Key>> _map = std::map<int, std::set<Qt::Key>>();
	};
} // namespace VTX::UI::QT::Input
#endif
