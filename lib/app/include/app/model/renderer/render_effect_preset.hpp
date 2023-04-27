#ifndef __VTX_MODEL_RENDERER_RENDER_EFFECT_PRESET__
#define __VTX_MODEL_RENDERER_RENDER_EFFECT_PRESET__

#include "app/old_app/color/rgba.hpp"
#include "app/old_app/id.hpp"
#include "app/core/model/base_model.hpp"
#include "app/old_app/renderer/base_renderer.hpp"
#include <util/math.hpp>
#include <string>

namespace VTX::Model::Renderer
{
	class RenderEffectPreset : public App::Core::Model::BaseModel
	{
		VTX_MODEL

	  public:
		const std::string & getName() const { return _name; };
		void				setName( const std::string & p_name );
		const std::string & getIconPath() const { return _iconPath; };
		void				setIconPath( const std::string & p_iconPath );
		bool				hasQuickAccess() const { return _quickAccess; }
		void				setQuickAccess( const bool p_quickAccess );

		const VTX::Renderer::SHADING & getShading() const { return _shading; }
		void						   setShading( VTX::Renderer::SHADING p_shading ) { _shading = p_shading; }

		bool isSSAOEnabled() const { return _ssao; };
		void enableSSAO( const bool p_enable );
		int	 getSSAOIntensity() const { return _ssaoIntensity; };
		void setSSAOIntensity( const int p_ssaoIntensity );
		int	 getSSAOBlurSize() const { return _ssaoBlurSize; };
		void setSSAOBlurSize( const int p_ssaoBlurSize );

		bool				isOutlineEnabled() const { return _outline; };
		void				enableOutline( const bool p_enable );
		uint				getOutlineThickness() const { return _outlineThickness; };
		void				setOutlineThickness( const uint p_outlineThickness );
		float				getOutlineSensivity() const { return _outlineSensivity; };
		void				setOutlineSensivity( const float p_outlineSensivity );
		const Color::Rgba & getOutlineColor() const { return _outlineColor; };
		void				setOutlineColor( const Color::Rgba & p_outlineColor );

		bool		isFogEnabled() const { return _fog; };
		void		enableFog( const bool p_enable );
		float		getFogNear() const { return _fogNear; };
		void		setFogNear( const float p_fogNear );
		float		getFogFar() const { return _fogFar; };
		void		setFogFar( const float p_fogFar );
		float		getFogDensity() const { return _fogDensity; };
		void		setFogDensity( const float p_fogDensity );
		Color::Rgba getFogColor() const { return _fogColor; };
		void		setFogColor( const Color::Rgba & p_fogColor );

		Color::Rgba getBackgroundColor() const { return _backgroundColor; };
		void		setBackgroundColor( const Color::Rgba & p_backgroundColor );
		Color::Rgba getCameraLightColor() const { return _cameraLightColor; };
		void		setCameraLightColor( const Color::Rgba & p_cameraLightColor );

		void copyFrom( const RenderEffectPreset & p_source );

	  protected:
		RenderEffectPreset();
		~RenderEffectPreset() = default;

	  private:
		std::string _name;
		std::string _iconPath;
		bool		_quickAccess = false;

		VTX::Renderer::SHADING _shading;

		bool _ssao;
		int	 _ssaoIntensity;
		int	 _ssaoBlurSize;

		bool		_outline;
		uint		_outlineThickness;
		float		_outlineSensivity;
		Color::Rgba _outlineColor;

		bool		_fog;
		float		_fogNear;
		float		_fogFar;
		float		_fogDensity;
		Color::Rgba _fogColor;

		Color::Rgba _backgroundColor;
		Color::Rgba _cameraLightColor;
	};

} // namespace VTX::Model::Renderer
#endif
