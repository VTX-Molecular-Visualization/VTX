#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#include "base_model.hpp"
#include "model_atom.hpp"
#include <iostream>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class ModelMolecule : BaseModel
		{
		  public:
			std::string getName() const { return _name; };
			void setName( const std::string & p_name ) { _name = p_name; };

		  private:
			std::string			   _name  = "";
			std::vector<ModelAtom> _atoms = std::vector<ModelAtom>();
		};
	} // namespace Model
} // namespace VTX
#endif
