#include <memory>
#include <qapplication.h>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <app/fixture.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <tool/mdprep/backends/gromacs/util.hpp>

using VTX::FilePath;

namespace VTX::test
{
	struct setup_env
	{
		int								   i = 0;
		std::unique_ptr<QCoreApplication>  papp { std::make_unique<QCoreApplication>( i, nullptr ) };
		std::unique_ptr<VTX::App::Fixture> app { std::make_unique<VTX::App::Fixture>() };
	};

	struct PrepareJobSetup
	{
		std::unique_ptr<VTX::App::Fixture> app { std::make_unique<VTX::App::Fixture>() };
		FilePath						   rootDir;
		const char *					   jobName;
		FilePath						   jobDir;
		PrepareJobSetup( const char * p_rootDirName, const char * jobName );
	};

	struct Pdb2gmx
	{
		static const FilePath & outputDir()
		{
			static const FilePath outputDir = VTX::App::SESSION().getDataDir() / "1ubq_out_pdb2gmx";
			return outputDir;
		}

		static const std::vector<std::string> & fileList()
		{
			static const std::vector<std::string> fileList { ( outputDir() / "1ubq.gro" ).string(),
															 ( outputDir() / "1ubq.itp" ).string(),
															 ( outputDir() / "1ubq.ndx" ).string(),
															 ( outputDir() / "1ubq.top" ).string() };
			return fileList;
		}
	};

	// Put a pointer to each of the input string into the pointer vector
	void fill(
		const std::vector<std::string> &,
		VTX::Tool::Mdprep::backends::Gromacs::CumulativeOuputFiles &
	) noexcept;

	struct Editconf
	{
		static const FilePath & outputDir()
		{
			static const FilePath outputDir = VTX::App::SESSION().getDataDir() / "1ubq_out_editconf";
			return outputDir;
		}

		static const std::vector<std::string> & fileList()
		{
			static const std::vector<std::string> fileList { ( outputDir() / "1ubq.gro" ).string() };
			return fileList;
		}
	};

	struct Solvate
	{
		static const FilePath & outputDir()
		{
			static const FilePath outputDir = VTX::App::SESSION().getDataDir() / "1ubq_out_solvate";
			return outputDir;
		}

		static const std::vector<std::string> & fileList()
		{
			static const std::vector<std::string> fileList { ( outputDir() / "1ubq.gro" ).string(),
															 ( outputDir() / "1ubq.top" ).string() };
			return fileList;
		}
	};

	struct TrjConv
	{
		static const FilePath & outputDir()
		{
			static const FilePath outputDir = VTX::App::SESSION().getDataDir() / "1ubq_out_trjconv";
			return outputDir;
		}

		static const std::vector<std::string> & fileList()
		{
			static const std::vector<std::string> fileList { ( outputDir() / "1ubq.gro" ).string() };
			return fileList;
		}
	};

	struct Grompp
	{
		static const FilePath & outputDir()
		{
			static const FilePath outputDir = VTX::App::SESSION().getDataDir() / "1ubq_out_grompp";
			return outputDir;
		}

		static const std::vector<std::string> & fileList()
		{
			static const std::vector<std::string> fileList { ( outputDir() / "1ubq.top" ).string(),
															 ( outputDir() / "1ubq.mdp" ).string(),
															 ( outputDir() / "1ubq.tpr" ).string() };
			return fileList;
		}
	};

	struct Genion
	{
		static const FilePath & outputDir()
		{
			static const FilePath outputDir = VTX::App::SESSION().getDataDir() / "1ubq_out_genion";
			return outputDir;
		}

		static const std::vector<std::string> & fileList()
		{
			static const std::vector<std::string> fileList { ( outputDir() / "1ubq.top" ).string(),
															 ( outputDir() / "1ubq.gro" ).string() };
			return fileList;
		}
	};

	struct Grompp2
	{
		static const FilePath & outputDir()
		{
			static const FilePath outputDir = VTX::App::SESSION().getDataDir() / "1ubq_out_grompp2";
			return outputDir;
		}

		static const std::vector<std::string> & fileList()
		{
			static const std::vector<std::string> fileList { ( outputDir() / "1ubq.top" ).string(),
															 ( outputDir() / "1ubq.mdp" ).string(),
															 ( outputDir() / "1ubq.tpr" ).string() };
			return fileList;
		}
	};

	inline std::vector<const std::string *> getPtrVectFromVect( const std::vector<std::string> & p_ )
	{
		std::vector<const std::string *> out;
		for ( auto & it : p_ )
		{
			out.push_back( &it );
		}
		return out;
	}

	std::string getFileContent( const FilePath & p_file ) noexcept;

} // namespace VTX::test
