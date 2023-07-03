#ifndef __VTX_IO_READER_MOLECULE__
#define __VTX_IO_READER_MOLECULE__

// #include "app/old/core/io/reader/base_reader.hpp"
// #include "io/metadata/molecule.hpp"
// #include <core/base_struct/_fwd.hpp>
// #include <core/struct/concept.hpp>
// #include <string>
// #include <util/types.hpp>
// #include <utility>
// #include <vector>
// #pragma warning( push, 0 )
// #include <chemfiles.hpp>
// #pragma warning( pop )

namespace VTX::IO::Reader
{
	// namespace Struct = VTX::Core::BaseStruct;

	// class Molecule /*: public App::Core::IO::Reader::BaseReader<App::Model::Chemistry::Molecule>*/
	//{
	//   private:
	//	struct ChemfileTrajectoryInfo
	//	{
	//		ChemfileTrajectoryInfo( const chemfiles::Trajectory &, const chemfiles::Frame &, const FilePath & );

	//		const FilePath &						filePath;
	//		const chemfiles::Trajectory &			trajectory;
	//		const chemfiles::Frame &				frame;
	//		const chemfiles::Topology &				topology;
	//		const std::vector<chemfiles::Residue> & residues;
	//		const std::vector<chemfiles::Bond> &	bonds;
	//	};
	//	struct FillStructureInfo
	//	{
	//		// Chain Data
	//		Struct::Chain * modelChain				 = nullptr;
	//		std::string		lastChainName			 = "";
	//		size_t			chainModelId			 = -1;
	//		size_t			currentChainResidueCount = 0;
	//		int				oldIndexInChain			 = INT_MIN;

	//		std::string currentChainName = "";
	//		std::string currentChainId	 = "";

	//		// Category Data
	//		VTX::Core::ChemDB::Category::TYPE lastCategoryEnum = VTX::Core::ChemDB::Category::TYPE::UNKNOWN;

	//		// Residue Data
	//		size_t		currentResidueIndex	 = 0;
	//		std::string currentResidueSymbol = "";

	//		// Bonds Data
	//		std::map<size_t, std::vector<size_t>> mapResidueBonds	   = std::map<size_t, std::vector<size_t>>();
	//		std::map<size_t, std::vector<size_t>> mapResidueExtraBonds = std::map<size_t, std::vector<size_t>>();
	//	};

	//  public:
	//	Molecule() {};

	//	void readFile( const FilePath & p_path, Struct::Molecule & p_molecule );
	//	void readBuffer( const std::string &, const FilePath &, Struct::Molecule & );

	//	std::vector<Vec3f> readTrajectoryFrame( chemfiles::Trajectory & p_trajectory );
	//	void			   fillTrajectoryFrame( Struct::Molecule &		   p_molecule,
	//											const size_t			   p_moleculeFrameIndex,
	//											const std::vector<Vec3f> & p_atomPositions );

	//  private:
	//	void			  _readTrajectory( chemfiles::Trajectory &, const FilePath &, Struct::Molecule & );
	//	const std::string _getFormat( const FilePath & );
	//	const bool		  _needToRecomputeBonds( const std::string & p_format ) const;
	//	const bool		  _checkTrajectoryFiles( const std::string & p_format ) const;
	//	const bool		  _readTrajectoryFile( const std::string & p_format ) const;

	//	void _readTrajectoryFrames( chemfiles::Trajectory &									   p_trajectory,
	//								const std::vector<std::pair<Struct::Molecule *, size_t>> & p_targets,
	//								const size_t p_trajectoryFrameStart = 0 );

	//	Metadata::Molecule _metaData = Metadata::Molecule();

	//	// STEPS
	//	void _setupTrajectory();

	//	void _preRead( const FilePath & p_path, chemfiles::Trajectory & p_trajectory );
	//	void _read( chemfiles::Trajectory & p_trajectory, chemfiles::Frame & p_frame );
	//	void _postRead( chemfiles::Frame & p_frame );

	//	void _preFillStructure( Struct::Molecule &, const ChemfileTrajectoryInfo & );
	//	void _fillStructure( Struct::Molecule &, const ChemfileTrajectoryInfo & );
	//	void _postFillStructure( Struct::Molecule &, const ChemfileTrajectoryInfo & );

	//	void _setMoleculeData( Struct::Molecule &, const ChemfileTrajectoryInfo & );
	//	void _createNewChain( Struct::Molecule &, FillStructureInfo & );
	//	void _setChainData( Struct::Chain &, const ChemfileTrajectoryInfo &, const FillStructureInfo & );
	//};
} // namespace VTX::IO::Reader
#endif
