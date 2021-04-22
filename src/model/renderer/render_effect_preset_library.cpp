#include "render_effect_preset_library.hpp"
#include "event/event_manager.hpp"
#include "mvc/mvc_manager.hpp"
#include "renderer/base_renderer.hpp"
#include "setting.hpp"
#include "view/callback_view.hpp"
#include "vtx_app.hpp"

namespace VTX::Model::Renderer
{
	RenderEffectPresetLibrary & RenderEffectPresetLibrary::get() { return VTXApp::get().getRenderEffectLibrary(); }

	RenderEffectPresetLibrary::RenderEffectPresetLibrary() :
		BaseModel( ID::Model::MODEL_RENDERER_RENDER_EFFECT_PRESET_LIBRARY )
	{
		_init();
	};
	RenderEffectPresetLibrary ::~RenderEffectPresetLibrary()
	{
		while ( _presets.size() > 0 )
		{
			MVC::MvcManager::get().deleteModel<RenderEffectPreset>( _presets[ _presets.size() - 1 ] );
			_presets.pop_back();
		}
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
		for ( auto it : _presets )
		{
			if ( it->getName() == p_name )
				return it;
		}

		return nullptr;
	};

	void RenderEffectPresetLibrary::addPreset( RenderEffectPreset * const p_preset, const bool p_notify )
	{
		_presets.emplace_back( p_preset );

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

		copiedPreset->setName( "copy of " + sourcePreset->getName() );
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

	bool RenderEffectPresetLibrary::canDeleteItem( RenderEffectPreset * const p_preset ) const
	{
		return _presets.size() > 1;
	}

	void RenderEffectPresetLibrary::applyPreset( const int p_presetIndex )
	{
		applyPreset( *_presets[ p_presetIndex ] );
	}
	void RenderEffectPresetLibrary::applyPreset( const RenderEffectPreset & p_preset )
	{
		p_preset.apply();
		_appliedPreset = &p_preset;

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
	int RenderEffectPresetLibrary::getAppliedPresetIndex() const
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

	void RenderEffectPresetLibrary::_init()
	{
		// Preset 1
		RenderEffectPreset * const preset1 = MVC::MvcManager::get().instantiateModel<RenderEffectPreset>();
		preset1->setName( "Default" );
		setQuickAccessToPreset( *preset1, true );
		preset1->setShading( VTX::Renderer::SHADING::DIFFUSE );

		preset1->setSSAOIntensity( 5 );
		preset1->setSSAOBlurSize( 17 );
		preset1->enableSSAO( true );

		preset1->enableOutline( false );
		preset1->enableFog( false );
		addPreset( preset1, false );
		//

		// Preset 2
		RenderEffectPreset * const preset2 = MVC::MvcManager::get().instantiateModel<RenderEffectPreset>();
		preset2->setName( "Good Diffuse" );
		setQuickAccessToPreset( *preset2, true );
		preset2->setShading( VTX::Renderer::SHADING::DIFFUSE );

		preset2->setSSAOIntensity( 17 );
		preset2->setSSAOBlurSize( 80 );
		preset2->enableSSAO( true );

		preset2->enableOutline( false );
		preset2->enableFog( false );
		addPreset( preset2, false );
		//

		// Preset 3
		RenderEffectPreset * const preset3 = MVC::MvcManager::get().instantiateModel<RenderEffectPreset>();
		preset3->setName( "White BG" );
		setQuickAccessToPreset( *preset3, true );
		preset3->setShading( VTX::Renderer::SHADING::FLAT_COLOR );

		preset3->setSSAOIntensity( 5 );
		preset3->setSSAOBlurSize( 17 );
		preset3->enableSSAO( true );

		preset3->setOutlineThickness( 1.0f );
		preset3->setOutlineColor( Color::Rgb::BLACK );
		preset3->enableOutline( true );

		preset3->enableFog( false );

		preset3->setBackgroundColor( Color::Rgb::WHITE );
		addPreset( preset3, false );
		//
	}

	void RenderEffectPresetLibrary::_onPresetChange( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE )
			_notifyViews( new Event::VTXEvent( Event::Model::SUBITEM_DATA_CHANGE ) );
		else
			_notifyViews( new Event::VTXEvent( *p_event ) );
	}
} // namespace VTX::Model::Renderer
