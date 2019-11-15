#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#include "../model/model_molecule.hpp"
#include "object_3d.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
	{
		class Scene : public Object3D
		{
		  public:
			void addMolecule( const Model::ModelMolecule );

		  private:
			std::vector<Model::ModelMolecule> _molecules = std::vector<Model::ModelMolecule>();
		};
	} // namespace Object3D
} // namespace VTX
#endif
