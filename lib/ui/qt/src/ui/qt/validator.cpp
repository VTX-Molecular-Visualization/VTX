#include "ui/qt/validator.hpp"
#include <utility>

namespace VTX::UI::QT
{
	QUInt64Validator::QUInt64Validator( const uint64_t p_min, const uint64_t p_max, QObject * const p_parent ) :
		QValidator( p_parent ), _min( p_min ), _max( p_max )
	{
	}

	QUInt64Validator::QUInt64Validator( QObject * p_parent ) : QValidator( p_parent ) {}

	QValidator::State QUInt64Validator::validate( QString & input, int & pos ) const
	{
		if ( input.isEmpty() )
		{
			return State::Acceptable;
		}
		for ( const auto & char_it : std::as_const( input ) )
		{
			if ( char_it < '0' || char_it > '9' )
			{
				return State::Invalid;
			}
		}

		bool		   ok	 = false;
		const uint64_t value = input.toULongLong( &ok );
		// Out of uint64 range or above max: typing more digits can only make it bigger.
		if ( not ok || value > _max )
		{
			return State::Invalid;
		}
		// Below min: may still become valid while the user is typing.
		if ( value < _min )
		{
			return State::Intermediate;
		}
		return State::Acceptable;
	}
} // namespace VTX::UI::QT
