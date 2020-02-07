#ifndef __VTX_MODEL_PATH__
#define __VTX_MODEL_PATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "../object3d/camera.hpp"
#include "base_model.hpp"
#include "define.hpp"
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

			ModelPath() { setId( 0 ); };
			~ModelPath();

			void addCheckpoint( const CheckpointPtr p_checkpoint ) { _checkpoints.emplace_back( p_checkpoint ); }
			inline VectorCheckpointPtr &	getCheckpoints() { return _checkpoints; }
			inline Model::ModelCheckpoint & getCheckpoint( uint p_index ) { return *_checkpoints.at( p_index ); }

			virtual void								 setSelected( const bool ) override;
			void										 setSelectedCheckpoint( const uint );
			float										 computeTotalTime() const;
			ModelCheckpoint::CheckpointInterpolationData getCurrentCheckpointInterpolationData( float p_time ) const;

		  protected:
			virtual void _addItems() override final;

		  private:
			VectorCheckpointPtr _checkpoints		= VectorCheckpointPtr();
			CheckpointPtr		_selectedCheckpoint = nullptr;

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
