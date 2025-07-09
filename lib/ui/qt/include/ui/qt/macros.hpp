#ifndef __VTX_UI_QT_MACROS__
#define __VTX_UI_QT_MACROS__

#include <QtResource>

#define VTX_INIT_RESOURCES( p_targetName )                         \
	namespace                                                      \
	{                                                              \
		struct initializer                                         \
		{                                                          \
			initializer() { Q_INIT_RESOURCE( p_targetName ); }     \
			~initializer() { Q_CLEANUP_RESOURCE( p_targetName ); } \
		} dummy;                                                   \
	}
#endif
