#ifndef __VTX_PYTHON_API_SELECTION_MOLECULE_INTERPRETOR__
#define __VTX_PYTHON_API_SELECTION_MOLECULE_INTERPRETOR__

#include <app/application/selection/selection.hpp>
#include <app/application/selection/system_data.hpp>
#include <core/chemdb/atom.hpp>
#include <pybind11/pybind11.h>
#include <set>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::PythonBinding::API::Selection
{
	class SystemInterpretor
	{
	  private:
		struct InterpretedKwargs
		{
			InterpretedKwargs( const pybind11::kwargs & p_kwargs );

			bool isValid() const;

			bool hasSpecifySystem() const;
			bool hasSpecifyChain() const;
			bool hasSpecifyResidue() const;
			bool hasSpecifyAtom() const;

			std::vector<std::string> systemNames;
			std::vector<size_t>		 systemIndexes;

			std::vector<std::string> chainNames;
			std::vector<size_t>		 chainIndexes;

			std::vector<std::string> residueNames;
			std::vector<size_t>		 residueIndexes;

			std::vector<std::string>					 atomNames;
			std::vector<VTX::Core::ChemDB::Atom::SYMBOL> atomSymbols;
			std::vector<atom_index_t>					 atomIndexes;

		  private:
			bool _hasSystemParams;
			bool _hasChainParams;
			bool _hasResidueParams;
			bool _hasAtomParams;

			std::vector<VTX::Core::ChemDB::Atom::SYMBOL> _interpretAtomSymbols( const pybind11::kwargs & p_kwargs );
		};

	  public:
		static void interpretSystems( App::Application::Selection::Selection &, const pybind11::kwargs & );

	  private:
		static std::set<App::Component::Chemistry::System *> _getSystems( const InterpretedKwargs & p_kwargs );

		static void _selectChains(
			const InterpretedKwargs &				  p_kwargs,
			App::Application::Selection::SystemData & p_systemSelectionData
		);

		static void _selectResidues(
			const InterpretedKwargs &				  p_kwargs,
			App::Application::Selection::SystemData & p_systemSelectionData
		);

		static void _selectAtoms(
			const InterpretedKwargs &				  p_kwargs,
			App::Application::Selection::SystemData & p_systemSelectionData
		);

		static void _addAtomsFollowingKwargs(
			const atom_index_t						  p_firstAtom,
			const atom_index_t						  p_lastAtom,
			App::Component::Chemistry::System &		  p_system,
			App::Application::Selection::SystemData & p_systemSelectionData,
			const InterpretedKwargs &				  p_kwargs
		);
	};

} // namespace VTX::PythonBinding::API::Selection
#endif
