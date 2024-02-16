#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_INPUTS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_INPUTS__

#include <memory>
#include <string_view>

class QProcess;

namespace VTX::Tool::Mdprep::Gromacs
{

	// This class is responsible for unfolding gromacs interactive process by entering interactive input when gromacs
	// expect it.
	class Inputs
	{
		class Model
		{
		  public:
			virtual ~Model()															 = default;
			virtual bool isWaitingForInput( const std::string_view & p_stdout ) noexcept = 0;
			virtual bool enterInput( QProcess &, std::string & stdout_ ) noexcept		 = 0;
		};

		template<typename ErasedType>
		class Container final : public Model
		{
			ErasedType _obj;

		  public:
			Container( ErasedType && p_ ) : _obj( std::move( p_ ) ) {}
			virtual bool isWaitingForInput( const std::string_view & p_stdout ) noexcept override {}
			virtual bool enterInput( QProcess &, std::string & stdout_ ) noexcept override {};
		};

		std::unique_ptr<Model> _ptr = nullptr;

	  public:
		template<typename ErasedType>
		Inputs( ErasedType && p_ ) : _ptr( std::make_unique<Container<ErasedType>>( std::move( p_ ) ) )
		{
		}

		// Returns whether gromacs is expecting something.
		inline bool isWaitingForInput( const std::string_view & p_stdout ) noexcept {}

		// To be called when gromacs is expecting inputs. Return if an input has successfuly be entered. Responsible for
		// handling the process if an input error is found. Will write the input in the stdout as well for logging
		// purpose.
		inline bool enterInput( QProcess &, std::string & stdout_ ) noexcept {}
	};
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
