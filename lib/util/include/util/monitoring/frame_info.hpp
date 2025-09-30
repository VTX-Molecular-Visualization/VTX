#ifndef __VTX_UTIL_MONITORING_FRAME_INFO__
#define __VTX_UTIL_MONITORING_FRAME_INFO__

#include <map>
#include <util/chrono.hpp>
#include <util/hashing.hpp>
#include <utility>
#include <variant>

namespace VTX::Util::Monitoring
{
	struct FrameInfo
	{
	  public:
		FrameInfo() : _timepoint( Util::Chrono::now() ) {}

		/**
		 * @brief Sets the value associated with a given hash.
		 */
		template<typename T>
		void set( const Hash & p_hashedKey, const T & p_value )
		{
			_metricsMap[ p_hashedKey ] = p_value;
		}

		/**
		 * @brief Gets the value associated with the specified hash.
		 */
		template<typename T>
		const T & get( const Hash & p_hashedKey ) const
		{
			assert( _metricsMap.contains( p_hashedKey ) );
			assert( std::get_if<T>( &_metricsMap.at( p_hashedKey ) ) != nullptr );

			return *std::get_if<T>( &_metricsMap.at( p_hashedKey ) );
		}

		/**
		 * @brief Checks if the specified hashed key exists.
		 */
		inline bool has( const Hash & p_hashedKey ) const { return _metricsMap.contains( p_hashedKey ); }

		/**
		 * @brief Get the frame start time point.
		 */
		inline Util::Chrono::TimePoint getTimepoint() const { return _timepoint; }

	  private:
		/**
		 * @brief Possible metric types.
		 */
		using Metric = std::variant<double, float, int, uint>;

		/**
		 * @brief Frame start time point.
		 */
		Util::Chrono::TimePoint _timepoint;

		/**
		 * @brief Stored data.
		 */
		std::map<Hash, Metric> _metricsMap;
	};
} // namespace VTX::Util::Monitoring
#endif
