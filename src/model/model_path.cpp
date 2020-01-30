#include "model_path.hpp"

namespace VTX
{
	namespace Model
	{
		ModelPath::~ModelPath()
		{
			for ( Model::ModelCheckpoint * checkpoint : _checkpoints )
			{
				delete checkpoint;
			}
			_checkpoints.clear();
		}

	} // namespace Model
} // namespace VTX
