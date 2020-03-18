#include "arc.hpp"
#undef INFINITE
#include <chemfiles.hpp>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			bool ARC::readFile( const Path & p_path, Model::Molecule & p_molecule ) { return false; }

			bool ARC::readBuffer( const std::string & p_buffer, Model::Molecule & p_molecule ) { return false; }

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
