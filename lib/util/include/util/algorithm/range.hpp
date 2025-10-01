#ifndef __VTX_UTIL_ALOGORITHM_RANGE__
#define __VTX_UTIL_ALOGORITHM_RANGE__

#include "details/range.hpp"
#include "util/math.hpp"
#include "util/math/range.hpp"
#include "util/math/range_list.hpp"
#include <concepts>
#include <list>

namespace VTX::Util::Algorithm::Range
{
	template<std::integral T>
	using RangeList = Math::RangeList<T>;

	// Merge two RangeList and return the result
	template<std::integral T>
	RangeList<T> merge( const RangeList<T> & p_lhs, const RangeList<T> & p_rhs )
	{
		RangeList<T> res = RangeList<T>( p_lhs );

		for ( typename RangeList<T>::RangeConstIterator itRange = p_rhs.rangeBegin(); itRange != p_rhs.rangeEnd();
			  ++itRange )
		{
			res.addRange( *itRange );
		}

		return res;
	}

	// Merge RangeList p_other directly in p_obj
	template<std::integral T>
	void mergeInSitu( RangeList<T> & p_obj, const RangeList<T> & p_other )
	{
		for ( typename RangeList<T>::RangeConstIterator itRange = p_other.rangeBegin(); itRange != p_other.rangeEnd();
			  ++itRange )
		{
			p_obj.addRange( *itRange );
		}
	}

	// Substract RangeList p_rhs to p_lhs and return the result
	template<std::integral T>
	RangeList<T> substract( const RangeList<T> & p_lhs, const RangeList<T> & p_rhs )
	{
		RangeList<T> res = RangeList<T>( p_lhs );

		for ( typename RangeList<T>::RangeConstIterator itRange = p_rhs.rangeBegin(); itRange != p_rhs.rangeEnd();
			  ++itRange )
		{
			res.removeRange( *itRange );
		}

		return res;
	}
	// Substract directly RangeList p_other from p_obj
	template<std::integral T>
	void substractInSitu( RangeList<T> & p_obj, const RangeList<T> & p_other )
	{
		for ( typename RangeList<T>::RangeConstIterator itRange = p_other.rangeBegin(); itRange != p_other.rangeEnd();
			  ++itRange )
		{
			p_obj.removeRange( *itRange );
		}
	}

	// Return intersection between two RangeList (items which are in p_lhs and in p_rhs )
	template<std::integral T>
	RangeList<T> intersect( const RangeList<T> & p_lhs, const RangeList<T> & p_rhs )
	{
		std::list<Math::Range<T>> newRange = std::list<Math::Range<T>>();

		typename RangeList<T>::RangeConstIterator it	  = p_lhs.rangeBegin();
		typename RangeList<T>::RangeConstIterator itOther = p_rhs.rangeBegin();

		while ( it != p_lhs.rangeEnd() && itOther != p_rhs.rangeEnd() )
		{
			if ( it->getLast() < itOther->getFirst() )
			{
				it++;
			}
			else if ( itOther->getLast() < it->getFirst() )
			{
				itOther++;
			}
			else // Intersection
			{
				const T first = Util::Math::max( it->getFirst(), itOther->getFirst() );
				const T last  = Util::Math::min( it->getLast(), itOther->getLast() );

				newRange.emplace_back( Math::Range<T>::createFirstLast( first, last ) );

				if ( it->getLast() < itOther->getLast() )
				{
					it++;
				}
				else if ( it->getLast() > itOther->getLast() )
				{
					itOther++;
				}
				else
				{
					it++;
					itOther++;
				}
			}
		}

		return RangeList<T>( newRange );
	}

	// Set p_obj as the intersection between itself and p_other
	template<std::integral T>
	void intersectInSitu( RangeList<T> & p_obj, const RangeList<T> & p_other )
	{
		p_obj = intersect( p_obj, p_other );
	}

	// Return exclusive between two RangeList (items which are in p_lhs but not in p_rhs and items which are in p_rhs
	// but not in p_lhs  )
	template<std::integral T>
	Math::RangeList<T> exclusive( const Math::RangeList<T> & p_lhs, const Math::RangeList<T> & p_rhs )
	{
		return Details::Range::Exclusive( p_lhs, p_rhs ).getRes();
	}

	// Set p_obj as the exclusive between itself and p_other
	template<std::integral T>
	void exclusiveInSitu( RangeList<T> & p_obj, const RangeList<T> & p_other )
	{
		p_obj = Details::Range::Exclusive( p_obj, p_other ).getRes();
	}

	// Generate a RangeList of index from any container following a predicate on its items
	template<Container C, std::predicate<typename C::value_type> Predicate>
	RangeList<typename C::size_type> generateIndexRangeList( const C & p_container, const Predicate & p_predicate )
	{
		using RangeType			 = typename C::size_type;
		RangeList<RangeType> res = RangeList<RangeType>();

		for ( RangeType i = 0; i < p_container.size(); i++ )
		{
			if ( p_predicate( p_container[ i ] ) )
			{
				res.addValue( i );
			}
		}

		return res;
	}
} // namespace VTX::Util::Algorithm::Range

#endif
