// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <Kirigami/Platform/PlatformTheme>

#include <QIcon>

class QuickStyle;
class OutputProperties;

class PlatformTheme : public Kirigami::Platform::PlatformTheme
{
    Q_OBJECT
public:
    explicit PlatformTheme(QObject *parent = nullptr);
    ~PlatformTheme() override;

    Q_INVOKABLE QIcon iconFromTheme(const QString &name, const QColor &customColor = Qt::transparent) override;

    void syncColors();

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *target, QEvent *event) override;

private:
    void syncColorSchemeColors();
    void syncUseAlternateBackground();

    QuickStyle *m_style = nullptr;
    OutputProperties *m_outputProperties = nullptr;
};
