#include "app/old/util/label.hpp"

namespace VTX::Util::App::Old::Label
{
	bool isLabelType( const VTX::App::Old::VTX_ID & p_id )
	{
		return p_id == VTX::App::Old::ID::Model::MODEL_LABEL || p_id == VTX::App::Old::ID::Model::MODEL_MEASUREMENT_DISTANCE
			   || p_id == VTX::App::Old::ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE
			   || p_id == VTX::App::Old::ID::Model::MODEL_MEASUREMENT_ANGLE
			   || p_id == VTX::App::Old::ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE;
	}

} // namespace VTX::Util::App::Old::Label
