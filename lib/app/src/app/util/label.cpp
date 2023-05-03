#include "app/util/label.hpp"

namespace VTX::Util::App::Label
{
	bool isLabelType( const VTX::App::VTX_ID & p_id )
	{
		return p_id == VTX::App::ID::Model::MODEL_LABEL || p_id == VTX::App::ID::Model::MODEL_MEASUREMENT_DISTANCE
			   || p_id == VTX::App::ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE
			   || p_id == VTX::App::ID::Model::MODEL_MEASUREMENT_ANGLE
			   || p_id == VTX::App::ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE;
	}

} // namespace VTX::Util::App::Label
