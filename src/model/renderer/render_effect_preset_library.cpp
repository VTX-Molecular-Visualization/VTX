#include "render_effect_preset_library.hpp"
#include "action/action_manager.hpp"
#include "action/renderer.hpp"
#include "event/event_manager.hpp"
#include "mvc/mvc_manager.hpp"
#include "renderer/base_renderer.hpp"
#include "renderer/gl/gl.hpp"
#include "setting.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/render/opengl_widget.hpp"
#include "view/callback_view.hpp"
#include "vtx_app.hpp"
#include "worker/render_effect_loader.hpp"
#include "worker/worker_manager.hpp"

namespace VTX::Model::Renderer
{
	RenderEffectPresetLibrary & RenderEffectPresetLibrary::get() { return VTXApp::get().getRenderEffectLibrary(); }

	RenderEffectPresetLibrary::RenderEffectPresetLibrary() :
		BaseModel( ID::Model::MODEL_RENDERER_RENDER_EFFECT_PRESET_LIBRARY )
	{
		Worker::RenderEffectPresetLibraryLoader * libraryLoader = new Worker::RenderEffectPresetLibraryLoader( *this );
		libraryLoader->activeNotify( false );
		VTX_WORKER( libraryLoader );

		if ( getPresetByName( "Default" ) == nullptr )
		{
			_generateDefaultPreset();
		}

		const int defaultIndex = getPresetIndex( VTX_SETTING().getTmpRenderEffectPresetDefaultName() );
		VTX_SETTING().setDefaultRenderEffectPresetIndex( defaultIndex == -1 ? getPresetIndex( "Default" )
																			: defaultIndex );
		VTX_SETTING().setTmpRenderEffectPresetDefaultName( "" );
	};
	RenderEffectPresetLibrary ::~RenderEffectPresetLibrary()
	{
		Action::Renderer::SavePreset * const saveAction = new Action::Renderer::SavePreset( *this );
		saveAction->setAsync( false );

		VTX_ACTION( saveAction );
		clear( false );
	};

	RenderEffectPreset * const RenderEffectPresetLibrary::getPreset( const int p_index )
	{
		if ( 0 <= p_index && p_index < _presets.size() )
			return _presets[ p_index ];

		return nullptr;
	};
	const RenderEffectPreset * const RenderEffectPresetLibrary::getPreset( const int p_index ) const
	{
		if ( 0 <= p_index && p_index < _presets.size() )
			return _presets[ p_index ];

		return nullptr;
	};
	RenderEffectPreset * const RenderEffectPresetLibrary::getPresetByName( const std::string & p_name )
	{
		for ( RenderEffectPreset * const it : _presets )
		{
			if ( it->getName() == p_name )
				return it;
		}

		return nullptr;
	};
	const RenderEffectPreset * const RenderEffectPresetLibrary::getPresetByName( const std::string & p_name ) const
	{
		for ( RenderEffectPreset * const it : _presets )
		{
			if ( it->getName() == p_name )
				return it;
		}

		return nullptr;
	};

	void RenderEffectPresetLibrary::addPreset( RenderEffectPreset * const p_preset,
											   const bool				  p_emplace,
											   const bool				  p_notify )
	{
		if ( p_emplace )
		{
			_presets.emplace_back( p_preset );
		}
		else
		{
			_presets.insert( _presets.begin(), p_preset );
		}

		View::CallbackView<RenderEffectPreset, RenderEffectPresetLibrary> * const callbackView
			= MVC::MvcManager::get().instantiateView<View::CallbackView<RenderEffectPreset, RenderEffectPresetLibrary>>(
				p_preset, ID::View::RENDER_EFFECT_LIBRARY_ON_ITEMS );

		callbackView->setCallback( this, &RenderEffectPresetLibrary::_onPresetChange );

		if ( p_notify )
			_notifyDataChanged();

		const int presetAddedIndex = int( _presets.size() - 1 );
		VTX_EVENT( new Event::VTXEventValue( Event::Global::RENDER_EFFECT_ADDED, presetAddedIndex ) );
	};

	void RenderEffectPresetLibrary::copyPreset( const int p_index )
	{
		const RenderEffectPreset * const sourcePreset = _presets[ p_index ];
		RenderEffectPreset * const		 copiedPreset = MVC::MvcManager::get().instantiateModel<RenderEffectPreset>();
		copiedPreset->copyFrom( *sourcePreset );

		copiedPreset->setName( getValidName( sourcePreset->getName() ) );
		copiedPreset->setQuickAccess( false );

		addPreset( copiedPreset );
	}

	RenderEffectPreset * const RenderEffectPresetLibrary::removePreset( const int p_index )
	{
		RenderEffectPreset * removedPreset;

		if ( 0 <= p_index && p_index < _presets.size() )
		{
			MVC::MvcManager::get().deleteView( _presets[ p_index ], ID::View::RENDER_EFFECT_LIBRARY_ON_ITEMS );

			removedPreset = _presets[ p_index ];

			if ( removedPreset == _appliedPreset )
			{
				const int newAppliedPreset = p_index == ( _presets.size() - 1 ) ? p_index - 1 : p_index + 1;
				applyPreset( newAppliedPreset );
			}

			_presets.erase( _presets.begin() + p_index );

			VTX_SETTING().setDefaultRenderEffectPresetIndex( getPresetIndex( _appliedPreset ) );

			_notifyDataChanged();

			VTX_EVENT( new Event::VTXEventValue( Event::Global::RENDER_EFFECT_REMOVED, p_index ) );
		}
		else
		{
			removedPreset = nullptr;
		}

		return removedPreset;
	};
	void RenderEffectPresetLibrary::deletePreset( const int p_index )
	{
		const RenderEffectPreset * const presetToDelete = removePreset( p_index );

		if ( presetToDelete != nullptr )
			MVC::MvcManager::get().deleteModel( presetToDelete );
	}

