#ifndef __VTX_RENDERER_LAYOUT_VOXELS__
#define __VTX_RENDERER_LAYOUT_VOXELS__

#include "base_layout.hpp"
#include <string_view>

namespace VTX::Renderer::Layout
{
	enum struct VOXEL_ATTR : uint8_t
	{
		MINS,
		MAXS
	};

	class Voxels : public BaseLayout
	{
	  public:
		Voxels()
		{
			attributes.push_back( { Desc::Key { VOXELS_MINS }, Desc::E_TYPE::VEC3F } );
			attributes.push_back( { Desc::Key { VOXELS_MAXS }, Desc::E_TYPE::VEC3F } );
		}

		void resizeStorage( Context::ContextWrapper & p_context, const Index p_size )
		{ _resize( p_context, p_size == 0 ? 1 : p_size ); }

		template<VOXEL_ATTR A, typename T>
		void upload( Context::ContextWrapper & p_context, const Desc::Handle, std::span<const T> p_data )
		{
			if constexpr ( A == VOXEL_ATTR::MINS )
			{
				p_context.setBuffer<Vec3f>( { Desc::Key { VOXELS_MINS } }, p_data );
			}
			else if constexpr ( A == VOXEL_ATTR::MAXS )
			{
				p_context.setBuffer<Vec3f>( { Desc::Key { VOXELS_MAXS } }, p_data );
			}
			else
			{
				static_assert( always_false_v<A>, "Invalid voxel attribute." );
			}
		}

	  protected:
		void _resize( Context::ContextWrapper & p_context, const Index p_size ) override
		{
			p_context.setBuffer<Vec3f>( { Desc::Key { VOXELS_MINS } }, p_size );
			p_context.setBuffer<Vec3f>( { Desc::Key { VOXELS_MAXS } }, p_size );
		}

	  private:
		static constexpr std::string_view VOXELS_MINS = "Voxels.Mins";
		static constexpr std::string_view VOXELS_MAXS = "Voxels.Maxs";
	};
} // namespace VTX::Renderer::Layout

#endif
