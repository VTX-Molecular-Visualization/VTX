#ifndef __VTX_APP_LIBRARY_PRESET__
#define __VTX_APP_LIBRARY_PRESET__

#include <util/callback.hpp>
#include <util/collection.hpp>

namespace VTX::App::Library
{
	class IPreset
	{
	  public:
		virtual ~IPreset() = default;
	};

	template<typename P>
	concept ConceptPreset = std::is_base_of_v<IPreset, P>;

	template<typename T>
	class BasePreset : public IPreset
	{
	  public:
		BasePreset() = default;
		BasePreset( const T & p_data ) : _data( p_data ) {}
		virtual ~BasePreset() = default;

		inline const T & getData() const { return _data; }
		inline T &		 getData() { return _data; }
		inline void		 setData( const T & p_data ) { _data = p_data; }

		virtual void save() = 0;
		virtual void load() = 0;

	  protected:
		T _data;
	};

} // namespace VTX::App::Library

#endif
