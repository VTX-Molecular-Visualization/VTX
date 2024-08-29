#include "ui/qt/validator.hpp"
#include <string>

namespace VTX::UI::QT
{
	QUInt64Validator::QUInt64Validator( uint64_t p_min, uint64_t p_max, QObject * p_parent ) :
		QValidator( p_parent ), _min( std::move( p_min ) ), _max( std::move( p_max ) )
	{
	}
	QUInt64Validator::QUInt64Validator( QObject * p_parent ) : QValidator( p_parent ) {}
	QValidator::State QUInt64Validator::validate( QString & input, int & pos ) const
	{
		if ( input.isEmpty() )
			return State::Acceptable;
		for ( auto & char_it : input )
		{
			if ( char_it < '0' || char_it > '9' )
				return State::Invalid;
		}
		std::string str = input.toStdString();
		try
		{
			uint64_t unused = std::stoull( str );
		}
		catch ( std::invalid_argument & )
		{
			return State::Invalid;
		}
		catch ( std::out_of_range & )
		{
			return State::Invalid;
		}
		return State::Acceptable;
	}
} // namespace VTX::UI::QT
