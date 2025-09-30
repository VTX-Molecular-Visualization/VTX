#ifndef __VTX_UTIL_MONITORING_FRAME_INFO__
#define __VTX_UTIL_MONITORING_FRAME_INFO__

#include <map>
#include <string>
#include <util/chrono.hpp>
#include <util/hashing.hpp>
#include <util/variant.hpp>
#include <utility>

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
		const T get( const Hash & p_hashedKey ) const
		{
			return _metricsMap.at( p_hashedKey ).get<T>();
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
		 * @brief Frame start time point.
		 */
		Util::Chrono::TimePoint _timepoint;

		/**
		 * @brief Stored data.
		 */
		std::map<Hash, Util::VTXVariant> _metricsMap;
	};
} // namespace VTX::Util::Monitoring
#endif
