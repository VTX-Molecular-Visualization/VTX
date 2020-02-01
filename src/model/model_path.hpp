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
			using CheckpointPtr		  = Model::ModelCheckpoint *;
			using VectorCheckpointPtr = std::vector<CheckpointPtr>;

			~ModelPath();

			void addCheckpoint( const CheckpointPtr p_checkpoint ) { _checkpoints.emplace_back( p_checkpoint ); }
			inline VectorCheckpointPtr & getCheckpoints() { return _checkpoints; }

			void setSelectedCheckpoint( const uint );

			float computeTotalTime() const;

		  protected:
			virtual void _addItems() override final;

		  private:
			VectorCheckpointPtr _checkpoints		= VectorCheckpointPtr();
			CheckpointPtr		_selectedCheckpoint = nullptr;

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
