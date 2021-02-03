#ifndef __VTX_MODEL_RENDERER_RENDER_EFFECT_PRESET__
#define __VTX_MODEL_RENDERER_RENDER_EFFECT_PRESET__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "renderer/base_renderer.hpp"
#include "setting.hpp"
#include "util/math.hpp"
#include <string>

namespace VTX::Model::Renderer
{
	class RenderEffectPreset : public BaseModel
	{
		VTX_MODEL

	  public:
		const std::string & getName() const { return _name; };
		void				setName( const std::string & p_name ) { _name = std::string( p_name ); };

		const std::string & getIconPath() const { return _iconPath; };
		void				setIconPath( const std::string & p_iconPath ) { _iconPath = std::string( p_iconPath ); };

		const VTX::Renderer::SHADING & getShading() const { return _shading; }
		void						   setShading( VTX::Renderer::SHADING p_shading ) { _shading = p_shading; }

		bool isSSAOEnabled() const { return _ssao; };
		void enableSSAO( const bool p_enable );
		int	 getSSAOIntensity() const { return _ssaoIntensity; };
		void setSSAOIntensity( const int p_ssaoIntensity );
		int	 getSSAOBlurSize() const { return _ssaoBlurSize; };
		void setSSAOBlurSize( const int p_ssaoBlurSize );

		bool			   isOutlineEnabled() const { return _outline; };
		void			   enableOutline( const bool p_enable );
		float			   getOutlineThickness() const { return _oulineThickness; };
		void			   setOutlineThickness( const float p_oulineThickness );
		const Color::Rgb & getOutlineColor() const { return _outlineColor; };
		void			   setOutlineColor( const Color::Rgb & p_outlineColor );

		bool	   isFogEnabled() const { return _fog; };
		void	   enableFog( const bool p_enable );
		float	   getFogNear() const { return _fogNear; };
		void	   setFogNear( const float p_fogNear );
		float	   getFogFar() const { return _fogFar; };
		void	   setFogFar( const float p_fogFar );
		float	   getFogDensity() const { return _fogDensity; };
		void	   setFogDensity( const float p_fogDensity );
		Color::Rgb getFogColor() const { return _fogColor; };
		void	   setFogColor( const Color::Rgb & p_fogColor );

		void apply() const;

	  protected:
		RenderEffectPreset() : BaseModel( ID::Model::MODEL_RENDERER_RENDER_EFFECT_PRESET ) {};
		~RenderEffectPreset() = default;

		void _notifyDataChanged();

	  private:
		std::string _name;
		std::string _iconPath;

		VTX::Renderer::SHADING _shading = Setting::SHADING_DEFAULT;

		bool _ssao			= Setting::ACTIVE_AO_DEFAULT;
		int	 _ssaoIntensity =  Setting::AO_INTENSITY_DEFAULT;
		int	 _ssaoBlurSize	=  Setting::AO_BLUR_SIZE_DEFAULT;

		bool	   _outline = Setting::ACTIVE_OUTLINE_DEFAULT;
		float	   _oulineThickness;
		Color::Rgb _outlineColor = Setting::OUTLINE_COLOR_DEFAULT;

		bool	   _fog		   = Setting::ACTIVE_FOG_DEFAULT;
		float	   _fogNear	   = Setting::FOG_NEAR_DEFAULT;
		float	   _fogFar	   = Setting::FOG_FAR_DEFAULT;
		float	   _fogDensity = Setting::FOG_DENSITY_DEFAULT;
		Color::Rgb _fogColor   = Setting::FOG_COLOR_DEFAULT;
	};

} // namespace VTX::Model::Renderer
#endif
