#ifndef __VTX_APP_RENDER_PROXY_BUILDER__
#define __VTX_APP_RENDER_PROXY_BUILDER__

#include "app/component/chemistry/molecule.hpp"
#include <renderer/gl/struct_proxy_molecule.hpp>

namespace VTX::App::Render
{
	class GPUProxyBuilder
	{
	  public:
		static void fillProxy( Component::Chemistry::Molecule &			p_molecule,
							   VTX::Renderer::GL::StructProxyMolecule & p_proxy )
		{
			p_proxy.transform		 = &( p_molecule.getTransform().get() );
			p_proxy.atomPositions	 = &( p_molecule.getTrajectory().getCurrentFrame() );
			p_proxy.atomColors		 = &( p_molecule._atomColors );
			p_proxy.atomRadii		 = &( p_molecule._atomRadii );
			p_proxy.atomVisibilities = &( p_molecule._atomVisibilities );
			p_proxy.atomSelections	 = &( p_molecule._atomSelections );
			p_proxy.atomIds			 = &( p_molecule._atomIds );
			// p_proxy.bonds			 = &( p_molecule._moleculeStruct.bondPairAtomIndexes );
		}
	};

} // namespace VTX::App::Render

#endif