#ifndef __VTX_IO_READER_CHEMFILES__
#define __VTX_IO_READER_CHEMFILES__

#include <core/chemdb/atom.hpp>
#include <core/chemdb/bond.hpp>
#include <core/chemdb/residue.hpp>
#include <memory>
#include <string>
#include <util/constants.hpp>
#include <util/exceptions.hpp>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::IO::Reader
{
	class Chemfiles
	{
	  public:
		static std::unique_ptr<Chemfiles> readFile( const FilePath & p_path );
		static std::unique_ptr<Chemfiles> readBuffer( const std::string & p_buffer, const FilePath & p_path );

		struct ResidueIt
		{
		  private:
			friend Chemfiles;

		  public:
			ResidueIt();
			ResidueIt( const ResidueIt & p_source );

			~ResidueIt();

			atom_index_t operator*() const;
			atom_index_t operator->() const;

			// Prefix increment
			ResidueIt & operator++();

			// Postfix increment
			ResidueIt operator++( int );

			friend bool operator==( const ResidueIt & p_lhs, const ResidueIt & p_rhs );
			friend bool operator!=( const ResidueIt & p_lhs, const ResidueIt & p_rhs );

		  private:
			struct InternalResidueIt;

			ResidueIt( InternalResidueIt & p_internalIt );
			std::unique_ptr<InternalResidueIt> _internalIterator;
		};

	  private:
		static void _warningCallback( const std::string & p_log );

	  public:
		// Read File constructor
		Chemfiles( const FilePath & p_path );

		// Read Buffer constructor
		Chemfiles( const std::string & p_buffer, const FilePath & p_path );

		~Chemfiles();

		// chemfiles::Trajectory & getTrajectory() { return _trajectory; }
		// chemfiles::Frame &		getCurrentFrame() { return _currentFrame; }
		const FilePath & getPath() { return _path; }

		// Trajectory
		size_t getFrameCount() const;
		size_t getResidueCount() const;
		size_t getAtomCount() const;
		size_t getBondCount() const;

		// Frame
		const std::string getFrameName() const;
		const std::string getPdbIdCode() const;

		void			   readNextFrame();
		std::vector<Vec3f> getCurrentFrameAtomPosition() const;

		// Chain
		const std::string getCurrentChainName() const { return getCurrentResidueStringProperty( "chainname" ); }
		const std::string getCurrentChainID() const { return getCurrentResidueStringProperty( "chainid" ); }

		// Residue
		void setCurrentResidue( const size_t p_residueIndex );

		const std::string getCurrentResidueStringProperty(
			const std::string & p_property,
			const std::string & p_defaultValue = ""
		) const;
		const double getCurrentResidueDoubleProperty( const std::string & p_property, const double p_defaultValue = 0. )
			const;
		const bool getCurrentResidueBoolProperty( const std::string & p_property, const bool p_defaultValue = false )
			const;

		const std::string & getCurrentResidueName() const;
		const size_t		getCurrentResidueId() const;
		const atom_index_t	getCurrentResidueFirstAtomIndex() const;
		const atom_index_t	getCurrentResidueAtomCount() const;

		ResidueIt getCurrentResidueAtomIteratorBegin() const;
		ResidueIt getCurrentResidueAtomIteratorEnd() const;

		// Atom
		void setCurrentAtom( const size_t p_index );

		const std::string getCurrentAtomStringProperty(
			const std::string & p_property,
			const std::string & p_defaultValue = ""
		) const;
		const double getCurrentAtomDoubleProperty( const std::string & p_property, const double p_defaultValue = 0. )
			const;
		const bool getCurrentAtomBoolProperty( const std::string & p_property, const bool p_defaultValue = false )
			const;

		const std::string &				getCurrentAtomName() const;
		Vec3f							getCurrentAtomPosition() const;
		VTX::Core::ChemDB::Atom::SYMBOL getCurrentAtomSymbol() const;
		int								getCurrentAtomType() const;

		// Bonds
		void setCurrentBond( const size_t p_bondIndex );

		atom_index_t						 getCurrentBondFirstAtomIndex() const;
		atom_index_t						 getCurrentBondSecondAtomIndex() const;
		const VTX::Core::ChemDB::Bond::ORDER getCurrentBondOrder() const;

	  protected:
		virtual void _preRead();
		virtual void _read();
		virtual void _postRead();

	  private:
		void _readTrajectory();

		const std::string _getFormat( const FilePath & );

		FilePath _path;

		struct ReadingData;
		std::unique_ptr<ReadingData> _readingData;
	};
} // namespace VTX::IO::Reader
#endif
