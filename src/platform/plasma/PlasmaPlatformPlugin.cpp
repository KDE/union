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

static const QString DefaultStyleId = u"breeze"_s;

PlasmaPlatformPlugin::PlasmaPlatformPlugin(QObject *parent)
    : Union::PlatformPlugin(parent)
    , m_kdeGlobalsWatcher(KConfigWatcher::create(KSharedConfig::openConfig()))
{
    KConfigGroup generalCfg = KConfigGroup(KSharedConfig::openConfig(), u"KDE"_s);
    m_smoothScroll = generalCfg.readEntry(u"SmoothScroll"_s, true);
    m_animationSpeedMultiplier = std::max<double>(0.0, generalCfg.readEntry(u"AnimationDurationFactor"_s, 1.0));
    m_defaultStyle = generalCfg.readEntry(u"unionStyle"_s, DefaultStyleId);

    connect(m_kdeGlobalsWatcher.data(), &KConfigWatcher::configChanged, this, [this](const KConfigGroup &group, const QByteArrayList &names) {
        if (group.name() == "KDE"_L1) {
            if (names.contains("AnimationDurationFactor"_ba)) {
                setAnimationSpeedMultiplier(std::max<double>(0.0, group.readEntry(u"AnimationDurationFactor"_s, 1.0)));
            }

            if (names.contains("SmoothScroll"_ba)) {
                setSmoothScroll(group.readEntry(u"SmoothScroll"_s, true));
            }

            if (names.contains("unionStyle"_ba)) {
                setDefaultStyle(group.readEntry(u"unionStyle"_s, DefaultStyleId));
            }
        }
    });

    // Listen to smooth scroll changed events
    QDBusConnection::sessionBus().connect(QString(),
                                          QStringLiteral("/SmoothScroll"),
                                          QStringLiteral("org.kde.SmoothScroll"),
                                          QStringLiteral("notifyChange"),
                                          this,
                                          SLOT(setSmoothScroll(bool)));
}

QString PlasmaPlatformPlugin::defaultStyleName()
{
    // These values should really be queries by the CSS using media queries,
    // however those are currently unimplemented in cxx-rust-cssparser and will
    // take some time to implement. So until that time, we switch the default
    // style based on these values.
    if (m_defaultStyle == u"breeze") {
        if (KRuntimePlatform::runtimePlatform().contains(u"phone"_s)) {
            return u"breeze-mobile"_s;
        }

        if (qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_MOBILE")) {
            return u"breeze-mobile"_s;
        }

        if (qGuiApp->layoutDirection() == Qt::LayoutDirection::RightToLeft) {
            return u"breeze-rtl"_s;
        }
    }

    return m_defaultStyle;
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

qreal PlasmaPlatformPlugin::animationSpeedMultiplier()
{
    return m_animationSpeedMultiplier;
}

void PlasmaPlatformPlugin::setAnimationSpeedMultiplier(qreal multiplier)
{
    if (qFuzzyCompare(multiplier, m_animationSpeedMultiplier)) {
        return;
    }

    m_animationSpeedMultiplier = multiplier;
    Q_EMIT animationSpeedMultiplierChanged();
    sendAnimationSpeedMultiplierChangedEvent();
}

void PlasmaPlatformPlugin::setDefaultStyle(const QString &style)
{
    if (m_defaultStyle == style) {
        return;
    }

    m_defaultStyle = style;
    sendDefaultStyleChangedEvent();
}
