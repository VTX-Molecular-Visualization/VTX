#ifndef __VTX_APP_CORE_MONITORING_FRAME_INFO__
#define __VTX_APP_CORE_MONITORING_FRAME_INFO__

#include <map>
#include <string>
#include <util/variant.hpp>
#include <utility>

namespace VTX::App::Core::Monitoring
{
	struct FrameInfo
	{
	  public:
		using hash_t = size_t;
		using key_t	 = std::string;

		static hash_t hash( const key_t & p_key ) { return std::hash<std::string_view>()( std::string_view( p_key ) ); }

	  public:
		FrameInfo();
		bool isValid() const;

		template<typename T>
		void set( const hash_t & p_hashedKey, const T & p_value )
		{
			_metricsMap[ p_hashedKey ] = p_value;
		}
		template<typename T>
		void set( const key_t & p_key, const T & p_value )
		{
			set( hash( p_key ), p_value );
		}

		template<typename T>
		const T get( const hash_t & p_hashedKey ) const
		{
			return _metricsMap.at( p_hashedKey ).get<T>();
		}
		template<typename T>
		const T get( const key_t & p_key ) const
		{
			return get( hash( p_key ) );
		}

		bool has( const hash_t & p_hashedKey ) const { return _metricsMap.contains( p_hashedKey ); }
		bool has( const key_t & p_key ) const { return _metricsMap.contains( hash( p_key ) ); }

		long long getTimestamp() const { return _timestamp; }

	  private:
		long long _timestamp;

		std::map<hash_t, Util::VTXVariant> _metricsMap = std::map<hash_t, Util::VTXVariant>();
	};
} // namespace VTX::App::Core::Monitoring
#endif
