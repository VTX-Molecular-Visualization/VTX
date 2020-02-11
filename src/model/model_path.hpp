#ifndef __VTX_MODEL_PATH__
#define __VTX_MODEL_PATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "../generic/base_serializable.hpp"
#include "../object3d/camera.hpp"
#include "base_model.hpp"
#include "define.hpp"
#include "model_checkpoint.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class ModelPath : public BaseModel, public Generic::BaseSerializable
		{
		  public:
			using CheckpointPtr		  = Model::ModelCheckpoint *;
			using VectorCheckpointPtr = std::vector<CheckpointPtr>;

			enum class COMPUTATION_MODE : int
			{
				DURATION,
				SPEED
			};

			ModelPath() { setId( 0 ); };
			~ModelPath();

			void addCheckpoint( const CheckpointPtr p_checkpoint ) { _checkpoints.emplace_back( p_checkpoint ); }
			void removeCheckpoint( const CheckpointPtr p_checkpoint )
			{
				_checkpoints.erase( std::find( _checkpoints.begin(), _checkpoints.end(), p_checkpoint ) );
			}
			inline VectorCheckpointPtr & getCheckpoints() { return _checkpoints; }

			virtual void								 setSelected( const bool ) override;
			void										 setSelectedCheckpoint( ModelCheckpoint * const );
			void										 resetSelectedCheckpoint();
			float										 computeTotalTime() const;
			ModelCheckpoint::CheckpointInterpolationData getCurrentCheckpointInterpolationData( float p_time ) const;

			// TODO: redo implementation.
			virtual void load( const IO::Path & ) override;
			virtual void save( const IO::Path & ) const override;

		  protected:
			virtual void _addItems() override final;

		  private:
			VectorCheckpointPtr _checkpoints		= VectorCheckpointPtr();
			COMPUTATION_MODE	_mode				= COMPUTATION_MODE::DURATION;
			CheckpointPtr		_selectedCheckpoint = nullptr;

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
