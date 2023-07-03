#ifndef __VTX_IO_READER_CHEMFILES__
#define __VTX_IO_READER_CHEMFILES__

#include <core/chemdb/atom.hpp>
#include <core/chemdb/bond.hpp>
#include <core/chemdb/residue.hpp>
#include <string>
#include <util/exceptions.hpp>
#include <util/types.hpp>
#include <utility>
#include <vector>
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX::IO::Reader
{
	class Chemfiles
	{
	  public:
		static std::unique_ptr<Chemfiles> readFile( const FilePath & p_path );
		static std::unique_ptr<Chemfiles> readBuffer( const std::string & p_buffer, const FilePath & p_path );

		using ResidueIt = chemfiles::Residue::const_iterator;

	  private:
		static void _warningCallback( const std::string & p_log );

	  public:
		// Read File constructor
		Chemfiles( const FilePath & p_path );
		// Read Buffer constructor
		Chemfiles( const std::string & p_buffer, const FilePath & p_path );

		chemfiles::Trajectory & getTrajectory() { return _trajectory; }
		chemfiles::Frame &		getCurrentFrame() { return _currentFrame; }
		const FilePath &		getPath() { return _path; }

		// Trajectory
		size_t getFrameCount() const { return _trajectory.nsteps(); }
		size_t getResidueCount() const { return _residues->size(); }
		size_t getAtomCount() const { return _currentFrame.size(); }
		size_t getBondCount() const { return _currentFrame.topology().bonds().size(); }

		// Topology
		const chemfiles::Topology & getTopology() const { return _topology; }

		// Frame
		void			   readNextFrame();
		std::vector<Vec3f> getCurrentFrameAtomPosition() const;

		template<typename T>
		const T getCurrentResidueProperty( const std::string & p_property, const T & p_defaultValue = T() ) const
		{
			throw VTXException( "Incorrect type for chemfiles residue property." );
		}
		template<typename T>
		const T getCurrentAtomProperty( const std::string & p_property, const T & p_defaultValue = T() ) const
		{
			throw VTXException( "Incorrect type for chemfiles atom property." );
		}

		// Chain
		const std::string getCurrentChainName() const { return getCurrentResidueProperty<std::string>( "chainname" ); }
		const std::string getCurrentChainID() const { return getCurrentResidueProperty<std::string>( "chainid" ); }

		// Residue
		void setCurrentResidue( const size_t p_residueIndex )
		{
			_currentResidue = &( ( *_residues )[ p_residueIndex ] );
		}

		template<>
		const bool getCurrentResidueProperty<bool>( const std::string & p_property, const bool & p_defaultValue ) const
		{
			return _currentResidue->properties().get( p_property ).value_or( p_defaultValue ).as_bool();
		}

		template<>
		const std::string getCurrentResidueProperty<std::string>( const std::string & p_property,
																  const std::string & p_defaultValue ) const
		{
			return _currentResidue->properties().get( p_property ).value_or( p_defaultValue ).as_string();
		}
		template<>
		const double getCurrentResidueProperty<double>( const std::string & p_property,
														const double &		p_defaultValue ) const
		{
			return _currentResidue->properties().get( p_property ).value_or( p_defaultValue ).as_double();
		}
		const std::string & getCurrentResidueName() const { return _currentResidue->name(); }
		const size_t		getCurrentResidueFirstAtomIndex() const { return *_currentResidue->begin(); }
		const size_t		getCurrentResidueAtomCount() const { return _currentResidue->size(); }

		ResidueIt getCurrentResidueAtomIteratorBegin() const { return _currentResidue->cbegin(); }
		ResidueIt getCurrentResidueAtomIteratorEnd() const { return _currentResidue->cend(); }

		// Atom
		void setCurrentAtom( const size_t p_index )
		{
			_currentAtom	  = &( _currentFrame[ p_index ] );
			_currentAtomIndex = p_index;
		}

		template<>
		const bool getCurrentAtomProperty<bool>( const std::string & p_property, const bool & p_defaultValue ) const
		{
			return _currentAtom->properties().get( p_property ).value_or( p_defaultValue ).as_bool();
		}

		template<>
		const std::string getCurrentAtomProperty<std::string>( const std::string & p_property,
															   const std::string & p_defaultValue ) const
		{
			return _currentAtom->properties().get( p_property ).value_or( p_defaultValue ).as_string();
		}
		template<>
		const double getCurrentAtomProperty<double>( const std::string & p_property,
													 const double &		 p_defaultValue ) const
		{
			return _currentAtom->properties().get( p_property ).value_or( p_defaultValue ).as_double();
		}

		Vec3f getCurrentAtomPosition() const;

		const std::string &				getCurrentAtomName() const { return _currentAtom->name(); }
		VTX::Core::ChemDB::Atom::SYMBOL getCurrentAtomSymbol() const;
		int getCurrentAtomType() const { return int( getCurrentAtomProperty<double>( "atom_type", -1. ) ); }

		// Bonds
		void setCurrentBond( const size_t p_bondIndex )
		{
			_currentBond	  = &( ( *_bonds )[ p_bondIndex ] );
			_currentBondIndex = p_bondIndex;
		}

		size_t								 getCurrentBondFirstAtomIndex() const { return ( *_currentBond )[ 0 ]; }
		size_t								 getCurrentBondSecondAtomIndex() const { return ( *_currentBond )[ 1 ]; }
		const VTX::Core::ChemDB::Bond::ORDER getCurrentBondOrder()
		{
			const chemfiles::Bond::BondOrder & bondOrder = _topology.bond_orders()[ _currentBondIndex ];
			return VTX::Core::ChemDB::Bond::ORDER( int( bondOrder ) );
		}

	  protected:
		virtual void _preRead();
		virtual void _read();
		virtual void _postRead();

	  private:
		void _readTrajectory();

		const std::string _getFormat( const FilePath & );

		FilePath								_path;
		chemfiles::Trajectory					_trajectory;
		chemfiles::Frame						_currentFrame = chemfiles::Frame();
		chemfiles::Topology						_topology	  = chemfiles::Topology();
		const std::vector<chemfiles::Residue> * _residues	  = nullptr;
		const std::vector<chemfiles::Bond> *	_bonds		  = nullptr;

		const chemfiles::Residue * _currentResidue	 = nullptr;
		const chemfiles::Atom *	   _currentAtom		 = nullptr;
		size_t					   _currentAtomIndex = -1;
		const chemfiles::Bond *	   _currentBond		 = nullptr;
		size_t					   _currentBondIndex = -1;
	};
} // namespace VTX::IO::Reader
#endif
