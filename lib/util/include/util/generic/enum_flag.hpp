#ifndef __VTX_UTIL_GENERIC_ENUM_FLAG__
#define __VTX_UTIL_GENERIC_ENUM_FLAG__

namespace VTX
{
	// Use VTX_FLAG define to generate static | and & operation between two Enums.
	// EnumFlag will manage all the casts needed for bitwise operations
#define VTX_FLAG( FLAG_TYPE, ENUM )                                      \
	using FLAG_TYPE = ::VTX::Util::Generic::EnumFlag<ENUM>;              \
	inline FLAG_TYPE operator|( const ENUM & p_lhs, const ENUM & p_rhs ) \
	{                                                                    \
		return FLAG_TYPE( p_lhs ) | p_rhs;                               \
	};                                                                   \
	inline FLAG_TYPE operator&( const ENUM & p_lhs, const ENUM & p_rhs ) \
	{                                                                    \
		return FLAG_TYPE( p_lhs ) & p_rhs;                               \
	};

	namespace Util::Generic
	{
		template<typename T>
		class EnumFlag
		{
		  public:
			using ENUM = T;

			EnumFlag() : _enum( 0 ) {}
			EnumFlag( const ENUM & p_enum ) : _enum( int( p_enum ) ) {}
			EnumFlag( const int p_value ) : _enum( p_value ) {}

			explicit operator bool() const { return bool( _enum ); }
			explicit operator int() const { return _enum; }
					 operator ENUM() const { return ENUM( _enum ); }

			EnumFlag & operator=( const ENUM & p_other )
			{
				_enum = int( p_other );
				return *this;
			}

			bool operator==( const ENUM & p_other ) const { return ENUM( _enum ) == p_other; }
			bool operator!=( const ENUM & p_other ) const { return ENUM( _enum ) != p_other; }

			EnumFlag   operator&( const EnumFlag & p_rhs ) const { return EnumFlag( _enum & p_rhs._enum ); }
			EnumFlag   operator|( const EnumFlag & p_rhs ) const { return EnumFlag( _enum | p_rhs._enum ); }
			EnumFlag & operator&=( const EnumFlag & p_rhs )
			{
				_enum = _enum & p_rhs._enum;
				return *this;
			}
			EnumFlag & operator|=( const EnumFlag & p_rhs )
			{
				_enum = _enum | p_rhs._enum;
				return *this;
			}

			EnumFlag   operator&( const ENUM & p_rhs ) const { return EnumFlag( _enum & int( p_rhs ) ); }
			EnumFlag   operator|( const ENUM & p_rhs ) const { return EnumFlag( _enum | int( p_rhs ) ); }
			EnumFlag & operator&=( const ENUM & p_rhs )
			{
				_enum = _enum & int( p_rhs );
				return *this;
			}
			EnumFlag & operator|=( const ENUM & p_rhs )
			{
				_enum = _enum | int( p_rhs );
				return *this;
			}

		  private:
			int _enum;
		};

	} // namespace Util::Generic
} // namespace VTX

#endif
