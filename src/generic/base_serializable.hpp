#ifndef __VTX_BASE_SERIALIZABLE__
#define __VTX_BASE_SERIALIZABLE__

#ifdef _MSC_VER
#pragma once
#endif



namespace VTX
{
	namespace Generic
	{
		class BaseSerializable
		{
		  public:
			virtual ~BaseSerializable() = default;

			virtual void load( const Path & )		= 0;
			virtual void save( const Path & ) const = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
