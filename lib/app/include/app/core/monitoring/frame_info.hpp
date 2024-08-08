#ifndef __VTX_APP_CORE_MONITORING_FRAME_INFO__
#define __VTX_APP_CORE_MONITORING_FRAME_INFO__

#include <map>
#include <string>
#include <util/hashing.hpp>
#include <util/variant.hpp>
#include <utility>

namespace VTX::App::Core::Monitoring
{
	struct FrameInfo
	{
	  public:
		using key_t = std::string;

	  public:
		FrameInfo();
		bool isValid() const;

		template<typename T>
		void set( const Hash & p_hashedKey, const T & p_value )
		{
			_metricsMap[ p_hashedKey ] = p_value;
		}
		template<typename T>
		void set( const key_t & p_key, const T & p_value )
		{
			set( Util::hash( p_key ), p_value );
		}

		template<typename T>
		const T get( const Hash & p_hashedKey ) const
		{
			return _metricsMap.at( p_hashedKey ).get<T>();
		}
		template<typename T>
		const T get( const key_t & p_key ) const
		{
			return get<T>( Util::hash( p_key ) );
		}

		bool has( const Hash & p_hashedKey ) const { return _metricsMap.contains( p_hashedKey ); }
		bool has( const key_t & p_key ) const { return _metricsMap.contains( Util::hash( p_key ) ); }

		long long getTimestamp() const { return _timestamp; }

	  private:
		long long _timestamp;

		std::map<Hash, Util::VTXVariant> _metricsMap;
	};
} // namespace VTX::App::Core::Monitoring
#endif
