#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_INPUTS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_INPUTS__

#include <concepts>
#include <memory>
#include <string>
#include <string_view>

class QProcess;

namespace VTX::Tool::Mdprep::backends::Gromacs
{

	class Inputs;
	// Any variation of the Inputs type should return false (e.g. const Inputs&)
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
		class Interface
		{
		  public:
			virtual ~Interface()																			= default;
			virtual bool _isWaitingForInput( const std::string_view & p_stdout ) const noexcept				= 0;
			virtual bool _enterInput( QProcess &, std::string & p_stdout, std::string & p_stderr ) noexcept = 0;
			virtual std::unique_ptr<Interface> clone() const noexcept										= 0;
		};

		template<typename ErasedType>
		class Container final : public Interface
		{
			ErasedType _obj;

		  public:
			Container( ErasedType && p_ ) : _obj( std::move( p_ ) ) {}
			Container( const ErasedType & p_ ) : _obj( p_ ) {}
			inline virtual bool _isWaitingForInput( const std::string_view & p_stdout ) const noexcept override
			{
				return VTX::Tool::Mdprep::backends::Gromacs::isWaitingForInput( _obj, p_stdout );
			}
			inline virtual bool _enterInput(
				QProcess &	  p_proc,
				std::string & p_stdout,
				std::string & p_stderr
			) noexcept override
			{
				return VTX::Tool::Mdprep::backends::Gromacs::enterInput( _obj, p_proc, p_stdout, p_stderr );
			}
			virtual std::unique_ptr<Interface> clone() const noexcept override
			{
				return std::make_unique<Container<ErasedType>>( ErasedType( _obj ) );
			};
		};

		std::unique_ptr<Interface> _ptr = nullptr;

	  public:
		template<typename ErasedType>
			requires NotInputsAtAll<ErasedType>
		Inputs( ErasedType && p_ ) : _ptr( std::make_unique<Container<ErasedType>>( std::forward<ErasedType>( p_ ) ) )
		{
		}
		~Inputs() = default;
		Inputs()  = default;

		Inputs( Inputs && p_m ) : _ptr( std::move( p_m._ptr ) ) {}
		inline Inputs & operator=( Inputs && p_m )
		{
			if ( &p_m == this )
				return *this;

			_ptr = std::move( p_m._ptr );
			return *this;
		}
		inline Inputs( const Inputs & p_c ) : _ptr( p_c._ptr->clone() ) {}
		inline Inputs & operator=( const Inputs & p_c )
		{
			_ptr = p_c._ptr->clone();
			return *this;
		}

		// Returns whether gromacs is expecting something.
		inline bool isWaitingForInput( const std::string_view & p_stdout ) const noexcept
		{
			return _ptr != nullptr && _ptr->_isWaitingForInput( p_stdout );
		}

		// To be called when gromacs is expecting inputs. Return if an input has successfuly be entered. Responsible for
		// handling the process if an input error is found. Will write the input in the stdout as well for logging
		// purpose.
		inline bool enterInput( QProcess & p_proc, std::string & p_stdout, std::string & p_stderr ) noexcept
		{
			return _ptr != nullptr && _ptr->_enterInput( p_proc, p_stdout, p_stderr );
		}
		bool operator==( const Inputs & ) const noexcept = default;
	};
} // namespace VTX::Tool::Mdprep::backends::Gromacs

#endif
