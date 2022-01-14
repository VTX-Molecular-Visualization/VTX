#ifndef __VTX_MODEL_LABEL__
#define __VTX_MODEL_LABEL__

#include "id.hpp"
#include "model/base_model.hpp"

namespace VTX::Model
{
	class Label : public BaseModel
	{
		VTX_MODEL

	  public:
	  protected:
		Label() : BaseModel( VTX::ID::Model::MODEL_LABEL ) {};
		Label( const VTX::ID::VTX_ID & p_typeId ) : BaseModel( p_typeId ) {};

		virtual ~Label() {}
	};

} // namespace VTX::Model
#endif
