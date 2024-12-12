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
				// Exit previous mode.
				_current->exit();
				onModeExit( _currentHash );
				APP::onUpdate -= _currentUpdateCallback;
			}

			// Enter new mode.
			M * mode	 = _modes.getOrCreate<M>();
			_currentHash = Util::hash<M>();

			_current = static_cast<BaseMode *>( mode );
			_current->enter();
			onModeEnter( _currentHash );

			// Connect update callback.
			_currentUpdateCallback = APP::onUpdate +=
				[ mode ]( const float p_delta, const float p_elapsed ) { mode->update( p_delta, p_elapsed ); };
		}

		Util::Callback<Hash> onModeEnter;
		Util::Callback<Hash> onModeExit;

	  private:
		Hash			 _currentHash;
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
