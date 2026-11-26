#ifndef VTX_APP_ACTION_MATERIAL_HPP
#define VTX_APP_ACTION_MATERIAL_HPP

#include "app/ecs.hpp"
#include "app/helper/preset.hpp"
#include <optional>
#include <renderer/material.hpp>
#include <util/filesystem.hpp>

namespace VTX::App::Action::Material
{
	namespace Detail
	{
		inline void setTexture( std::optional<Renderer::MaterialTexture> & p_texture, const FilePath & p_path )
		{
			p_texture
				= p_path.empty() ? std::nullopt : std::optional<Renderer::MaterialTexture> { { p_path.string(), 0 } };
		}
	} // namespace Detail

	/**
	 * @brief Change a material value.
	 */
	template<Renderer::E_MATERIAL_VALUES S, typename T>
	struct Change
	{
		void execute( const Entity p_e, const T & p_value )
		{
			Helper::Preset::checkEditable( p_e );
			REG().patch<Renderer::Material>(
				p_e,
				[ p_value ]( Renderer::Material & p_material )
				{
					if constexpr ( S == Renderer::E_MATERIAL_VALUES::METALLIC )
					{
						p_material.metallic = p_value;
					}
					else if constexpr ( S == Renderer::E_MATERIAL_VALUES::ROUGHNESS )
					{
						p_material.roughness = p_value;
					}
					else if constexpr ( S == Renderer::E_MATERIAL_VALUES::EMISSIVE_INTENSITY )
					{
						p_material.emissiveIntensity = p_value;
					}
					else if constexpr ( S == Renderer::E_MATERIAL_VALUES::TEXTURE_SCALE )
					{
						p_material.textureScale = p_value;
					}
					else if constexpr ( S == Renderer::E_MATERIAL_VALUES::ALBEDO_TEXTURE )
					{
						Detail::setTexture( p_material.albedoTexture, p_value );
					}
					else if constexpr ( S == Renderer::E_MATERIAL_VALUES::NORMAL_TEXTURE )
					{
						Detail::setTexture( p_material.normalTexture, p_value );
					}
					else if constexpr ( S == Renderer::E_MATERIAL_VALUES::METALLIC_TEXTURE )
					{
						Detail::setTexture( p_material.metallicTexture, p_value );
					}
					else if constexpr ( S == Renderer::E_MATERIAL_VALUES::ROUGHNESS_TEXTURE )
					{
						Detail::setTexture( p_material.roughnessTexture, p_value );
					}
					else if constexpr ( S == Renderer::E_MATERIAL_VALUES::AMBIENT_OCCLUSION_TEXTURE )
					{
						Detail::setTexture( p_material.ambientOcclusionTexture, p_value );
					}
					else if constexpr ( S == Renderer::E_MATERIAL_VALUES::EMISSIVE_TEXTURE )
					{
						Detail::setTexture( p_material.emissiveTexture, p_value );
					}

					p_material.clamp();
				}
			);
		}
	};
} // namespace VTX::App::Action::Material

#endif
