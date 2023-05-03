#include "app/application/render_effect/render_effect_library.hpp"
#include "app/action/renderer.hpp"
#include "app/application/setting.hpp"
#include "app/core/view/callback_view.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/internal/worker/render_effect_loader.hpp"
#include "app/manager/action_manager.hpp"
#include "app/mvc.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/render/renderer/base_renderer.hpp"
#include "app/render/renderer/gl/gl.hpp"
#include "app/worker.hpp"

namespace VTX::App::Application::RenderEffect
{
	RenderEffectLibrary & RenderEffectLibrary::get() { return VTXApp::get().getRenderEffectLibrary(); }

	RenderEffectLibrary::RenderEffectLibrary() :
		BaseModel( App::ID::Model::MODEL_RENDERER_RENDER_EFFECT_PRESET_LIBRARY )
	{
		Internal::Worker::RenderEffectPresetLibraryLoader * libraryLoader
			= new Internal::Worker::RenderEffectPresetLibraryLoader( *this );
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
	RenderEffectLibrary ::~RenderEffectLibrary()
	{
		App::Action::Renderer::SavePreset * const saveAction = new App::Action::Renderer::SavePreset( *this );
		saveAction->setAsync( false );

		VTX_ACTION( saveAction );
		clear( false );
	};

	RenderEffectPreset * const RenderEffectLibrary::getPreset( const int p_index )
	{
		if ( 0 <= p_index && p_index < _presets.size() )
			return _presets[ p_index ];

		return nullptr;
	};
	const RenderEffectPreset * const RenderEffectLibrary::getPreset( const int p_index ) const
	{
		if ( 0 <= p_index && p_index < _presets.size() )
			return _presets[ p_index ];

		return nullptr;
	};
	RenderEffectPreset * const RenderEffectLibrary::getPresetByName( const std::string & p_name )
	{
		for ( RenderEffectPreset * const it : _presets )
		{
			if ( it->getName() == p_name )
				return it;
		}

		return nullptr;
	};
	const RenderEffectPreset * const RenderEffectLibrary::getPresetByName( const std::string & p_name ) const
	{
		for ( RenderEffectPreset * const it : _presets )
		{
			if ( it->getName() == p_name )
				return it;
		}

		return nullptr;
	};

	void RenderEffectLibrary::addPreset( RenderEffectPreset * const p_preset,
										 const bool					p_emplace,
										 const bool					p_notify )
	{
		if ( p_emplace )
		{
			_presets.emplace_back( p_preset );
		}
		else
		{
			_presets.insert( _presets.begin(), p_preset );
		}

		App::Core::View::CallbackView<RenderEffectPreset, RenderEffectLibrary> * const callbackView
			= VTX::MVC_MANAGER()
				  .instantiateView<App::Core::View::CallbackView<RenderEffectPreset, RenderEffectLibrary>>(
					  p_preset, VTX::App::ID::View::RENDER_EFFECT_LIBRARY_ON_ITEMS );

		callbackView->setCallback( this, &RenderEffectLibrary::_onPresetChange );

		if ( p_notify )
			_notifyDataChanged();

		const int presetAddedIndex = int( _presets.size() - 1 );
		VTX_EVENT<int>( VTX::App::Event::Global::RENDER_EFFECT_ADDED, presetAddedIndex );
	};

	RenderEffectPreset * const RenderEffectLibrary::copyPreset( const int p_index )
	{
		const RenderEffectPreset * const sourcePreset = _presets[ p_index ];
		RenderEffectPreset * const		 copiedPreset = VTX::MVC_MANAGER().instantiateModel<RenderEffectPreset>();
		copiedPreset->copyFrom( *sourcePreset );

		copiedPreset->setName( getValidName( sourcePreset->getName() ) );
		copiedPreset->setQuickAccess( false );

		addPreset( copiedPreset );

		return copiedPreset;
	}

	RenderEffectPreset * const RenderEffectLibrary::removePreset( const int p_index )
	{
		RenderEffectPreset * removedPreset;

		if ( 0 <= p_index && p_index < _presets.size() )
		{
			VTX::MVC_MANAGER().deleteView( _presets[ p_index ], VTX::App::ID::View::RENDER_EFFECT_LIBRARY_ON_ITEMS );

			removedPreset = _presets[ p_index ];

			if ( removedPreset == _appliedPreset )
			{
				const int newAppliedPreset = p_index == ( _presets.size() - 1 ) ? p_index - 1 : p_index + 1;
				applyPreset( newAppliedPreset );
			}

			_presets.erase( _presets.begin() + p_index );

			VTX_SETTING().setDefaultRenderEffectPresetIndex( getPresetIndex( _appliedPreset ) );

			_notifyDataChanged();

			VTX_EVENT( VTX::App::Event::Global::SCENE_SAVED );

			VTX_EVENT<int>( VTX::App::Event::Global::RENDER_EFFECT_REMOVED, p_index );
		}
		else
		{
			removedPreset = nullptr;
		}

		return removedPreset;
	};
	void RenderEffectLibrary::deletePreset( const int p_index )
	{
		const RenderEffectPreset * const presetToDelete = removePreset( p_index );

		if ( presetToDelete != nullptr )
			VTX::MVC_MANAGER().deleteModel( presetToDelete );
	}

	int RenderEffectLibrary::getPresetIndex( const RenderEffectPreset * const p_preset ) const
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
	int RenderEffectLibrary::getPresetIndex( const std::string & p_presetName ) const
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

	bool RenderEffectLibrary::canDeleteItem( RenderEffectPreset * const p_preset ) const { return _presets.size() > 1; }

	void RenderEffectLibrary::setAppliedPreset( const int p_presetIndex )
	{
		if ( 0 <= p_presetIndex && p_presetIndex < _presets.size() )
			_appliedPreset = _presets[ p_presetIndex ];
		else
			_appliedPreset = _presets[ 0 ];
	}

	void RenderEffectLibrary::applyPreset( const int p_presetIndex )
	{
		if ( 0 <= p_presetIndex && p_presetIndex < _presets.size() )
			applyPreset( *_presets[ p_presetIndex ] );
		else
			applyPreset( *_presets[ 0 ] );
	}
	void RenderEffectLibrary::applyPreset( RenderEffectPreset & p_preset )
	{
		_appliedPreset = &p_preset;

		_notifyViews( App::Event::Model::APPLIED_PRESET_CHANGE );
		VTX_EVENT( VTX::App::Event::Global::APPLIED_RENDER_EFFECT_CHANGE );
	}
	bool RenderEffectLibrary::isAppliedPreset( const RenderEffectPreset & p_preset ) const
	{
		return &p_preset == _appliedPreset;
	}
	bool RenderEffectLibrary::isAppliedPreset( const RenderEffectPreset * const & p_preset ) const
	{
		return p_preset == _appliedPreset;
	}
	RenderEffectPreset &	   RenderEffectLibrary::getAppliedPreset() { return *_appliedPreset; }
	const RenderEffectPreset & RenderEffectLibrary::getAppliedPreset() const { return *_appliedPreset; }
	int						   RenderEffectLibrary::getAppliedPresetIndex() const
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

	void RenderEffectLibrary::setQuickAccessToPreset( RenderEffectPreset & p_preset, const bool p_quickAccess )
	{
		if ( p_quickAccess )
		{
			const int quickAccessCount = _getNbPresetWithQuickAccess();

			if ( quickAccessCount >= VTX::App::Application::Setting::MAX_QUICK_ACCESS_COUNT
				 && _lastPresetQuickAccessed != nullptr )
				_lastPresetQuickAccessed->setQuickAccess( false );
		}

		p_preset.setQuickAccess( p_quickAccess );

		if ( p_quickAccess )
			_lastPresetQuickAccessed = &p_preset;
	}

	int RenderEffectLibrary::_getNbPresetWithQuickAccess() const
	{
		int res = 0;
		for ( const RenderEffectPreset * preset : _presets )
		{
			if ( preset->hasQuickAccess() )
				res++;
		}
		return res;
	}

	void RenderEffectLibrary::clear( const bool p_notify )
	{
		if ( getAppliedPresetIndex() != -1 )
			_appliedPreset = nullptr;

		while ( _presets.size() > 0 )
		{
			VTX::MVC_MANAGER().deleteModel<RenderEffectPreset>( _presets.back() );
			_presets.pop_back();
		}

		if ( p_notify )
		{
			VTX_EVENT( VTX::App::Event::Global::RENDER_EFFECT_LIBRARY_CLEARED );
			_notifyDataChanged();
		}
	}

	void RenderEffectLibrary::resetToDefault()
	{
		clear( true );
		_generateDefaultLibrary( false );

		applyPreset( 0 );
		_notifyDataChanged();
	}

	std::string RenderEffectLibrary::getValidName( const std::string & p_name ) const
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
	bool RenderEffectLibrary::isValidName( const std::string & p_name ) const
	{
		for ( const RenderEffectPreset * const preset : _presets )
			if ( preset->getName() == p_name )
				return false;

		return true;
	}

	void RenderEffectLibrary::_generateDefaultPreset()
	{
		// Preset default
		RenderEffectPreset * const defaultPreset = VTX::MVC_MANAGER().instantiateModel<RenderEffectPreset>();
		defaultPreset->setName( "Default" );
		setQuickAccessToPreset( *defaultPreset, true );
		addPreset( defaultPreset, false, false );
	}

	void RenderEffectLibrary::_generateDefaultLibrary( const bool p_notify )
	{
		_generateDefaultPreset();

		// Preset Sketch
		RenderEffectPreset * const presetSketch = VTX::MVC_MANAGER().instantiateModel<RenderEffectPreset>();
		presetSketch->setName( "Sketch" );
		presetSketch->setShading( Render::Renderer::SHADING::FLAT_COLOR );
		presetSketch->enableSSAO( true );
		presetSketch->enableOutline( true );
		presetSketch->setOutlineColor( Util::Color::Rgba::BLACK );
		presetSketch->enableFog( false );
		presetSketch->setBackgroundColor( Util::Color::Rgba::WHITE );
		setQuickAccessToPreset( *presetSketch, true );
		addPreset( presetSketch, true, false );

		if ( p_notify )
			_notifyDataChanged();
	}

	void RenderEffectLibrary::_onPresetChange( const App::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == App::Event::Model::DATA_CHANGE )
			_notifyViews( App::Event::Model::SUBITEM_DATA_CHANGE );
		else
			_notifyViews( new App::Core::Event::VTXEvent( *p_event ) );
	}
} // namespace VTX::App::Application::RenderEffect
