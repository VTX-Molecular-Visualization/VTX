#ifndef __VTX_MODEL_SELECTION__
#define __VTX_MODEL_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Selection : public BaseModel
		{
			using MapIds = std::map<ulong, std::map<ulong, std::map<ulong, std::vector<ulong>>>>;

		  public:
			Selection() : BaseModel( ID::Model::MODEL_SELECTION ) {}
			~Selection() = default;

		  private:
			MapIds _items = MapIds();
		};

	} // namespace Model
} // namespace VTX
#endif
