project(kate-fuzzysearch)

# search packages used by KDE
find_package(KDE4 REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckLibraryExists)
include(CheckCSourceCompiles)


add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})




########### next target ###############

set(katefuzzydocumentswitcherplugin_PART_SRCS sortfuzzyfilterproxymodel.cpp plugin_katefuzzydocumentswitcher.cpp )


kde4_add_plugin(katefuzzydocumentswitcherplugin ${katefuzzydocumentswitcherplugin_PART_SRCS})



target_link_libraries(katefuzzydocumentswitcherplugin  ${KDE4_KDEUI_LIBS} ${KDE4_KTEXTEDITOR_LIBS} ${KDE4_KPARTS_LIBS} kateinterfaces )

install(TARGETS katefuzzydocumentswitcherplugin  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES ui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kate/plugins/katefuzzydocumentswitcher )
install( FILES katefuzzydocumentswitcher.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
