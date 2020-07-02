#ifndef __VTX_BASE_RENDERABLE__
#define __VTX_BASE_RENDERABLE__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		class BaseRenderable
		{
		  public:
			virtual ~BaseRenderable() = default;

			virtual void render() = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
