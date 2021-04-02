#ifndef __VTX_MODEL_RENDER_EFFECT_PRESET_LIBRARY__
#define __VTX_MODEL_RENDER_EFFECT_PRESET_LIBRARY__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/base_model.hpp"
#include "mvc/mvc_manager.hpp"
#include "render_effect_preset.hpp"
#include <vector>

namespace VTX::Model::Renderer
{
	class RenderEffectPresetLibrary : public BaseModel
	{
		VTX_MODEL

	  public:
		static RenderEffectPresetLibrary & get();

		RenderEffectPreset * const		 getPreset( const int p_index );
		const RenderEffectPreset * const getPreset( const int p_index ) const;
		RenderEffectPreset * const		 getPresetByName( const std::string & p_name );
		const RenderEffectPreset * const getPresetByName( const std::string & p_name ) const;

		inline const std::vector<RenderEffectPreset *> & getPresets() const { return _presets; };
		inline int										 getPresetCount() const { return (int)_presets.size(); };

		void addPreset( RenderEffectPreset * const p_representation );
		void removePreset( const int p_index );

	  private:
		inline RenderEffectPresetLibrary() : BaseModel( ID::Model::MODEL_RENDERER_RENDER_EFFECT_PRESET_LIBRARY )
		{
			_init();
		};
		~RenderEffectPresetLibrary();

		void _init();

		std::vector<RenderEffectPreset *> _presets;
	};
} // namespace VTX::Model::Renderer
#endif
