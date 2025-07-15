#include "bcs/core/type.hpp"

namespace bcs
{
    template<class Type>
    Span<Type>::Span( Type * pptr, std::size_t ssize ) : ptr( pptr ), size( ssize )
    {
    }
    template<class Type>
    Span<Type>::Span( std::vector<Type> & data ) : ptr( data.data() ), size( data.size() )
    {
    }

    template<class Type>
    ConstSpan<Type>::ConstSpan( const Type * pptr, std::size_t ssize ) : ptr( pptr ), size( ssize )
    {
    }
    template<class Type>
    ConstSpan<Type>::ConstSpan( const std::vector<Type> & data ) : ptr( data.data() ), size( data.size() )
    {
    }
    template<class Type>
    ConstSpan<Type>::ConstSpan( const Span<Type> data ) : ptr( data.ptr ), size( data.size )
    {
    }

    template<class HandleType>
    HandleSpan<HandleType>::HandleSpan( HandleType hhandle, std::size_t ooffset, std::size_t ssize ) :
        handle( hhandle ), offset( ooffset ), size( ssize )
    {
    }

    template<class HandleType>
    HandleSpan<HandleType>::HandleSpan( HandleType hhandle, std::size_t ssize ) :
        handle( hhandle ), offset( 0 ), size( ssize )
    {
    }

    template<class Type>
    Type RangeBase<Type>::size() const
    {
        return end - start;
    }
} // namespace bcs
