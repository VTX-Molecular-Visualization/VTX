#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_BASE__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_BASE__

#if defined( _WIN32 ) || defined( _WIN64 )
#define LIB_EXPORT __declspec( dllexport )
#else
#define LIB_EXPORT
#endif

#endif
