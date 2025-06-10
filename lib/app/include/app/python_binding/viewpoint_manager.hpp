#ifndef __VTX_APP_PYTHONBINDING_VIEWPOINT_MANAGER__
#define __VTX_APP_PYTHONBINDING_VIEWPOINT_MANAGER__

#include <util/math.hpp>

namespace VTX::App::PythonBinding
{
	struct TravelViewpoint
	{
		Vec3f position;
		Quatf rotation;
		float travelTime;
	};

	/**
	 * @brief Class responsible for Gathering viewpoints, storing them and exposing them for straight travel purpose.
	 */
	class TravelViewpointManager
	{
	  public:
		struct Iterator
		{
			using iterator_category = std::contiguous_iterator_tag;
			using difference_type	= std::ptrdiff_t;
			using value_type		= TravelViewpoint;
			using pointer			= value_type *; // or also value_type*
			using reference			= value_type &; // or also value_type&

			inline reference operator*() { return *_it; }
			inline pointer	 operator->() { return _it.operator->(); }

			// Prefix increment
			inline Iterator & operator++()
			{
				_it++;
				return *this;
			}

			// Postfix increment
			inline Iterator operator++( int )
			{
				Iterator tmp = *this;
				++( *this );
				return tmp;
			}
			inline size_t operator-( const Iterator & p_other ) const noexcept { return _it - p_other._it; }

			friend bool operator==( const Iterator & a, const Iterator & b ) { return a._it == b._it; };
			friend bool operator!=( const Iterator & a, const Iterator & b ) { return a._it != b._it; };

			Iterator() = default;
			inline Iterator( std::vector<TravelViewpoint>::iterator p_it ) : _it( std::move( p_it ) ) {}

		  private:
			std::vector<TravelViewpoint>::iterator _it;
		};

		void		registerCurrent();
		void		registerCurrent( float );
		inline void setDefaultTravelTime( float p_ ) { _defaultTravelTime = std::move( p_ ); }

		Iterator begin();
		Iterator end();

		TravelViewpoint & operator[]( const size_t & );
		size_t			  size() const;

	  private:
		std::vector<TravelViewpoint> _viewpoints;
		float						 _defaultTravelTime = 300.f;
	};
} // namespace VTX::App::PythonBinding

#endif
