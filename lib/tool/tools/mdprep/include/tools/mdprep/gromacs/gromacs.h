
#if defined( _WIN32 ) || defined( _WIN64 )
#define LIB_EXPORT __declspec( dllexport )
#else
#define LIB_EXPORT
#endif

namespace vtx::tool::mdprep
{
	struct poc_args;

} // namespace vtx::tool::mdprep

LIB_EXPORT void submit_gromacs_command( vtx::tool::mdprep::poc_args & );
