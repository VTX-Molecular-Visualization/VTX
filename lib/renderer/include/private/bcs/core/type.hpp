#ifndef BCS_CORE_TYPE_HPP
#define BCS_CORE_TYPE_HPP

#include <cstddef>
#include <vector>

namespace bcs
{
    template<class Type>
    struct Span
    {
        Type *      ptr  = nullptr;
        std::size_t size = 0;

        Span() = default;
        Span( Type * ptr, std::size_t size );
        Span( std::vector<Type> & data );
    };

    template<class Type>
    struct ConstSpan
    {
        const Type * ptr  = nullptr;
        std::size_t  size = 0;

        ConstSpan() = default;
        ConstSpan( const Type * ptr, std::size_t size );
        ConstSpan( const std::vector<Type> & data );
        ConstSpan( const Span<Type> data );
    };

    template<class HandleType>
    struct HandleSpan
    {
        HandleType  handle;
        std::size_t offset = 0;
        std::size_t size   = 0;

        HandleSpan() = default;
        HandleSpan( HandleType handle, std::size_t offset, std::size_t size );
        HandleSpan( HandleType handle, std::size_t size );
    };

    template<class Type>
    struct RangeBase
    {
        Type start;
        Type end;

        Type size() const;
    };

    using Range = RangeBase<std::size_t>;
} // namespace bcs

#include "bcs/core/type.inl"

#endif // BCS_CORE_TYPE_HPP
