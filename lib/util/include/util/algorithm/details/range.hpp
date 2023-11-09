#ifndef __VTX_UTIL_ALOGORITHM_DETAILS_RANGE__
#define __VTX_UTIL_ALOGORITHM_DETAILS_RANGE__

#include "util/math/range.hpp"
#include "util/math/range_list.hpp"
#include <list>
#include <optional>

namespace VTX::Util::Algorithm::Details::Range
{
	template<typename T>
	struct Exclusive
	{
	  public:
		Exclusive( const Math::RangeList<T> & p_lhs, const Math::RangeList<T> & p_rhs ) :
			_lhs( p_lhs ), _rhs( p_rhs ), _itDataLhs( ItData( _lhs.rangeBegin() ) ),
			_itDataRhs( ItData( _rhs.rangeBegin() ) )
		{
			_newRange.clear();

			_startValue.reset();
			_endValue.reset();

			_compute();
		}
		Math::RangeList<T> getRes() { return Math::RangeList<T>( _newRange ); }

	  private:
		enum class Target : int
		{
			First = 0,
			Last  = 1
		};

		struct ItData
		{
			ItData( typename std::list<Math::Range<T>>::const_iterator p_it ) : it( p_it ) {}

			operator typename std::list<Math::Range<T>>::const_iterator() const { return it; }

			T	 value() const { return state == Target::First ? it->getFirst() : it->getLast(); }
			void next()
			{
				if ( state == Target::First )
				{
					state = Target::Last;
				}
				else
				{
					it++;
					state = Target::First;
				}
			};
			bool isOpen() const { return state == Target::First; }

			typename std::list<Math::Range<T>>::const_iterator it;
			Target											   state = Target::First;
		};

		void _compute()
		{
			while ( _itDataLhs != _lhs.rangeEnd() && _itDataRhs != _rhs.rangeEnd() )
			{
				if ( _itDataLhs.value() < _itDataRhs.value() )
				{
					_pushLhsValue();
				}
				else if ( _itDataRhs.value() < _itDataLhs.value() )
				{
					_pushRhsValue();
				}
				else
				{
					if ( _startValue )
					{
						_endValue = _itDataLhs.value() - 1;
						_tryPushRange();

						_startValue = _itDataLhs.value() + 1;
						_endValue.reset();
					}

					_itDataLhs.next();
					_itDataRhs.next();
				}

				_tryPushRange();
			}

			while ( _itDataLhs != _lhs.rangeEnd() )
			{
				_pushLhsValue();
				_tryPushRange();
			}

			while ( _itDataRhs != _rhs.rangeEnd() )
			{
				_pushRhsValue();
				_tryPushRange();
			}
		}

		void _pushLhsValue()
		{
			T value = _itDataLhs.value();

			if ( !_startValue )
			{
				_startValue = ( _itDataRhs.state == Target::Last ) ? T( value + 1 ) : value;
			}
			else
			{
				_endValue = ( _itDataRhs.state == Target::Last ) ? T( value - 1 ) : value;
			}

			_itDataLhs.next();
		}
		void _pushRhsValue()
		{
			T value = _itDataRhs.value();

			if ( !_startValue )
			{
				_startValue = ( _itDataLhs.state == Target::Last ) ? T( value + 1 ) : value;
			}
			else
			{
				_endValue = ( _itDataLhs.state == Target::Last ) ? T( value - 1 ) : value;
			}

			_itDataRhs.next();
		}
		void _tryPushRange()
		{
			if ( _endValue )
			{
				if ( _newRange.size() > 0 && ( ( _newRange.rbegin()->getLast() + 1 ) == _startValue.value() ) )
					_newRange.rbegin()->setLast( _endValue.value() );
				else
					_newRange.emplace_back( Math::Range<T>::createFirstLast( _startValue.value(), _endValue.value() ) );

				_startValue.reset();
				_endValue.reset();
			}
		}

		const Math::RangeList<T> & _lhs;
		const Math::RangeList<T> & _rhs;

		std::list<Math::Range<T>> _newRange = std::list<Math::Range<T>>();

		ItData _itDataLhs;
		ItData _itDataRhs;

		std::optional<T> _startValue = std::optional<T>();
		std::optional<T> _endValue	 = std::optional<T>();
	};
} // namespace VTX::Util::Algorithm::Details::Range

#endif
