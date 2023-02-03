#ifndef __VTX_MODEL_RENDERER_RENDER_EFFECT_PRESET__
#define __VTX_MODEL_RENDERER_RENDER_EFFECT_PRESET__

#include "color/rgba.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "renderer/base_renderer.hpp"
#include "util/math.hpp"
#include <string>

namespace VTX::Model::Renderer
{
	class RenderEffectPreset : public BaseModel
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
		float		getCameraFOV() const { return _cameraFOV; };
		void		setCameraFOV( const float p_cameraFOV );
		float		getCameraNearClip() const { return _cameraNearClip; };
		void		setCameraNearClip( const float p_cameraNearClip );
		float		getCameraFarClip() const { return _cameraFarClip; };
		void		setCameraFarClip( const float p_cameraFarClip );
		bool		getAA() const { return _antiAliasing; };
		void		setAA( const bool p_antiAliasing );
		bool		isPerspectiveProjection() const { return _perspectiveProjection; };
		void		setPerspectiveProjection( const bool p_perspectiveProjection );

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
		float		_cameraFOV;
		float		_cameraNearClip;
		float		_cameraFarClip;
		bool		_antiAliasing;
		bool		_perspectiveProjection;
	};

} // namespace VTX::Model::Renderer
#endif
