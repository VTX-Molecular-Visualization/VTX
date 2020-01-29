#ifndef __VTX_BASE_DRAWABLE__
#define __VTX_BASE_DRAWABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_collectionable.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseDrawable : public BaseCollectionable
		{
		  public:
			virtual void draw();

		  protected:
			virtual void _draw() = 0;
			virtual void _draw( std::string );

			bool * _show = nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
