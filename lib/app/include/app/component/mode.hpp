#ifndef __VTX_APP_COMPONENT_MODE__
#define __VTX_APP_COMPONENT_MODE__

#include "app/core/ecs/base_component.hpp"
#include "app/core/mode/concepts.hpp"
#include "app/vtx_app.hpp"
#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/hashing.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Component
{
	class Mode : public Core::ECS::BaseComponent
	{
	  public:
		Mode()				 = default;
		Mode( const Mode & ) = delete;

		template<Core::Mode::ConceptMode M>
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

			_current = static_cast<Core::Mode::BaseMode *>( mode );
			_current->enter();
			onModeEnter( _currentHash );

			// Connect update callback.
			_currentUpdateCallback = APP::onUpdate +=
				[ mode ]( const float p_delta, const float p_elapsed ) { mode->update( p_delta, p_elapsed ); };
		}

		Util::Callback<Hash> onModeEnter;
		Util::Callback<Hash> onModeExit;

	  private:
		Hash				   _currentHash;
		Core::Mode::BaseMode * _current;
		Util::CallbackId	   _currentUpdateCallback;

		Util::Collection<std::unique_ptr<Core::Mode::BaseMode>> _modes;
	};
} // namespace VTX::App::Component

#endif
