#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_INPUTS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_INPUTS__

#include <memory>

namespace VTX::Tool::Mdprep::Gromacs
{

	// This class is responsible for unfolding gromacs interactive process by entering interactive input when gromacs
	// expect it.
	class Inputs
	{
		class Model
		{
		  public:
			virtual ~Model() = default;
		};

		template<typename ErasedType>
		class Container final : public Model
		{
			ErasedType _obj;

		  public:
			Container( ErasedType && o ) : _obj( std::move( o ) ) {}
		};

		std::unique_ptr<Model> _ptr = nullptr;

	  public:
		template<typename ErasedType>
		Inputs( ErasedType && o ) : _ptr( std::make_unique<Container<ErasedType>>( std::move( o ) ) )
		{
		}
	};
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
