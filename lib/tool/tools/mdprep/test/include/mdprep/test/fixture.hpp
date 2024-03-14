#include <memory>
#include <qapplication.h>
#include <tools/mdprep/gromacs/gromacs.hpp>
#include <tools/mdprep/gromacs/util.hpp>

namespace VTX::test
{
	struct setup_env
	{
		int								  i = 0;
		std::unique_ptr<QCoreApplication> papp { std::make_unique<QCoreApplication>( i, nullptr ) };
	};

	struct PrepareJobSetup
	{
		fs::path	 rootDir;
		const char * jobName;
		fs::path	 jobDir;
		PrepareJobSetup( const char * p_rootDirName, const char * jobName );
	};

	struct Pdb2gmx
	{
		inline static const fs::path & g_outputDir { VTX::Tool::Mdprep::executableDirectory() / "data"
													 / "1ubq_out_pdb2gmx" };

		inline static const std::vector<std::string> g_fileList { ( g_outputDir / "1ubq.gro" ).string(),
																  ( g_outputDir / "1ubq.itp" ).string(),
																  ( g_outputDir / "1ubq.ndx" ).string(),
																  ( g_outputDir / "1ubq.top" ).string() };
	};

	// Put a pointer to each of the input string into the pointer vector
	void fill( const std::vector<std::string> &, std::vector<const std::string *> & ) noexcept;

	struct Editconf
	{
		inline static const fs::path & g_outputDir { VTX::Tool::Mdprep::executableDirectory() / "data"
													 / "1ubq_out_editconf" };

		inline static const std::vector<std::string> g_fileList { ( g_outputDir / "1ubq.gro" ).string() };
	};

	inline std::vector<const std::string *> getPtrVectFromVect( const std::vector<std::string> & p_ )
	{
		std::vector<const std::string *> out;
		for ( auto & it : p_ )
			out.push_back( &it );
		return out;
	}

} // namespace VTX::test
