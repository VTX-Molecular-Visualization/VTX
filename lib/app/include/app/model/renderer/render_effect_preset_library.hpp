#ifndef __VTX_MODEL_RENDER_EFFECT_PRESET_LIBRARY__
#define __VTX_MODEL_RENDER_EFFECT_PRESET_LIBRARY__

#include "app/core/event/vtx_event.hpp"
#include "app/core/mvc/mvc_manager.hpp"
#include "app/model/base_model.hpp"
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
		int								 getPresetIndex( const std::string & ) const;

		inline const std::vector<RenderEffectPreset *> & getPresets() const { return _presets; };
		inline int										 getPresetCount() const { return (int)_presets.size(); };

		void addPreset( RenderEffectPreset * const p_preset, const bool p_emplace = true, const bool p_notify = true );
		RenderEffectPreset * const copyPreset( const int p_index );
		RenderEffectPreset * const removePreset( const int p_index );
		void					   deletePreset( const int p_index );

		bool canDeleteItem( RenderEffectPreset * const p_preset ) const;

		void					   setAppliedPreset( const int p_presetIndex );
		void					   applyPreset( const int p_presetIndex );
		void					   applyPreset( RenderEffectPreset & p_preset );
		bool					   isAppliedPreset( const RenderEffectPreset & p_preset ) const;
		bool					   isAppliedPreset( const RenderEffectPreset * const & p_preset ) const;
		const RenderEffectPreset & getAppliedPreset() const;
		RenderEffectPreset &	   getAppliedPreset();
		int						   getAppliedPresetIndex() const;

		std::string getValidName( const std::string & p_name ) const;
		bool		isValidName( const std::string & p_name ) const;

		void setQuickAccessToPreset( RenderEffectPreset & p_preset, const bool p_quikAccess );

		void clear( const bool p_notify = true );
		void resetToDefault();

	  private:
		RenderEffectPresetLibrary();
		~RenderEffectPresetLibrary();

		void _generateDefaultPreset();
		void _generateDefaultLibrary( const bool p_notify = true );
		void _onPresetChange( const App::Core::Event::VTXEvent * const p_event );
		int	 _getNbPresetWithQuickAccess() const;

		std::vector<RenderEffectPreset *> _presets;
		RenderEffectPreset *			  _appliedPreset		   = nullptr;
		RenderEffectPreset *			  _lastPresetQuickAccessed = nullptr;
	};
} // namespace VTX::Model::Renderer
#endif
