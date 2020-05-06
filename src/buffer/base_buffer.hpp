#ifndef __VTX_BASE_BUFFER__
#define __VTX_BASE_BUFFER__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Buffer
	{
		class BaseBuffer
		{
		  public:
			BaseBuffer()		  = default;
			virtual ~BaseBuffer() = default;

			virtual void bind()	  = 0;
			virtual void unbind() = 0;
		};
	} // namespace Buffer
} // namespace VTX
#endif
