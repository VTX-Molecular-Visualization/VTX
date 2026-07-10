#ifndef __VTX_APP_PYTHON_BINDING_TOPOLOGY_TYPES__
#define __VTX_APP_PYTHON_BINDING_TOPOLOGY_TYPES__

#include "app/ecs.hpp"
#include <core/struct/topology.hpp>

namespace VTX::App::PythonBinding::Topology
{
	/**
	 * @brief Aliases form Core.
	 */
	using SystemItem = Core::Struct::E_SYSTEM_ITEM;
	using Range		 = Core::Struct::IndexRange;
	using RangeList	 = Core::Struct::IndexRangeList;

	/**
	 * @brief Forward.
	 */
	struct System;
	struct Chain;
	struct Residue;
	struct Atom;
	struct Bond;
	struct Category;

	/**
	 * @brief Describes a collection of topology items (entity + ranges).
	 */
	template<typename T>
	struct Collection
	{
		Entity	  entity = InvalidEntity;
		RangeList ranges;
	};

	/**
	 * @brief Aliases.
	 */
	using ChainCollection	 = Collection<Chain>;
	using ResidueCollection	 = Collection<Residue>;
	using AtomCollection	 = Collection<Atom>;
	using BondCollection	 = Collection<Bond>;
	using CategoryCollection = Collection<Category>;

	/**
	 * @brief System API.
	 */
	struct System
	{
		Entity entity = InvalidEntity;

		Chain	 getChain( const Index p_index ) const;
		Residue	 getResidue( const Index p_index ) const;
		Atom	 getAtom( const Index p_index ) const;
		Bond	 getBond( const Index p_index ) const;
		Category getCategory( const Index p_index ) const;
	};

	/**
	 * @brief Chain API.
	 */
	struct Chain
	{
		Entity entity = InvalidEntity;
		Index  index  = INVALID_INDEX;

		System			  getSystem() const;
		ResidueCollection getResidues() const;
		AtomCollection	  getAtoms() const;
		Residue			  getResidue( const Index p_index ) const;
		Atom			  getAtom( const Index p_index ) const;
	};

	/**
	 * @brief Residue API.
	 */
	struct Residue
	{
		Entity entity = InvalidEntity;
		Index  index  = INVALID_INDEX;

		System		   getSystem() const;
		Chain		   getChain() const;
		Category	   getCategory() const;
		AtomCollection getAtoms() const;
		Atom		   getAtom( const Index p_index ) const;
	};

	/**
	 * @brief Atom API.
	 */
	struct Atom
	{
		Entity entity = InvalidEntity;
		Index  index  = INVALID_INDEX;

		System	 getSystem() const;
		Residue	 getResidue() const;
		Chain	 getChain() const;
		Category getCategory() const;
	};

	/**
	 * @brief Bond API.
	 */
	struct Bond
	{
		Entity entity = InvalidEntity;
		Index  index  = INVALID_INDEX;

		System getSystem() const;
		Atom   getFirstAtom() const;
		Atom   getSecondAtom() const;
	};

	/**
	 * @brief Category API.
	 */
	struct Category
	{
		Entity entity = InvalidEntity;
		Index  index  = INVALID_INDEX;

		System			  getSystem() const;
		ResidueCollection getResidues() const;
		AtomCollection	  getAtoms() const;
	};

} // namespace VTX::App::PythonBinding::Topology

#endif