	int RenderEffectPresetLibrary::getPresetIndex( const RenderEffectPreset * const p_preset ) const
	{
		for ( int i = 0; i < _presets.size(); i++ )
		{
			if ( _presets[ i ] == p_preset )
			{
				return i;
			}
		}

		return -1;
	}
	int RenderEffectPresetLibrary::getPresetIndex( const std::string & p_presetName ) const
	{
		for ( int i = 0; i < _presets.size(); i++ )
		{
			if ( _presets[ i ]->getName() == p_presetName )
			{
				return i;
			}
		}

		return -1;
	}

	bool RenderEffectPresetLibrary::canDeleteItem( RenderEffectPreset * const p_preset ) const
	{
		return _presets.size() > 1;
	}

	void RenderEffectPresetLibrary::setAppliedPreset( const int p_presetIndex )
	{
		if ( 0 <= p_presetIndex && p_presetIndex < _presets.size() )
			_appliedPreset = _presets[ p_presetIndex ];
		else
			_appliedPreset = _presets[ 0 ];
	}

	void RenderEffectPresetLibrary::applyPreset( const int p_presetIndex )
	{
		if ( 0 <= p_presetIndex && p_presetIndex < _presets.size() )
			applyPreset( *_presets[ p_presetIndex ] );
		else
			applyPreset( *_presets[ 0 ] );
	}
	void RenderEffectPresetLibrary::applyPreset( RenderEffectPreset & p_preset )
	{
		_appliedPreset = &p_preset;

		VTX::Renderer::GL::GL & gl = VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL();

		gl.setShading();
		gl.activeSSAO( _appliedPreset->isSSAOEnabled() );
		gl.activeOutline( _appliedPreset->isOutlineEnabled() );
		gl.activeFog( _appliedPreset->isFogEnabled() );
		gl.activeAA( _appliedPreset->getAA() );

		_notifyDataChanged();
	}
	bool RenderEffectPresetLibrary::isAppliedPreset( const RenderEffectPreset & p_preset ) const
	{
		return &p_preset == _appliedPreset;
	}
	bool RenderEffectPresetLibrary::isAppliedPreset( const RenderEffectPreset * const & p_preset ) const
	{
		return p_preset == _appliedPreset;
	}
	RenderEffectPreset &	   RenderEffectPresetLibrary::getAppliedPreset() { return *_appliedPreset; }
	const RenderEffectPreset & RenderEffectPresetLibrary::getAppliedPreset() const { return *_appliedPreset; }
	int						   RenderEffectPresetLibrary::getAppliedPresetIndex() const
	{
		for ( int i = 0; i < _presets.size(); i++ )
		{
			if ( _presets[ i ] == _appliedPreset )
			{
				return i;
			}
		}
		return -1;
	}

	void RenderEffectPresetLibrary::setQuickAccessToPreset( RenderEffectPreset & p_preset, const bool p_quickAccess )
	{
		if ( p_quickAccess )
		{
			const int quickAccessCount = _getNbPresetWithQuickAccess();

			if ( quickAccessCount >= Setting::MAX_QUICK_ACCESS_COUNT && _lastPresetQuickAccessed != nullptr )
				_lastPresetQuickAccessed->setQuickAccess( false );
		}

		p_preset.setQuickAccess( p_quickAccess );

		if ( p_quickAccess )
			_lastPresetQuickAccessed = &p_preset;
	}

	int RenderEffectPresetLibrary::_getNbPresetWithQuickAccess() const
	{
		int res = 0;
		for ( const RenderEffectPreset * preset : _presets )
		{
			if ( preset->hasQuickAccess() )
				res++;
		}
		return res;
	}

	void RenderEffectPresetLibrary::clear( const bool p_notify )
	{
		if ( getAppliedPresetIndex() != -1 )
			_appliedPreset = nullptr;

		while ( _presets.size() > 0 )
		{
			MVC::MvcManager::get().deleteModel<RenderEffectPreset>( _presets.back() );
			_presets.pop_back();
		}

		if ( p_notify )
			_notifyDataChanged();
	}
	std::string RenderEffectPresetLibrary::getValidName( const std::string & p_name ) const
	{
		const std::string & defaultStr = p_name;
		std::string			validName  = defaultStr;

		int counter = 2;
		while ( !isValidName( validName ) )
		{
			validName = defaultStr + " (" + std::to_string( counter ) + ")";
			counter++;
		}

		return validName;
	}
	bool RenderEffectPresetLibrary::isValidName( const std::string & p_name ) const
	{
		for ( const RenderEffectPreset * const preset : _presets )
			if ( preset->getName() == p_name )
				return false;

		return true;
	}

	void RenderEffectPresetLibrary::_generateDefaultPreset()
	{
		// Preset default
		RenderEffectPreset * const preset0 = MVC::MvcManager::get().instantiateModel<RenderEffectPreset>();
		preset0->setName( "Default" );
		setQuickAccessToPreset( *preset0, true );
		addPreset( preset0, false, false );
	}

	void RenderEffectPresetLibrary::_onPresetChange( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE )
			_notifyViews( new Event::VTXEvent( Event::Model::SUBITEM_DATA_CHANGE ) );
		else
			_notifyViews( new Event::VTXEvent( *p_event ) );
	}
} // namespace VTX::Model::Renderer
