#include <vtx/secondary_structure/shared.hpp>

namespace pdb100
{

	/**
	 * @brief Create a system in the map and test it against ou secondary structure algorithm. Fill the log with a
	 * report.
	 * @param p_systemPath
	 * @param p_systemMap
	 */
	void testSystem( const fs::path & p_systemPath, SystemMap & p_systemMap );

	/**
	 * @brief Starts N threads (defined in the cmake) that will serve as a worker pool to analyze each file
	 * consecutively
	 * @param p_systemPaths Files to be analyzing
	 */
	void testSystems( VTX::Util::DataLocker<FileCollection> & p_systemPaths );

} // namespace pdb100
