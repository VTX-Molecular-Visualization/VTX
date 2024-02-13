#ifndef __VTX_UI_CORE_INPUT_KEY_SEQUENCE__
#define __VTX_UI_CORE_INPUT_KEY_SEQUENCE__

#include "ui/core/input/keys.hpp"
#include <compare>

namespace VTX::UI::Core::Input
{
	class KeySequence
	{
	  public:
		KeySequence( const Key & p_key, const ModifierFlag & p_modifier = ModifierEnum::None ) :
			_key( p_key ), _modifier( p_modifier )
		{
		}
		bool match( const Key & p_key, const ModifierFlag & p_modifier ) const;

		inline Key			getKey() const { return _key; }
		inline ModifierFlag getModifiers() const { return _modifier; }

		friend bool					 operator==( const KeySequence & p_lhs, const KeySequence & p_rhs );
		friend std::partial_ordering operator<=>( const KeySequence & p_lhs, const KeySequence & p_rhs );

	  private:
		Key			 _key;
		ModifierFlag _modifier;
	};
} // namespace VTX::UI::Core::Input
#endif
