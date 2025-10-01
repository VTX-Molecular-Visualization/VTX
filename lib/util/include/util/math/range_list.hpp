#ifndef __VTX_UTIL_MATH_RANGE_VECTOR__
#define __VTX_UTIL_MATH_RANGE_VECTOR__

#include "util/concepts.hpp"
#include "util/math/range.hpp"
#include <concepts>
#include <list>
#include <type_traits>
#include <vector>

namespace VTX::Util::Math
{
	/**
	 * @brief Class to describe a list of Range, managing merge and split when items are added / removed
	 * @tparam T Listed integer type
	 */
	template<std::integral T>
	class RangeList
	{
	  public:
		using RangeIterator		 = typename std::list<Range<T>>::iterator;
		using RangeConstIterator = typename std::list<Range<T>>::const_iterator;

		/**
		 * @brief Iterator returns each value of all the ranges
		 */
		struct Iterator
		{
			using ListIt = typename std::list<Range<T>>::const_iterator;

		  public:
			Iterator( const ListIt & p_it ) : _listIterator( p_it ) {}

			T operator*() const { return _getRangeIterator(); }

			/**
			 * @brief Prefix increment
			 */
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

			/**
			 * @brief Postfix increment
			 * @param  unused. Here for postfix convention.
			 */
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

	  public:
		/**
		 * @brief Generate RangeList from any container
		 * @tparam ContainerType Collection of T. Should be deduced from the method variable input
		 * @param p_container
		 * @return
		 */
		template<ContainerOfType<T> ContainerType>
		static RangeList<T> fromList( const ContainerType & p_container )
		{
			RangeList res = RangeList();

			for ( T value : p_container )
				res.addValue( value );

			return res;
		}

	  public:
		RangeList() {}
		explicit RangeList( const std::initializer_list<Range<T>> & p_ranges )
		{
			for ( const Range<T> & range : p_ranges )
			{
				addRange( range );
			}
		}
		explicit RangeList( const std::list<Range<T>> & p_ranges )
		{
			for ( const Range<T> & range : p_ranges )
			{
				addRange( range );
			}
		}
		RangeList( const std::initializer_list<T> & p_values )
		{
			for ( const T value : p_values )
			{
				addValue( value );
			}
		}
		RangeList( const T & p_value ) { addValue( p_value ); }

		friend bool operator==( const RangeList<T> & p_lhs, const RangeList<T> & p_rhs )
		{
			if ( p_lhs._ranges.size() != p_rhs._ranges.size() )
				return false;

			auto itLhs = p_lhs._ranges.begin();
			auto itRhs = p_rhs._ranges.begin();

			while ( itLhs != p_lhs._ranges.end() )
			{
				if ( *itLhs != *itRhs )
					return false;

				++itLhs;
				++itRhs;
			}

			return true;
		}
		friend bool operator!=( const RangeList<T> & p_lhs, const RangeList<T> & p_rhs )
		{
			if ( p_lhs._ranges.size() != p_rhs._ranges.size() )
				return true;

			auto itLhs = p_lhs._ranges.begin();
			auto itRhs = p_rhs._ranges.begin();

			while ( itLhs != p_lhs._ranges.end() )
			{
				if ( *itLhs != *itRhs )
					return true;

				++itLhs;
				++itRhs;
			}

			return false;
		}

		void addRange( const Range<T> & p_range )
		{
			// Enum to keep trace of current operation during add function
			enum class ADD_STATE : int
			{
				UNDONE,
				STARTED,
				DONE
			};

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

		Iterator begin() const { return Iterator( _ranges.begin() ); }
		Iterator end() const { return Iterator( _ranges.end() ); }

		// Keep ability to iterate on Range
		RangeIterator	   rangeBegin() { return _ranges.begin(); }
		RangeIterator	   rangeEnd() { return _ranges.end(); }
		RangeConstIterator rangeBegin() const { return _ranges.begin(); }
		RangeConstIterator rangeEnd() const { return _ranges.end(); }

		bool contains( const T p_value ) const
		{
			for ( const Range<T> & range : _ranges )
			{
				if ( range.contains( p_value ) )
					return true;
			}

			return false;
		}
		bool contains( const Range<T> & p_range ) const
		{
			for ( const Range<T> & range : _ranges )
			{
				if ( range.contains( p_range.getFirst() ) )
					return range.contains( p_range.getLast() );
			}

			return false;
		}

		template<ContainerOfType<Range<T>> C>
		bool contains( const C & p_ranges ) const
		{
			for ( const Range<T> & range : p_ranges )
			{
				if ( !contains( range ) )
				{
					return false;
				}
			}

			return true;
		}
		template<ContainerOfType<T> C>
		bool contains( const C & p_values ) const
		{
			for ( const T & value : p_values )
			{
				if ( !contains( value ) )
				{
					return false;
				}
			}

			return true;
		}

		bool contains( const RangeList & p_ranges ) const { return contains( p_ranges._ranges ); }
		bool contains( const std::initializer_list<Range<T>> & p_ranges ) const
		{
			for ( const Range<T> & range : p_ranges )
			{
				if ( !contains( range ) )
				{
					return false;
				}
			}

			return true;
		}
		bool contains( const std::initializer_list<T> & p_values ) const
		{
			for ( const T & value : p_values )
			{
				if ( !contains( value ) )
				{
					return false;
				}
			}

			return true;
		}

		bool intersectWith( const Range<T> & p_other ) const
		{
			for ( const Range<T> & range : _ranges )
			{
				if ( range.intersectWith( p_other ) )
					return true;
			}

			return false;
		}

		/**
		 * @brief Assumes the list is not empty
		 * @return First range of the list
		 */
		inline T getFirst() const { return _ranges.cbegin()->getFirst(); }

		/**
		 * @brief Assumes the list is not empty
		 * @return Last range of the list
		 */
		inline T getLast() const { return _ranges.crbegin()->getLast(); }

		void clear() { _ranges.clear(); }
		bool isEmpty() const { return _ranges.size() == 0; }

		/**
		 * @brief Return number of values held by this instance (use size to get Range count)
		 */
		T count() const
		{
			T res = 0;

			for ( const Range<T> & range : _ranges )
				res += range.getCount();

			return res;
		}

		/**
		 * @brief  Return number of range object currently used. (use size to get Range count)
		 */
		size_t rangeCount() const { return _ranges.size(); }

		/**
		 * @brief Returns the size in bytes taken by this instance. Heap size and stack size are summed up.
		 */
		size_t currentSize() const
		{
			size_t totalSize = sizeof( RangeList<T> );
			totalSize += _ranges.size() * sizeof( Range<T> );

			return totalSize;
		}

		template<typename T1, typename T2>
		void toVectors( std::vector<T1> & p_starts, std::vector<T2> & p_counts ) const
		{
			p_starts.resize( _ranges.size() );
			p_counts.resize( _ranges.size() );

			size_t i = 0;
			for ( const Range<T> & range : _ranges )
			{
				p_starts[ i ] = T1( range.getFirst() );
				p_counts[ i ] = T2( range.getCount() );
				i++;
			}
		}

	  private:
		std::list<Range<T>> _ranges;
	};
} // namespace VTX::Util::Math

#endif
