#ifndef BCS_CORE_META_HPP
#define BCS_CORE_META_HPP

#include <type_traits>

#define BCS_DEFINE_ENUM_BITWISE_FUNCTION( Type )                                     \
    inline constexpr Type operator&( const Type l, const Type r )                    \
    {                                                                                \
        return static_cast<Type>( bcs::toUnderlying( l ) & bcs::toUnderlying( r ) ); \
    }                                                                                \
                                                                                     \
    inline constexpr Type operator|( const Type l, const Type r )                    \
    {                                                                                \
        return static_cast<Type>( bcs::toUnderlying( l ) | bcs::toUnderlying( r ) ); \
    }                                                                                \
                                                                                     \
    inline constexpr Type operator~( const Type m )                                  \
    {                                                                                \
        return static_cast<Type>( ~bcs::toUnderlying( m ) );                         \
    }

namespace bcs
{
    template<class Enum>
    constexpr typename std::underlying_type<Enum>::type toUnderlying( const Enum e ) noexcept
    {
        return static_cast<typename std::underlying_type<Enum>::type>( e );
    }
} // namespace bcs

#endif // BCS_CORE_META_HPP
