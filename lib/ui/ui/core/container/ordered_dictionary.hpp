#ifndef __VTX_UI_CORE_CONTAINER_ORDERED_DICTIONARY__
#define __VTX_UI_CORE_CONTAINER_ORDERED_DICTIONARY__

#include <map>
#include <vector>

namespace VTX::UI::Core::Container
{
	template<typename Key, typename Value>
	class OrderedDictionary
	{
	  public:
		Value & operator[ const Key & p_key ]
		{
			std::map<Key, int>::const_iterator it = _mapKeyTab.find( p_key );

			Value & res;
			if ( it == _mapKeyTab.end() )
			{
				res = _tabs[ it->second ];
			}
			else
			{
				emplace( p_key, Value() );
				res = _mapKeyTab[ p_key ];
			}

			return res;
		}

		void emplace( const Key & p_key, Value & p_value )
		{
			int valueIndex = _tabs.size();
			_tabs.emplace_back( p_value );

			_mapKeyTab[ p_key ] = valueIndex;
		}

		bool exists( const Key & p_key )
		{
			int valueIndex = _tabs.size();
			_tabs.emplace_back( p_value );

			_mapKeyTab[ p_key ] = valueIndex;
		}

	  private:
		std::vector<Value> _tabs	  = std::vector<Value>();
		std::map<Key, int> _mapKeyTab = std::map<Key, int>();
	};
} // namespace VTX::UI::Core::Container

#endif
