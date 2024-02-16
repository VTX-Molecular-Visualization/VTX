#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_INPUTS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_INPUTS__

#include <concepts>
#include <memory>
#include <string_view>

class QProcess;

namespace VTX::Tool::Mdprep::Gromacs
{

	class Inputs;
	template<typename T>
	concept NotInputsAtAll = std::same_as<Inputs, T> == false && std::same_as<const Inputs, T> == false
							 && std::same_as<const Inputs &, T> == false && std::same_as<Inputs &&, T> == false
							 && std::same_as<Inputs &, T> == false;
	void isWaitingForInput();
	void enterInput();
	//  This class is responsible for unfolding gromacs interactive process by entering interactive input when gromacs
	//  expect it.
	class Inputs
	{
		class Model
		{
		  public:
			virtual ~Model()																				= default;
			virtual bool _isWaitingForInput( const std::string_view & p_stdout ) const noexcept				= 0;
			virtual bool _enterInput( QProcess &, std::string & p_stdout, std::string & p_stderr ) noexcept = 0;
		};

		template<typename ErasedType>
		class Container final : public Model
		{
			ErasedType _obj;

		  public:
			Container( ErasedType && p_ ) : _obj( std::move( p_ ) ) {}
			inline virtual bool _isWaitingForInput( const std::string_view & p_stdout ) const noexcept override
			{
				return VTX::Tool::Mdprep::Gromacs::isWaitingForInput( _obj, p_stdout );
			}
			inline virtual bool _enterInput(
				QProcess &	  p_proc,
				std::string & p_stdout,
				std::string & p_stderr
			) noexcept override
			{
				return VTX::Tool::Mdprep::Gromacs::enterInput( _obj, p_proc, p_stdout, p_stderr );
			}
		};

		std::unique_ptr<Model> _ptr = nullptr;

	  public:
		template<typename ErasedType>
			requires NotInputsAtAll<ErasedType>
		Inputs( ErasedType && p_ ) : _ptr( std::make_unique<Container<ErasedType>>( std::move( p_ ) ) )
		{
		}
		~Inputs() = default;
		// Returns whether gromacs is expecting something.
		inline bool isWaitingForInput( const std::string_view & p_stdout ) const noexcept
		{
			return _ptr->_isWaitingForInput( p_stdout );
		}

		// To be called when gromacs is expecting inputs. Return if an input has successfuly be entered. Responsible for
		// handling the process if an input error is found. Will write the input in the stdout as well for logging
		// purpose.
		inline bool enterInput( QProcess & p_proc, std::string & p_stdout, std::string & p_stderr ) noexcept
		{
			return _ptr->_enterInput( p_proc, p_stdout, p_stderr );
		}
	};
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
