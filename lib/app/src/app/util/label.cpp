#include "app/util/label.hpp"

namespace VTX::Util::App::Label
{
	bool isLabelType( const VTX::ID::VTX_ID & p_id )
	{
		return p_id == ID::Model::MODEL_LABEL || p_id == ID::Model::MODEL_MEASUREMENT_DISTANCE
			   || p_id == ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE || p_id == ID::Model::MODEL_MEASUREMENT_ANGLE
			   || p_id == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE;
	}

} // namespace VTX::Util::App::Label
