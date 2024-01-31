#include "ui/qt/input/key_sequence.hpp"

namespace VTX::UI::QT::Input
{
	bool KeySequence::match( const Key & p_key, const ModifierFlag & p_modifier ) const
	{
		return _key == p_key && _modifier == p_modifier;
	}

	bool operator==( const KeySequence & p_lhs, const KeySequence & p_rhs )
	{
		return p_lhs._key == p_rhs._key && p_lhs._modifier == p_rhs._modifier;
	}
	std::partial_ordering operator<=>( const KeySequence & p_lhs, const KeySequence & p_rhs )
	{
		if ( p_lhs._key == p_rhs._key )
			return p_lhs._modifier <=> p_rhs._modifier;

		return p_lhs._key <=> p_rhs._key;
	}
} // namespace VTX::UI::QT::Input
