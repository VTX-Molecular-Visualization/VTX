#ifndef __VTX_APP_RENDER_PROXY_BUILDER__
#define __VTX_APP_RENDER_PROXY_BUILDER__

#include "app/model/chemistry/molecule.hpp"
#include <renderer/gl/struct_proxy_molecule.hpp>

namespace VTX::App::Render
{
	class GPUProxyBuilder
	{
	  public:
		static VTX::Renderer::GL::StructProxyMolecule * setupProxy( Model::Chemistry::Molecule & p_molecule )
		{
			VTX::Renderer::GL::StructProxyMolecule * const proxy = new VTX::Renderer::GL::StructProxyMolecule();

			proxy->transform		= &( p_molecule._transform );
			proxy->atomPositions	= &( p_molecule._trajectory.frames[ p_molecule._trajectory.currentFrameIndex ] );
			proxy->atomColors		= &( p_molecule._atomColors );
			proxy->atomRadii		= &( p_molecule._atomRadii );
			proxy->atomVisibilities = &( p_molecule._atomVisibilities );
			proxy->atomSelections	= &( p_molecule._atomSelections );
			proxy->atomIds			= &( p_molecule._atomIds );
			proxy->bonds			= &( p_molecule._bondAtomPairIds );

			return proxy;
		}
	};

} // namespace VTX::App::Render

#endif
