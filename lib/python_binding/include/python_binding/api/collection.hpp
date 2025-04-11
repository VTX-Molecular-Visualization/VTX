#ifndef __VTX_PYTHON_API_COLECTION__

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

	  public:
		struct Iterator
		{
			using iterator_category = std::contiguous_iterator_tag;
			using difference_type	= std::ptrdiff_t;
			using value_type		= T;
			using pointer			= value_type *; // or also value_type*
			using reference			= value_type &; // or also value_type&
		};

		inline Iterator begin() {}
		inline Iterator end() {}

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
		};

		template<class TT>
		class _wrapper final : public _interface
		{
			TT & _obj;

		  public:
			_wrapper( TT & p_ ) : _obj( p_ ) {}
			virtual T at( const size_t & p_idx ) override { return { _obj.at( p_idx ) }; }
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
