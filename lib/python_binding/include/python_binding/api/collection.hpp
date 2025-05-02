#ifndef __VTX_PYTHON_API_COLECTION__
#define __VTX_PYTHON_API_COLECTION__

#include <cstddef>
#include <iterator>
#include <memory>

namespace VTX::PythonBinding::API
{
	/**
	 * @brief Class responsible for using a reference to the provided container to iterate over its content, giving up
	 * each element as the type T. It means that the type contained within the provided collection must be convertible
	 * to the type T provided as a template argument.
	 * @tparam T
	 */
	template<class T>
	class Collection
	{
	  public:
		Collection() = default;

	  private:
		struct _incrementer
		{
			virtual T	   at( const size_t & p_idx ) = 0;
			virtual size_t size()					  = 0;
		};

	  public:
		struct Iterator
		{
			using iterator_category = std::contiguous_iterator_tag;
			using difference_type	= std::ptrdiff_t;
			using value_type		= T;
			using pointer			= value_type *; // or also value_type*
			using reference			= value_type &; // or also value_type&

			reference operator*() { return _obj; }
			pointer	  operator->() { return &_obj; }

			// Prefix increment
			Iterator & operator++()
			{
				_idx++;
				if ( _idx < _incr->size() )
					_obj = _incr->at( _idx );
				else
					_obj = T();
				return *this;
			}

			// Postfix increment
			Iterator operator++( int )
			{
				Iterator tmp = *this;
				++( *this );
				return tmp;
			}
			size_t operator-( const Iterator & p_other ) const noexcept { return _idx - p_other._idx; }

			friend bool operator==( const Iterator & a, const Iterator & b ) { return a._idx == b._idx; };
			friend bool operator!=( const Iterator & a, const Iterator & b ) { return a._idx != b._idx; };

			Iterator() = default;
			Iterator( T p_obj, size_t p_idx, _incrementer & p_incr ) :
				_obj( std::move( p_obj ) ), _idx( std::move( p_idx ) ), _incr( &p_incr )
			{
			}

		  private:
			T			   _obj;
			size_t		   _idx	 = 0;
			_incrementer * _incr = nullptr;
		};

		inline Iterator begin()
		{
			if ( _ptr )
				return _ptr->begin();
			return {};
		}
		inline Iterator end()
		{
			if ( _ptr )
				return _ptr->end();
			return {};
		}

		inline size_t size()
		{
			if ( _ptr )
				return _ptr->size();
			return 0xffffffffffffffff;
		}

		T operator[]( const size_t & p_ ) noexcept
		{
			if ( _ptr )
				return _ptr->at( p_ );
			return {};
		}

	  private:
		struct _interface
		{
			~_interface() = default;

			virtual T at( const size_t & ) = 0;

			virtual Iterator begin() = 0;
			virtual Iterator end()	 = 0;
			virtual size_t	 size()	 = 0;
		};

		template<class TT>
		class _wrapper final : public _interface, private _incrementer
		{
			TT & _obj;

		  public:
			_wrapper( TT & p_ ) : _obj( p_ ) {}
			virtual T		 at( const size_t & p_idx ) override { return { _obj.at( p_idx ) }; }
			virtual Iterator begin() override { return Iterator( { _obj[ 0 ] }, 0, *this ); }
			virtual Iterator end() override { return Iterator( {}, _obj.size(), *this ); }
			virtual size_t	 size() override { return _obj.size(); }
		};
		std::shared_ptr<_interface> _ptr = nullptr;

	  public:
		template<class T>
			requires( not std::same_as<std::remove_cv<T>, Collection> )
		Collection( T & p_ ) : _ptr( new _wrapper<T>( p_ ) )
		{
		}
	};
} // namespace VTX::PythonBinding::API

#endif
