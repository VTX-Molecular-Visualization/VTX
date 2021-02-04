#include "render_effect_preset_library.hpp"
#include "renderer/base_renderer.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX::Model::Renderer
{
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

	void RenderEffectPresetLibrary::addPreset( RenderEffectPreset * const p_representation )
	{
		_presets.emplace_back( p_representation );
	};
	void RenderEffectPresetLibrary::removePreset( const int p_index )
	{
		if ( 0 <= p_index && p_index < _presets.size() )
			_presets.erase( _presets.begin() + p_index );
	};

	void RenderEffectPresetLibrary::_init()
	{
		// Preset 1
		RenderEffectPreset * const preset1 = MVC::MvcManager::get().instantiateModel<RenderEffectPreset>();
		preset1->setName( "Default" );
		preset1->setShading( VTX::Renderer::SHADING::DIFFUSE );

		preset1->setSSAOIntensity( 5 );
		preset1->setSSAOBlurSize( 17 );
		preset1->enableSSAO( true );

		preset1->enableOutline( false );
		preset1->enableFog( false );
		addPreset( preset1 );
		//

		// Preset 2
		RenderEffectPreset * const preset2 = MVC::MvcManager::get().instantiateModel<RenderEffectPreset>();
		preset2->setName( "Good diffuse" );
		preset2->setShading( VTX::Renderer::SHADING::DIFFUSE );

		preset2->setSSAOIntensity( 17 );
		preset2->setSSAOBlurSize( 80 );
		preset2->enableSSAO( true );

		preset2->enableOutline( false );
		preset2->enableFog( false );
		addPreset( preset2 );
		//

		// Preset 3
		RenderEffectPreset * const preset3 = MVC::MvcManager::get().instantiateModel<RenderEffectPreset>();
		preset3->setName( "Gloubiboulga" );
		preset3->setShading( VTX::Renderer::SHADING::FLAT_COLOR );

		preset3->setSSAOIntensity( 5 );
		preset3->setSSAOBlurSize( 17 );
		preset3->enableSSAO( true );

		preset3->setOutlineThickness( 1.0f );
		preset3->setOutlineColor( Color::Rgb( 200, 70, 140 ) );
		preset3->enableOutline( true );

		preset3->setFogNear( 50.0f );
		preset3->setFogFar( 200.0f );
		preset3->setFogDensity( 0.8f );
		preset3->setFogColor( Color::Rgb( 50, 50, 50 ) );
		preset3->enableFog( false );
		addPreset( preset3 );
		//
	}
} // namespace VTX::Model::Renderer
