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

		template<int S, typename T>
		Util::Callback<T> & getCallback()
		{
			if ( not _callbacks.has( S ) )
			{
				_callbacks.createWithHash<Util::Callback<T>>( S );
			}

			return *_callbacks.get<Util::Callback<T>>( S );
		}

	  private:
		Util::Collection<std::unique_ptr<Util::ICallback>> _callbacks;
	};

	template<typename T>
	class Preset : public IPreset
	{
	  public:
		virtual ~Preset() = default;

		inline const T & getData() const { return _data; }
		inline T &		 getData() { return _data; }

		virtual void save() = 0;
		virtual void load() = 0;

	  protected:
		T _data;
	};

	template<typename P>
	concept ConceptPreset = std::is_base_of_v<IPreset, P>;

} // namespace VTX::App::Core::Library

#endif
