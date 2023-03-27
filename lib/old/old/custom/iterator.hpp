namespace VTX::Custom
{
	template<typename T>
	class Iterator
	{
	  public:
		using iterator_category = std::forward_iterator_tag;
		using value_type		= T;
		using difference_type	= std::ptrdiff_t;
		using pointer			= T *;
		using reference			= T &;

		Iterator( T * const p_ptr ) : _ptr( p_ptr ) {}

		Iterator & operator++()
		{
			++_ptr;
			return *this;
		}

		Iterator operator++( int )
		{
			Iterator temp = *this;
			++_ptr;
			return temp;
		}

		T & operator*() const { return *_ptr; }

		bool operator==( const Iterator & p_other ) const { return _ptr == p_other._ptr; }

		bool operator!=( const Iterator & p_other ) const { return _ptr != p_other._ptr; }

	  private:
		T * _ptr;
	};
} // namespace VTX::Custom
