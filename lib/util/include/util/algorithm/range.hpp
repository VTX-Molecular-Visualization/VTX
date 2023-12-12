#ifndef __VTX_UTIL_ALOGORITHM_RANGE__
#define __VTX_UTIL_ALOGORITHM_RANGE__

#include "details/range.hpp"
#include "util/math.hpp"
#include "util/math/range.hpp"
#include "util/math/range_list.hpp"
#include <concepts>
#include <functional>
#include <list>
#include <vector>

namespace VTX::Util::Algorithm::Range
{
	template<std::integral T>
	using RangeList = Math::RangeList<T>;

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
	template<std::integral T>
	void mergeInSitu( RangeList<T> & p_obj, const RangeList<T> & p_other )
	{
		for ( typename RangeList<T>::RangeConstIterator itRange = p_other.rangeBegin(); itRange != p_other.rangeEnd();
			  ++itRange )
		{
			p_obj.addRange( *itRange );
		}
	}

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
	template<std::integral T>
	void substractInSitu( RangeList<T> & p_obj, const RangeList<T> & p_other )
	{
		for ( typename RangeList<T>::RangeConstIterator itRange = p_other.rangeBegin(); itRange != p_other.rangeEnd();
			  ++itRange )
		{
			p_obj.removeRange( *itRange );
		}
	}

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

	template<std::integral T>
	void intersectInSitu( RangeList<T> & p_obj, const RangeList<T> & p_other )
	{
		p_obj = intersect( p_obj, p_other );
	}

	template<std::integral T>
	Math::RangeList<T> exclusive( const Math::RangeList<T> & p_lhs, const Math::RangeList<T> & p_rhs )
	{
		return Details::Range::Exclusive( p_lhs, p_rhs ).getRes();
	}

	template<std::integral T>
	void exclusiveInSitu( RangeList<T> & p_obj, const RangeList<T> & p_other )
	{
		p_obj = Details::Range::Exclusive( p_obj, p_other ).getRes();
	}

	template<typename TVector>
	RangeList<typename std::vector<TVector>::size_type> generateRangeList(
		const std::vector<TVector> &		   p_vector,
		std::function<bool( const TVector & )> p_predicate
	)
	{
		using TRange		  = typename std::vector<TVector>::size_type;
		RangeList<TRange> res = RangeList<TRange>();

		for ( TRange i = 0; i < p_vector.size(); i++ )
		{
			if ( p_predicate( p_vector[ i ] ) )
			{
				res.addValue( i );
			}
		}

		return res;
	}
} // namespace VTX::Util::Algorithm::Range

#endif
