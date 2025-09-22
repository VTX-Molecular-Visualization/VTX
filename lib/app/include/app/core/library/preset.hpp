#ifndef __VTX_APP_CORE_LIBRARY_PRESET__
#define __VTX_APP_CORE_LIBRARY_PRESET__

#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Library
{
	class IPreset
	{
	  public:
		IPreset()		   = default;
		virtual ~IPreset() = default;
		IPreset( const IPreset & ) {}

		IPreset & operator=( const IPreset & ) { return *this; }

		template<int S, typename... Args>
		Util::Callback<Args...> & getCallback()
		{
			return *_callbacks.getOrCreateWithHash<Util::Callback<Args...>>( S );
		}

	  private:
		Util::Collection<std::unique_ptr<Util::ICallback>> _callbacks;
	};

	template<typename T>
	class Preset : public IPreset
	{
	  public:
		Preset() = default;
		Preset( const T & p_data ) : _data( p_data ) {}
		virtual ~Preset() = default;

		inline const T & getData() const { return _data; }
		inline T &		 getData() { return _data; }
		inline void		 setData( const T & p_data ) { _data = p_data; }

		virtual void save() = 0;
		virtual void load() = 0;

	  protected:
		T _data;
	};

	template<typename P>
	concept ConceptPreset = std::is_base_of_v<IPreset, P>;

} // namespace VTX::App::Core::Library

#endif
