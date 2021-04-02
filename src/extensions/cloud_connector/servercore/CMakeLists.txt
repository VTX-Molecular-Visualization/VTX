find_package(Qt5 COMPONENTS Network REQUIRED)

include(GenerateExportHeader)

add_library(MoleQueueServerCore
  connection.h
  connectionlistener.h
  jsonrpc.cpp
  localsocketconnection.cpp
  localsocketconnectionlistener.cpp
  message.cpp
  messageidmanager_p.cpp
)

qt5_use_modules(MoleQueueServerCore Network)
set_target_properties(MoleQueueServerCore PROPERTIES AUTOMOC TRUE)
#target_link_libraries(MoleQueueServerCore)

set(hdrs
  connection.h
  connectionlistener.h
  connectionlistenerfactory.h
  jsonrpc.h
  localsocketconnection.h
  localsocketconnectionlistener.h
  message.h
  servercoreglobal.h
)

generate_export_header(MoleQueueServerCore
  EXPORT_FILE_NAME molequeueservercoreexport.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
set_property(TARGET MoleQueueServerCore APPEND
  PROPERTY COMPILE_FLAGS ${molequeue_export_flags})

list(APPEND hdrs
  "${CMAKE_CURRENT_BINARY_DIR}/molequeueservercoreexport.h")

install(FILES ${hdrs} DESTINATION "${INSTALL_INCLUDE_DIR}/molequeue/servercore")
install(TARGETS MoleQueueServerCore
  EXPORT "MoleQueueTargets"
  RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}
  LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
  ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR})
