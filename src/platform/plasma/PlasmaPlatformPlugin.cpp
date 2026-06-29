// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include "PlasmaPlatformPlugin.h"

#include <QDBusConnection>
#include <QFile>
#include <QGuiApplication>
#include <QIcon>
#include <QSettings>
#include <QStandardPaths>

#include <KIconColors>
#include <KRuntimePlatform>

using namespace Qt::StringLiterals;

PlasmaPlatformPlugin::PlasmaPlatformPlugin(QObject *parent)
    : Union::PlatformPlugin(parent)
    , m_kdeGlobalsWatcher(KConfigWatcher::create(KSharedConfig::openConfig()))
{
    KConfigGroup generalCfg = KConfigGroup(KSharedConfig::openConfig(), u"KDE"_s);
    m_smoothScroll = generalCfg.readEntry(u"SmoothScroll"_s, true);

    connect(m_kdeGlobalsWatcher.data(), &KConfigWatcher::configChanged, this, [this](const KConfigGroup &group, const QByteArrayList &names) {
        if (group.name() == "KDE"_L1) {
            if (names.contains("SmoothScroll"_ba)) {
                setSmoothScroll(group.readEntry(u"SmoothScroll"_s, true));
            }
        }
    });
}

QString PlasmaPlatformPlugin::defaultInputPlugin()
{
    return u"css"_s;
}

QString PlasmaPlatformPlugin::defaultStyleName()
{
    // These values should really be queries by the CSS using media queries,
    // however those are currently unimplemented in cxx-rust-cssparser and will
    // take some time to implement. So until that time, we switch the default
    // style based on these values.
    if (KRuntimePlatform::runtimePlatform().contains(u"phone"_s)) {
        return u"breeze-mobile"_s;
    }

    if (qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_MOBILE")) {
        return u"breeze-mobile"_s;
    }

    if (qGuiApp->layoutDirection() == Qt::LayoutDirection::RightToLeft) {
        return u"breeze-rtl"_s;
    }

    // TODO: Read from config
    return u"breeze"_s;
}

QIcon PlasmaPlatformPlugin::platformIcon(const QString &name, const QColor &color)
{
    if (color != Qt::transparent) {
        KIconColors colors;
        colors.setText(color);
        return KDE::icon(name, colors);
    } else {
        return KDE::icon(name);
    }
}

bool PlasmaPlatformPlugin::smoothScroll()
{
    return m_smoothScroll;
}

void PlasmaPlatformPlugin::setSmoothScroll(bool enabled)
{
    if (enabled == m_smoothScroll) {
        return;
    }

    m_smoothScroll = enabled;
    Q_EMIT smoothScrollChanged();
}
