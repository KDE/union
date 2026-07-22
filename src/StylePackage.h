// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>

#include <QObject>

#include "union_export.h"

namespace Union
{
class InputPlugin;
class Style;

/*!
 * A bundle of assets that represent a style.
 *
 * The StylePackage class is a representation of a package containing style
 * assets, along with metadata about that style.
 *
 * Packages are effectively directories with certain expected contents. At the
 * very least, a package is expected to contain a "metadata.json" file and a
 * "contents" directory. Other files may be required depending on the input
 * format used for the package.
 */
class UNION_EXPORT StylePackage
{
public:
    /*!
     *
     */
    enum class Error {
        None,
        NotFound,
        MissingFiles,
        InvalidMetaData,
        UnknownInputType,
    };

    /*!
     * Default constructor.
     *
     * Creates an empty and invalid package.
     */
    StylePackage();
    /*!
     * Creates a package pointing at a specific directories.
     */
    explicit StylePackage(const std::filesystem::path &path);
    StylePackage(StylePackage &&other);
    StylePackage(const StylePackage &other);
    ~StylePackage();

    StylePackage &operator=(StylePackage &&other);
    StylePackage &operator=(const StylePackage &other);

    /*!
     * Return the path of this package.
     */
    std::filesystem::path path() const;
    /*!
     * Returns the ID of this package.
     *
     * This is effectively the filename of the path.
     */
    QString id() const;
    /*!
     * Return the input type that is required for this package.
     */
    QString inputType() const;
    /*!
     * Returns the name of this package.
     */
    QString name() const;
    /*!
     * Returns the description of this package.
     */
    QString description() const;
    /*!
     * Returns the version of this package.
     *
     * Note that there are no constraints on the format of the version.
     */
    QString version() const;
    /*!
     * Returns the license of this package.
     */
    QString license() const;
    /*!
     * Returns the list of authors of this package.
     *
     * This is just a list of author names.
     */
    QStringList authors() const;

    /*!
     * Returns if the package is valid.
     *
     * This will load the package's metadata file to determine the input type
     * for this package, then ask the input plugin to validate the package, if
     * that was not done before.
     *
     * You can use the error() method to check what kind of problem an invalid
     * package has.
     */
    bool isValid() const;
    /*!
     * Returns the error that caused the package to be invalid.
     */
    Error error() const;
    /*!
     * Load the package fully and use it to create a new Style instance.
     */
    std::shared_ptr<Style> load();

private:
    void validate() const;
    void loadMetadata() const;

    class Private;
    std::unique_ptr<Private> d;
};

}

UNION_EXPORT QDebug operator<<(QDebug debug, const Union::StylePackage &package);
