add_library(org.kde.union MODULE)

target_sources(org.kde.union PRIVATE
    PluginFactory.cpp
    PlatformTheme.cpp
    Units.cpp
)

target_link_libraries(org.kde.union
    PUBLIC
        Qt6::Core
        KF6::KirigamiPlatform
    PRIVATE
        Qt6::Qml
        Qt6::Quick
        Union
        UnionQuickImpl
)

install(TARGETS org.kde.union DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/kirigami/platform)

install(FILES
    AbstractApplicationHeader.qml
    AbstractCard.qml
    Heading.qml
    DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami/styles/org.kde.union
)
