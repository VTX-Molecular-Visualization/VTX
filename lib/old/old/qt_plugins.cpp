#include <QtCore/QtPlugin>

// QT plugin Import for static builds on Linux 
#ifdef __linux__
	Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
	Q_IMPORT_PLUGIN(QXcbGlxIntegrationPlugin)
#endif

// QT plugin Import for Windows 
// #ifdef _WIN32
// 	Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
// #endif
