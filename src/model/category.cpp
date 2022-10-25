#include "category.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "representation/representation_manager.hpp"

namespace VTX::Model
{
	void Category::Iterator::_next()
	{
		_currentIndex++;
		_currentResidue = _getResidueAtIndex();
	}
	Model::Residue * Category::Iterator::_getResidueAtIndex()
	{
		_currentIndex++;

		const std::vector<Struct::Range> & ranges = _linkedCategory.getRanges();

		uint seekIndex	  = _currentIndex;
		uint residueIndex = 0;

		for ( const Struct::Range & range : ranges )
		{
			if ( range.getCount() > seekIndex )
				seekIndex -= range.getCount();
			else
				residueIndex = range.getFirst() + seekIndex;
		}

		return _linkedCategory.getMolecule()->getResidue( residueIndex );
	}
	void Category::setMoleculePtr( Molecule * const p_molecule )
	{
		_moleculePtr = p_molecule;

		setParent( p_molecule );
		setRepresentableMolecule( p_molecule );
	}
	void Category::addResidueRange( const uint p_from, const uint p_count )
	{
		_ranges.emplace_back( Struct::Range( p_from, p_count ) );
	};
	const std::vector<Struct::Range> & Category::getRanges() const { return _ranges; };
	bool							   Category::isEmpty() const { return _ranges.size() == 0; };
	std::vector<Model::Chain *>		   Category::getChains() const
	{
		std::vector<Model::Chain *> res = std::vector<Model::Chain *>();
		res.reserve( _moleculePtr->getChainCount() );

		uint   iChain = 0;
		size_t iRange = 0;

		while ( iChain < _moleculePtr->getChainCount() && iRange < _ranges.size() )
		{
			Model::Chain * const  currentChain = _moleculePtr->getChain( iChain );
			const Struct::Range & currentRange = _ranges[ iRange ];

			if ( currentChain->getIndexLastResidue() < currentRange.getFirst() )
			{
				iChain++;
			}
			else if ( currentChain->getIndexFirstResidue() > currentRange.getLast() )
			{
				iRange++;
			}
			else
			{
				res.emplace_back( currentChain );
				iChain++;
			}
		}

		return res;
	}
	bool Category::contains( const uint p_residueIndex ) const
	{
		for ( const Struct::Range & range : _ranges )
		{
			if ( range.getFirst() <= p_residueIndex && p_residueIndex <= range.getLast() )
				return true;
		}

		return false;
	}

	void Category::setVisible( const bool p_visible )
	{
		BaseVisible::setVisible( p_visible );

		if ( isVisible() != p_visible )
		{
			_notifyViews( new Event::VTXEventValue<CATEGORY_ENUM>( Event::Model::CATEGORY_VISIBILITY, _category ) );
			_moleculePtr->propagateEventToViews(
				new Event::VTXEventValue<CATEGORY_ENUM>( Event::Model::CATEGORY_VISIBILITY, _category ) );
		}
	}
	void Category::setVisible( const bool p_visible, const bool p_notify )
	{
		BaseVisible::setVisible( p_visible );

		if ( isVisible() != p_visible )
		{
			if ( p_notify )
			{
				_notifyViews( new Event::VTXEventValue<CATEGORY_ENUM>( Event::Model::CATEGORY_VISIBILITY, _category ) );
				_moleculePtr->propagateEventToViews(
					new Event::VTXEventValue<CATEGORY_ENUM>( Event::Model::CATEGORY_VISIBILITY, _category ) );
			}
		}
	}

	void Category::_onRepresentationChange()
	{
		_notifyViews( new Event::VTXEvent( Event::Model::REPRESENTATION_CHANGE ) );
	}
} // namespace VTX::Model
