#include "app/old_app/util/label.hpp"

namespace VTX::Util::Label
{
	bool isLabelType( const ID::VTX_ID & p_id )
	{
		return p_id == ID::Model::MODEL_LABEL || p_id == ID::Model::MODEL_MEASUREMENT_DISTANCE
			   || p_id == ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE || p_id == ID::Model::MODEL_MEASUREMENT_ANGLE
			   || p_id == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE;
	}

} // namespace VTX::Util::Label
