#ifndef __VTX_MODEL_RENDER_EFFECT_PRESET_LIBRARY__
#define __VTX_MODEL_RENDER_EFFECT_PRESET_LIBRARY__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
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
		int								 getPresetIndex( const RenderEffectPreset * const ) const;

		inline const std::vector<RenderEffectPreset *> & getPresets() const { return _presets; };
		inline int										 getPresetCount() const { return (int)_presets.size(); };

		void					   addPreset( RenderEffectPreset * const p_preset, const bool p_notify = true );
		void					   copyPreset( const int p_index );
		RenderEffectPreset * const removePreset( const int p_index );
		void					   deletePreset( const int p_index );

		bool canDeleteItem( RenderEffectPreset * const p_preset ) const;

		void					   applyPreset( const int p_presetIndex );
		void					   applyPreset( RenderEffectPreset & p_preset );
		bool					   isAppliedPreset( const RenderEffectPreset & p_preset ) const;
		bool					   isAppliedPreset( const RenderEffectPreset * const & p_preset ) const;
		const RenderEffectPreset & getAppliedPreset() const;
		RenderEffectPreset &	   getAppliedPreset();
		int						   getAppliedPresetIndex() const;

		void setQuickAccessToPreset( RenderEffectPreset & p_preset, const bool p_quikAccess );

		void clear( const bool p_notify = true );

	  private:
		RenderEffectPresetLibrary();
		~RenderEffectPresetLibrary();

		void _init();
		void _onPresetChange( const Event::VTXEvent * const p_event );
		int	 _getNbPresetWithQuickAccess() const;

		std::vector<RenderEffectPreset *> _presets;
		RenderEffectPreset *			  _appliedPreset		   = nullptr;
		RenderEffectPreset *			  _lastPresetQuickAccessed = nullptr;
	};
} // namespace VTX::Model::Renderer
#endif
