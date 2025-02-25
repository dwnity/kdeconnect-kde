add_definitions(-DTRANSLATION_DOMAIN="kdeconnect-kded")

set(debug_file_SRCS)
ecm_qt_declare_logging_category(
    debug_file_SRCS HEADER kdeconnectd_debug.h
    IDENTIFIER KDECONNECT_DAEMON CATEGORY_NAME kdeconnect.daemon
    DEFAULT_SEVERITY Warning
    EXPORT kdeconnect-kde DESCRIPTION "kdeconnect (daemon)")

qt5_add_resources(kdeconnect_custom_icons_SRCS ${CMAKE_SOURCE_DIR}/icons/custom_icons.qrc)

add_executable(kdeconnectd kdeconnectd.cpp ${kdeconnect_custom_icons_SRCS} ${debug_file_SRCS})
target_link_libraries(kdeconnectd
    kdeconnectcore kdeconnectversion
    KF5::KIOWidgets
    KF5::DBusAddons
    KF5::Notifications
    KF5::I18n
    Qt5::Widgets)

ecm_mark_nongui_executable(kdeconnectd)

configure_file(org.kde.kdeconnect.daemon.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kdeconnect.daemon.desktop)

# Conditionally configure the daemon service according to the target OS
if(WIN32)
    set(SERV_EXEC kdeconnectd)
else()
    set(SERV_EXEC ${KDE_INSTALL_FULL_LIBEXECDIR}/kdeconnectd)
endif()
configure_file(org.kde.kdeconnect.service.in ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kdeconnect.service)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kdeconnect.daemon.desktop DESTINATION ${AUTOSTART_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kdeconnect.daemon.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kdeconnect.service DESTINATION ${DBUS_SERVICES_INSTALL_DIR})
install(TARGETS kdeconnectd DESTINATION ${LIBEXEC_INSTALL_DIR})

