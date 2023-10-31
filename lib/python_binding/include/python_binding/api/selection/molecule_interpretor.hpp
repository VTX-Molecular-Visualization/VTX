#ifndef __VTX_PYTHON_API_SELECTION_MOLECULE_INTERPRETOR__
#define __VTX_PYTHON_API_SELECTION_MOLECULE_INTERPRETOR__

#include <app/application/selection/molecule_data.hpp>
#include <app/application/selection/selection.hpp>
#include <core/chemdb/atom.hpp>
#include <pybind11/pybind11.h>
#include <set>
#include <string>
#include <vector>

namespace VTX::PythonBinding::API::Selection
{
	class MoleculeInterpretor
	{
	  private:
		struct InterpretedKwargs
		{
			InterpretedKwargs( const pybind11::kwargs & p_kwargs );

			bool isValid() const;

			bool hasSpecifyMolecule() const;
			bool hasSpecifyChain() const;
			bool hasSpecifyResidue() const;
			bool hasSpecifyAtom() const;

			std::vector<std::string> moleculeNames;
			std::vector<size_t>		 moleculeIndexes;

			std::vector<std::string> chainNames;
			std::vector<size_t>		 chainIndexes;

			std::vector<std::string> residueNames;
			std::vector<size_t>		 residueIndexes;

			std::vector<std::string>					 atomNames;
			std::vector<VTX::Core::ChemDB::Atom::SYMBOL> atomSymbols;
			std::vector<size_t>							 atomIndexes;

		  private:
			bool _hasMoleculeParams;
			bool _hasChainParams;
			bool _hasResidueParams;
			bool _hasAtomParams;

			std::vector<VTX::Core::ChemDB::Atom::SYMBOL> _interpretAtomSymbols( const pybind11::kwargs & p_kwargs );
		};

	  public:
		static void interpretMolecules( App::Application::Selection::Selection &, const pybind11::kwargs & );

	  private:
		static std::set<App::Component::Chemistry::Molecule *> _getMolecules( const InterpretedKwargs & p_kwargs );

		static void _selectChains(
			const InterpretedKwargs &					p_kwargs,
			App::Application::Selection::MoleculeData & p_moleculeSelectionData
		);

		static void _selectResidues(
			const InterpretedKwargs &					p_kwargs,
			App::Application::Selection::MoleculeData & p_moleculeSelectionData
		);

		static void _selectAtoms(
			const InterpretedKwargs &					p_kwargs,
			App::Application::Selection::MoleculeData & p_moleculeSelectionData
		);

		static void _addAtomsFollowingKwargs(
			const size_t								p_firstAtom,
			const size_t								p_lastAtom,
			App::Component::Chemistry::Molecule &		p_molecule,
			App::Application::Selection::MoleculeData & p_moleculeSelectionData,
			const InterpretedKwargs &					p_kwargs
		);
	};

} // namespace VTX::PythonBinding::API::Selection
#endif
