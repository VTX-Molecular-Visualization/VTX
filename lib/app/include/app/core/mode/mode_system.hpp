#ifndef __VTX_APP_CORE_MODE_SYSTEM__
#define __VTX_APP_CORE_MODE_SYSTEM__

#include "app/vtx_app.hpp"
#include "concepts.hpp"
#include <util/collection.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Mode
{

	class ModeSystem
	{
	  public:
		template<ConceptMode M>
		inline void setMode()
		{
			if ( _current )
			{
				_current->exit();
				onModeExit( _current->getName() );
				APP::onUpdate -= _currentUpdateCallback;
			}

			M * mode = _modes.getOrCreate<M>();

			_current = static_cast<BaseMode *>( mode );
			_current->enter();
			onModeEnter( _current->getName() );

			// TODO: lambda or std::bind?
			_currentUpdateCallback = APP::onUpdate +=
				[ mode ]( const float p_delta, const float p_elapsed ) { mode->update( p_delta, p_elapsed ); };
			//_currentUpdateCallback = APP::onUpdate
			//	+= std::bind( &M::update, mode, std::placeholders::_1, std::placeholders::_2 );
		}

		Util::Callback<Name> onModeEnter;
		Util::Callback<Name> onModeExit;

	  private:
		BaseMode *		 _current;
		Util::CallbackId _currentUpdateCallback;

		Util::Collection<std::unique_ptr<BaseMode>> _modes;
	};

} // namespace VTX::App::Core::Mode

namespace VTX::App
{
	inline Core::Mode::ModeSystem & MODE_SYSTEM() { return Util::Singleton<Core::Mode::ModeSystem>::get(); }
} // namespace VTX::App

#endif
