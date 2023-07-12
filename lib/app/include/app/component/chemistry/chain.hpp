#ifndef __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__
#define __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__

#include "_fwd.hpp"
#include <string>
#include <util/color/rgba.hpp>
#include <util/constants.hpp>

namespace VTX::App::Component::Chemistry
{
	class Chain
	{
	  public:
		Chain();
		Chain( Molecule * const p_molecule, const size_t p_index );

		const size_t getIndex() const { return _index; }
		void		 setIndex( const size_t p_index ) { _index = p_index; }

		Molecule * const	   getMoleculePtr() const { return _moleculePtr; };
		const Molecule * const getConstMoleculePtr() const { return _moleculePtr; };
		void				   setMoleculePtr( Molecule * const p_moleculePtr ) { _moleculePtr = p_moleculePtr; };

		const std::string & getName() const;
		void				setName( const std::string & p_name );

		const size_t getIndexFirstResidue() const;
		void		 setIndexFirstResidue( const size_t p_residueIndex );

		const size_t getResidueCount() const;
		void		 setResidueCount( const size_t p_residueCount );

		const std::string & getOriginalChainID() const { return _originalChainID; }
		void				setOriginalChainID( const std::string & p_chainId ) { _originalChainID = p_chainId; }

		const Util::Color::Rgba & getColor() const { return _defaultColor; };
		void setColor( const Util::Color::Rgba & p_defaultColor ) { _defaultColor = p_defaultColor; };

	  private:
		Molecule *		  _moleculePtr	   = nullptr;
		size_t			  _index		   = INVALID_INDEX;
		std::string		  _originalChainID = "";
		Util::Color::Rgba _defaultColor	   = COLOR_WHITE;
	};
} // namespace VTX::App::Component::Chemistry
#endif
