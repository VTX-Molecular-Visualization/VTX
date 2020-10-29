#ifndef __VTX_MODEL_SELECTION__
#define __VTX_MODEL_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Selection : public BaseModel
		{
			// Map molecule model id with index (not model id) of chains, residues and atoms.
			using MapIds = std::map<ulong, std::map<uint, std::map<uint, std::vector<uint>>>>;

		  public:
			Selection() : BaseModel( ID::Model::MODEL_SELECTION ) {}
			~Selection() = default;

			void selectMolecule( Model::Molecule * const );
			void unselectMolecule( Model::Molecule * const );
			void selectChain( Model::Chain * const );
			void unselectChain( Model::Chain * const );
			void selectResidue( Model::Residue * const );
			void unselectResidue( Model::Residue * const );
			void selectAtom( Model::Atom * const );
			void unselectAtom( Model::Atom * const );

			void clear();

		  private:
			MapIds _items = MapIds();
		};

	} // namespace Model
} // namespace VTX
#endif
