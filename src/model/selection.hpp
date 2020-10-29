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
			using MapIds = std::map<Model::Model_ID, std::map<uint, std::map<uint, std::vector<uint>>>>;

		  public:
			Selection() : BaseModel( ID::Model::MODEL_SELECTION ) {}
			~Selection() = default;

			void selectMolecule( const Model::Molecule & );
			void unselectMolecule( const Model::Molecule & );
			void selectChain( const Model::Chain & );
			void unselectChain( const Model::Chain & );
			void selectResidue( const Model::Residue & );
			void unselectResidue( const Model::Residue & );
			void selectAtom( const Model::Atom & );
			void unselectAtom( const Model::Atom & );

			void clear();

		  private:
			MapIds _items = MapIds();
		};

	} // namespace Model
} // namespace VTX
#endif
