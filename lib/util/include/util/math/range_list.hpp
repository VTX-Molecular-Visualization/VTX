#ifndef __VTX_UTIL_MATH_RANGE_VECTOR__
#define __VTX_UTIL_MATH_RANGE_VECTOR__

#include "util/math/range.hpp"
#include <list>
#include <type_traits>
#include <vector>

namespace VTX::Util::Math
{
	template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
	class RangeList
	{
	  public:
		struct Iterator
		{
			using ListIt = typename std::list<Range<T>>::iterator;

		  public:
			Iterator( const ListIt & p_it ) : _listIterator( p_it ) {}

			T operator*() const { return _getRangeIterator(); }

			// Prefix increment
			Iterator & operator++()
			{
				T & rangeIterator = _getRangeIterator();

				if ( rangeIterator == _listIterator->getLast() )
				{
					++_listIterator;
					_initialized = false;
				}
				else
				{
					++rangeIterator;
				}

				return *this;
			}

			// Postfix increment
			Iterator operator++( int )
			{
				Iterator tmp		   = *this;
				T &		 rangeIterator = _getRangeIterator();

				if ( rangeIterator == _listIterator.getLast() )
				{
					++_listIterator;
				}
				else
				{
					++rangeIterator;
				}

				return tmp;
			}

			friend bool operator==( const Iterator & p_lhs, const Iterator & p_rhs )
			{
				return p_lhs._listIterator == p_rhs._listIterator
					   && ( ( !p_lhs._initialized && !p_rhs._initialized )
							|| ( p_lhs._rangeIterator == p_rhs._rangeIterator ) );
			};
			friend bool operator!=( const Iterator & p_lhs, const Iterator & p_rhs )
			{
				return p_lhs._listIterator != p_rhs._listIterator || ( p_lhs._initialized ^ p_rhs._initialized )
					   || ( p_lhs._initialized && p_rhs._initialized && p_lhs._rangeIterator != p_rhs._rangeIterator );
			};

		  private:
			T & _getRangeIterator() const
			{
				if ( !_initialized )
				{
					_rangeIterator = _listIterator->getFirst();
					_initialized   = true;
				}

				return _rangeIterator;
			}

			ListIt _listIterator;

			mutable T	 _rangeIterator;
			mutable bool _initialized = false;
		};

	  private:
		enum class ADD_STATE : int
		{
			UNDONE,
			STARTED,
			DONE
		};

	  public:
		RangeList() {}
		RangeList( const std::vector<Range<T>> & p_ranges )
		{
			for ( const Range<T> & range : p_ranges )
			{
				addRange( range );
			}
		}

		void addRange( const Range<T> & p_range )
		{
			auto	  it			  = _ranges.begin();
			ADD_STATE addState		  = ADD_STATE::UNDONE;
			auto	  modifiedRangeIt = _ranges.end();

			while ( it != _ranges.end() )
			{
				if ( it->getFirst() > ( p_range.getLast() + T( 1 ) ) )
				{
					if ( addState == ADD_STATE::UNDONE )
					{
						_ranges.insert( it, p_range );
					}
					else
					{
						modifiedRangeIt->setLast( p_range.getLast() );
					}

					addState = ADD_STATE::DONE;
					break;
				}
				else if ( ( it->getLast() + T( 1 ) ) < p_range.getFirst() )
				{
					it++;
				}
				else
				{
					switch ( addState )
					{
					case ADD_STATE::UNDONE:
					{
						addState = it->getLast() >= p_range.getLast() ? ADD_STATE::DONE : ADD_STATE::STARTED;

						it->merge( p_range );
						modifiedRangeIt = it;
						it++;
					}
					break;

					case ADD_STATE::STARTED:
					{
						if ( it->getLast() >= p_range.getLast() )
						{
							modifiedRangeIt->setLast( it->getLast() );
							addState = ADD_STATE::DONE;
						}

						it = _ranges.erase( it );
					}
					break;

					default: break;
					}

					if ( addState == ADD_STATE::DONE )
						break;
				}
			}

			if ( addState == ADD_STATE::UNDONE )
			{
				_ranges.insert( it, p_range );
			}
		}

		void removeRange( const Range<T> & p_range )
		{
			auto it = _ranges.begin();

			while ( it != _ranges.end() )
			{
				if ( it->getFirst() > p_range.getLast() )
				{
					break;
				}
				else if ( it->getLast() < p_range.getFirst() )
				{
					it++;
				}
				else
				{
					if ( it->getFirst() < p_range.getFirst() )
					{
						if ( it->getLast() > p_range.getLast() )
						{
							const T insertFirst = it->getFirst();
							const T insertLast	= p_range.getFirst() - T( 1 );

							const T newFirst = p_range.getLast() + T( 1 );

							it->setFirst( newFirst );
							it = _ranges.insert( it, Range<T>::createFirstLast( insertFirst, insertLast ) );

							break;
						}
						else
						{
							it->setLast( p_range.getFirst() - T( 1 ) );
							it++;
						}
					}
					else if ( it->getLast() > p_range.getLast() )
					{
						it->setFirst( p_range.getLast() + T( 1 ) );
						break;
					}
					else // p_range.getFirst() <=  it->getFirst() && p_range.getLast() >= it->getLast()
					{
						it = _ranges.erase( it );
					}
				}
			}
		}

		void addValue( const T p_value ) { addRange( Range<T>( p_value ) ); }
		void removeValue( const T p_value ) { removeRange( Range<T>( p_value ) ); }

		Iterator begin() { return Iterator( _ranges.begin() ); }
		Iterator end() { return Iterator( _ranges.end() ); }

		bool contains( const T p_value ) const
		{
			for ( const Range<T> & range : _ranges )
			{
				if ( range.contains( p_value ) )
					return true;
			}

			return false;
		}
		bool contains( const std::vector<T> & p_values ) const
		{
			for ( const T & value : p_values )
			{
				bool res = false;

				for ( const Range<T> & range : _ranges )
				{
					if ( range.contains( value ) )
					{
						res = true;
						break;
					}
				}

				if ( !res )
					return false;
			}

			return true;
		}
		bool contains( const Range<T> p_range ) const
		{
			for ( const Range<T> & range : _ranges )
			{
				if ( range.contains( p_range ) )
					return true;
			}

			return false;
		}
		bool contains( const std::vector<Range<T>> p_ranges ) const
		{
			for ( const Range<T> & rangeToFind : p_ranges )
			{
				bool res = false;

				for ( const Range<T> & range : _ranges )
				{
					if ( range.contains( rangeToFind ) )
					{
						res = true;
						break;
					}
				}

				if ( !res )
					return false;
			}

			return true;
		}

	  private:
		std::list<Range<T>> _ranges = std::list<Range<T>>();
	};
} // namespace VTX::Util::Math

#endif
