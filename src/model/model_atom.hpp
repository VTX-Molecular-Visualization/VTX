#ifndef __VTX_MODEL_ATOM__
#define __VTX_MODEL_ATOM__

#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		enum ATOM_TYPE
		{
			H,
			C,
			N,
			O,
			NA,
			MG,
			P,
			S,
			CA,
			INVALID_TYPE,
			ATOM_TYPE_COUNT
		};

		class ModelAtom : BaseModel
		{
		  public:
			ModelAtom();

		  private:
		};
	} // namespace Model
} // namespace VTX
#endif
