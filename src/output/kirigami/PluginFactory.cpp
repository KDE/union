// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PluginFactory.h"

#include <Kirigami/Platform/StyleHints>

#include "../qtquick/plugin/StyleHints.h"

#include "PlatformTheme.h"
#include "Units.h"
#include <StyleRegistry.h>

class KirigamiStyleHints : public Kirigami::Platform::StyleHints
{
public:
    KirigamiStyleHints(QObject *parent = nullptr)
        : Kirigami::Platform::StyleHints(parent)
    {
        m_unionStyleHints = static_cast<Union::Quick::StyleHints *>(qmlAttachedPropertiesObject<Union::Quick::StyleHints>(parent));
        auto platform = Union::StyleRegistry::instance()->platform();
        connect(platform.get(), &Union::PlatformPlugin::scrollingModifiesControlsChanged, this, [this]() {
            propertyChanged(ChangedProperty::ScrollingModifiesControls);
        });
    }

    bool scrollingModifiesControls() const override
    {
        return Union::StyleRegistry::instance()->platform()->scrollingModifiesControls();
    }

    void propertyChanged(ChangedProperty change) override
    {
        if (!m_unionStyleHints) {
            return;
        }

        switch (change) {
        case ChangedProperty::UseAlternateBackgroundColor:
            m_unionStyleHints->setUseAlternatingColors(useAlternateBackgroundColor());
            break;
        case ChangedProperty::ShowFramedBackground:
            m_unionStyleHints->setShowFramedBackground(showFramedBackground());
            break;
        case ChangedProperty::TickMarkStepSize:
            m_unionStyleHints->setTickMarkStepSize(tickMarkStepSize());
            break;
        case ChangedProperty::IconName:
            m_unionStyleHints->setIconName(iconName());
            break;
        case ChangedProperty::IconSource:
            m_unionStyleHints->setIconSource(iconSource());
            break;
        case ChangedProperty::ScrollingModifiesControls:
            Q_EMIT scrollingModifiesControlsChanged();
            break;
        }
    }

private:
    Union::Quick::StyleHints *m_unionStyleHints = nullptr;
};

PluginFactory::PluginFactory(QObject *parent)
    : Kirigami::Platform::PlatformPluginFactory(parent)
{
    Kirigami::Platform::StyleHints::setMakeStyleHintsFunction([](QObject *object) {
        return new KirigamiStyleHints(object);
    });
}

Kirigami::Platform::PlatformTheme *PluginFactory::createPlatformTheme(QObject *parent)
{
    return new PlatformTheme(parent);
}

Kirigami::Platform::Units *PluginFactory::createUnits(QObject *parent)
{
    return new Units(parent);
}

#include "moc_PluginFactory.cpp"
