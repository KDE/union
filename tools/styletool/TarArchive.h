// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include <cstdint>
#include <filesystem>

#include <QByteArray>
#include <QList>

namespace Tar
{

static constexpr uint8_t RegularFileType = '0';
static constexpr uint8_t DirectoryType = '5';

/* Header entry for an USTar format tar file
 */
struct USTarHeader {
    char name[100] = {'\0'};
    char mode[8] = {'\0'};
    uint8_t uid[8] = {0};
    uint8_t gid[8] = {0};
    uint8_t size[12] = {0};
    uint8_t mtime[12] = {0};
    char checksum[8] = {0};
    uint8_t type = {0};
    uint8_t linkName[100] = {0};
    uint8_t ustar[8] = "ustar  ";
    uint8_t userName[32] = {0};
    uint8_t groupName[32] = {0};
    uint8_t deviceMajor[8] = {0};
    uint8_t deviceMinor[8] = {0};
    uint8_t namePrefix[155] = {0};
    uint8_t padding[12] = {0};

    inline int actualSize() const
    {
        QByteArrayView sizeData(size, 11);
        return sizeData.toInt(nullptr, 8);
    }
};

/* A single entry in a tar file.
 */
struct TarEntry {
    USTarHeader header;
    QByteArray data;
    QByteArray padding;
};

/*
 * This is a very simple and naive implementation of creating an in-memory tar
 * archive that is written to disk with gzip compression. This avoids having to
 * drag in a complete archival library just to write a single package format.
 */
class TarArchive
{
public:
    TarArchive();
    TarArchive(const std::filesystem::path &path);

    bool addEntry(const std::filesystem::path &path, std::error_code &errorCode);
    bool addEntry(const std::filesystem::path &path, const std::filesystem::path &baseDirectory, std::error_code &errorCode);

    bool read(std::error_code &errorCode);
    bool write(std::error_code &errorCode);

    bool extract(const std::string &name, const std::filesystem::path &destination, std::error_code &errorCode);
    bool extractAll(const std::filesystem::path &destination, std::error_code &errorCode);

private:
    bool extractEntry(const std::filesystem::path &destination, const TarEntry &entry, std::error_code &errorCode);

    std::filesystem::path m_path;
    QList<TarEntry> m_entries;
};

}
