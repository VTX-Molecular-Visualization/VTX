#ifndef __VTX_BASE_PRINTABLE__
#define __VTX_BASE_PRINTABLE__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		class BasePrintable
		{
		  public:
			virtual ~BasePrintable() = default;

			virtual void print() const {};
		};
	} // namespace Generic
} // namespace VTX
#endif
