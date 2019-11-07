#ifndef __VTX_MODEL_MOLECULE__
#define __VTX_MODEL_MOLECULE__

#include "base_model.hpp"
#include <iostream>
#include <vector>

namespace VTX
{
	namespace Model
	{
		enum ATOM_TYPE
		{
			H,
			C,
			N,
			O,
			NA,
			MG,
			P,
			S,
			CA,
			INVALID_TYPE,
			ATOM_TYPE_COUNT
		};

		struct Atom
		{
		};

		struct Chain
		{
			std::string name = "";
		};

		struct Residue
		{
		};

		class ModelMolecule : BaseModel
		{
		  public:
			std::string getName() const { return _name; };
			void setName( const std::string & p_name ) { _name = p_name; };

		  private:
			std::string		  _name	 = "";
			std::vector<Atom> _atoms = std::vector<Atom>();
		};
	} // namespace Model
} // namespace VTX
#endif
