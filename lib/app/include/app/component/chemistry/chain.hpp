#ifndef __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__
#define __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__

#include "_fwd.hpp"
#include "app/component/chemistry/index_types.hpp"
#include "app/component/chemistry/iterator/atom.hpp"
#include "app/component/chemistry/iterator/residue.hpp"
#include "app/core/ecs/base_component.hpp"
#include <string>
#include <util/color/rgba.hpp>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	class Chain : public Core::ECS::BaseComponent
	{
	  public:
		Chain();
		Chain( System * const p_molecule, const size_t p_index );

		const size_t getIndex() const { return _index; }
		void		 setIndex( const size_t p_index ) { _index = p_index; }

		System * const	   getMoleculePtr() const { return _moleculePtr; };
		const System * const getConstMoleculePtr() const { return _moleculePtr; };
		void				   setMoleculePtr( System * const p_moleculePtr ) { _moleculePtr = p_moleculePtr; };

		const std::string & getName() const;
		void				setName( const std::string & p_name );

		size_t getIndexFirstResidue() const;
		size_t getIndexLastResidue() const;
		void   setIndexFirstResidue( const size_t p_residueIndex );

		size_t getResidueCount() const;
		void   setResidueCount( const size_t p_residueCount );

		atom_index_t getIndexFirstAtom() const;
		atom_index_t getIndexLastAtom() const;

		const std::string & getOriginalChainID() const { return _originalChainID; }
		void				setOriginalChainID( const std::string & p_chainId ) { _originalChainID = p_chainId; }

		const Util::Color::Rgba & getColor() const { return _defaultColor; };
		void setColor( const Util::Color::Rgba & p_defaultColor ) { _defaultColor = p_defaultColor; };

		ResidueIndexRange getResidueRange() const;
		AtomIndexRange	  getAtomRange() const;

		Iterator::ResidueContainer residues() const;
		Iterator::AtomContainer	   atoms() const;

		bool isVisible() const;
		bool isFullyVisible() const;

		void setVisible( const bool p_visible );

		void remove();

	  private:
		System *		  _moleculePtr	   = nullptr;
		size_t			  _index		   = INVALID_INDEX;
		std::string		  _originalChainID = "";
		Util::Color::Rgba _defaultColor	   = COLOR_WHITE;
	};
} // namespace VTX::App::Component::Chemistry
#endif
