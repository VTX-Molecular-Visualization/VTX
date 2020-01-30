#ifndef __VTX_MODEL_PATH__
#define __VTX_MODEL_PATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "../define.hpp"
#include "base_model.hpp"
#include "model_checkpoint.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class ModelPath : public BaseModel
		{
		  public:
			using VectorCheckpointPtr = std::vector<Model::ModelCheckpoint *>;

			~ModelPath();

		  private:
			VectorCheckpointPtr _checkpoints = VectorCheckpointPtr();

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
