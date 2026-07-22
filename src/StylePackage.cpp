// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "StylePackage.h"

#include <QFile>
#include <QJsonArray>

#include "InputPlugin.h"
#include "Style.h"

#include "union_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

namespace fs = std::filesystem;

struct StyleMetaData {
    QString name;
    QString description;
    QString version;
    QString license;
    QUrl url;
    QStringList authors;
};

class UNION_NO_EXPORT StylePackage::Private
{
public:
    Error error = Error::None;

    fs::path path;
    QString inputType;

    std::optional<StyleMetaData> metaData;
};

StylePackage::StylePackage()
    : StylePackage(fs::path())
{
}

StylePackage::StylePackage(const std::filesystem::path &path)
    : d(std::make_unique<Private>())
{
    d->path = path;

    if (!fs::exists(d->path)) {
        qCDebug(UNION_GENERAL) << "Package" << path.string() << "was not found";
        d->error = Error::NotFound;
    } else if (!fs::exists(d->path / "metadata.json")) {
        qCDebug(UNION_GENERAL) << "Could not find metadata.json in package" << path.string();
        d->error = Error::MissingFiles;
    } else if (!fs::exists(d->path / "contents")) {
        qCDebug(UNION_GENERAL) << "Could not find contents directory in package" << path.string();
        d->error = Error::MissingFiles;
    }
}

StylePackage::StylePackage(StylePackage &&other)
    : d(std::move(other.d))
{
}

Union::StylePackage::StylePackage(const StylePackage &other)
    : d(std::make_unique<Private>())
{
    *this = other;
}

StylePackage::~StylePackage() = default;

StylePackage &Union::StylePackage::operator=(StylePackage &&other)
{
    std::swap(d, other.d);
    return *this;
}

StylePackage &Union::StylePackage::operator=(const StylePackage &other)
{
    if (this != &other) {
        d->error = other.d->error;
        d->path = other.d->path;
        d->inputType = other.d->inputType;
        d->metaData = other.d->metaData;
    }
    return *this;
}

fs::path StylePackage::path() const
{
    return d->path;
}

QString StylePackage::id() const
{
    return QString::fromStdString(d->path.filename());
}

QString StylePackage::inputType() const
{
    if (!isValid()) {
        return {};
    }

    return d->inputType;
}

QString StylePackage::name() const
{
    loadMetadata();

    if (d->metaData) {
        return d->metaData->name;
    }

    return {};
}

QString StylePackage::description() const
{
    loadMetadata();

    if (d->metaData) {
        return d->metaData->description;
    }

    return {};
}

QString StylePackage::version() const
{
    loadMetadata();

    if (d->metaData) {
        return d->metaData->version;
    }

    return {};
}

QString StylePackage::license() const
{
    loadMetadata();

    if (d->metaData) {
        return d->metaData->license;
    }

    return {};
}

QStringList StylePackage::authors() const
{
    loadMetadata();

    if (d->metaData) {
        return d->metaData->authors;
    }

    return {};
}

bool StylePackage::isValid() const
{
    if (d->error != Error::None) {
        return false;
    }

    if (d->inputType.isEmpty()) {
        validate();
    }

    return d->error == Error::None;
}

StylePackage::Error StylePackage::error() const
{
    return d->error;
}

Style::Ptr StylePackage::load()
{
    if (!isValid()) {
        return nullptr;
    }

    loadMetadata();

    auto inputPlugin = InputPlugin::inputPlugin(d->inputType);
    if (!inputPlugin) {
        return nullptr;
    }

    return inputPlugin->createStyle(*this);
}

void StylePackage::validate() const
{
    if (d->error != Error::None) {
        return;
    }

    QFile metaDataFile(d->path / "metadata.json");
    if (!metaDataFile.open(QIODevice::ReadOnly)) {
        qCDebug(UNION_GENERAL) << "Could not open metadata.json of package" << d->path.string() << "for reading";
        d->error = Error::InvalidMetaData;
        return;
    }

    QJsonParseError parseError;
    auto json = QJsonDocument::fromJson(metaDataFile.readAll(), &parseError).object();
    if (parseError.error != QJsonParseError::NoError) {
        qCDebug(UNION_GENERAL) << "Could not parse metadata for package" << d->path.string() << parseError.errorString();
        d->error = Error::InvalidMetaData;
        return;
    }

    if (json.isEmpty()) {
        qCDebug(UNION_GENERAL) << "Metadata for package" << d->path.string() << "is not an object";
        d->error = Error::InvalidMetaData;
        return;
    }

    d->inputType = json.value(u"input-type").toString();

    auto inputPlugin = InputPlugin::inputPlugin(d->inputType);
    if (!inputPlugin) {
        qCDebug(UNION_GENERAL) << "Could not find input type" << d->inputType << "for package" << d->path.string();
        d->error = Error::UnknownInputType;
        return;
    }

    if (auto result = inputPlugin->validatePackage(*this); result != Error::None) {
        qCDebug(UNION_GENERAL) << "Input plugin" << d->inputType << "considers package" << d->path.string() << "invalid";
        d->error = result;
        return;
    }
}

void StylePackage::loadMetadata() const
{
    if (d->metaData) {
        return;
    }

    if (!fs::exists(d->path / "metadata.json")) {
        return;
    }

    QFile metaDataFile(d->path / "metadata.json");
    if (!metaDataFile.open(QIODevice::ReadOnly)) {
        return;
    }

    auto json = QJsonDocument::fromJson(metaDataFile.readAll()).object();

    auto metaData = StyleMetaData{};
    metaData.name = json.value(u"name").toString();
    metaData.description = json.value(u"description").toString();
    metaData.version = json.value(u"version").toString();
    metaData.license = json.value(u"license").toString();
    metaData.url = QUrl{json.value(u"url").toString()};

    const auto authors = json.value(u"authors").toArray();
    std::ranges::transform(authors, std::back_inserter(metaData.authors), [](const QJsonValue &value) {
        return value.toString();
    });

    d->metaData = metaData;
}

QDebug operator<<(QDebug debug, const StylePackage &package)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "StylePackage(";
    debug << package.path().string();
    if (package.isValid()) {
        debug.nospace() << ", valid, inputType: " << package.inputType();
    } else {
        debug.nospace() << ", invalid: " << int(package.error());
    }
    debug << ")";
    return debug;
}
