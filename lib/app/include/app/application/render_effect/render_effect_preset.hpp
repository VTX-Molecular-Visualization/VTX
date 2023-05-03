#ifndef __VTX_APP_APPLICATION_RENDER_EFFECT_RENDER_EFFECT_PRESET__
#define __VTX_APP_APPLICATION_RENDER_EFFECT_RENDER_EFFECT_PRESET__

#include "app/core/model/base_model.hpp"
#include "app/old_app/id.hpp"
#include "app/render/renderer/enum_renderer.hpp"
#include <string>
#include <util/color/rgba.hpp>
#include <util/math.hpp>

namespace VTX::App::Application::RenderEffect
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

		const Render::Renderer::SHADING & getShading() const { return _shading; }
		void							  setShading( Render::Renderer::SHADING p_shading ) { _shading = p_shading; }

		bool isSSAOEnabled() const { return _ssao; };
		void enableSSAO( const bool p_enable );
		int	 getSSAOIntensity() const { return _ssaoIntensity; };
		void setSSAOIntensity( const int p_ssaoIntensity );
		int	 getSSAOBlurSize() const { return _ssaoBlurSize; };
		void setSSAOBlurSize( const int p_ssaoBlurSize );

		bool					  isOutlineEnabled() const { return _outline; };
		void					  enableOutline( const bool p_enable );
		uint					  getOutlineThickness() const { return _outlineThickness; };
		void					  setOutlineThickness( const uint p_outlineThickness );
		float					  getOutlineSensivity() const { return _outlineSensivity; };
		void					  setOutlineSensivity( const float p_outlineSensivity );
		const Util::Color::Rgba & getOutlineColor() const { return _outlineColor; };
		void					  setOutlineColor( const Util::Color::Rgba & p_outlineColor );

		bool			  isFogEnabled() const { return _fog; };
		void			  enableFog( const bool p_enable );
		float			  getFogNear() const { return _fogNear; };
		void			  setFogNear( const float p_fogNear );
		float			  getFogFar() const { return _fogFar; };
		void			  setFogFar( const float p_fogFar );
		float			  getFogDensity() const { return _fogDensity; };
		void			  setFogDensity( const float p_fogDensity );
		Util::Color::Rgba getFogColor() const { return _fogColor; };
		void			  setFogColor( const Util::Color::Rgba & p_fogColor );

		Util::Color::Rgba getBackgroundColor() const { return _backgroundColor; };
		void			  setBackgroundColor( const Util::Color::Rgba & p_backgroundColor );
		Util::Color::Rgba getCameraLightColor() const { return _cameraLightColor; };
		void			  setCameraLightColor( const Util::Color::Rgba & p_cameraLightColor );

		void copyFrom( const RenderEffectPreset & p_source );

	  protected:
		RenderEffectPreset();
		~RenderEffectPreset() = default;

	  private:
		std::string _name;
		std::string _iconPath;
		bool		_quickAccess = false;

		Render::Renderer::SHADING _shading;

		bool _ssao;
		int	 _ssaoIntensity;
		int	 _ssaoBlurSize;

		bool			  _outline;
		uint			  _outlineThickness;
		float			  _outlineSensivity;
		Util::Color::Rgba _outlineColor;

		bool			  _fog;
		float			  _fogNear;
		float			  _fogFar;
		float			  _fogDensity;
		Util::Color::Rgba _fogColor;

		Util::Color::Rgba _backgroundColor;
		Util::Color::Rgba _cameraLightColor;
	};

} // namespace VTX::App::Application::RenderEffect
#endif