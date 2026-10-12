// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include "TarArchive.h"

#include <format>
#include <fstream>

#include <QFileInfo>

extern "C" {
#include <zlib.h>
}

using namespace Tar;
using namespace Qt::StringLiterals;

namespace fs = std::filesystem;

TarArchive::TarArchive()
{
}

TarArchive::TarArchive(const std::filesystem::path &path)
    : m_path(path)
{
}

bool TarArchive::addEntry(const std::filesystem::path &path, std::error_code &errorCode)
{
    return addEntry(path, path.parent_path(), errorCode);
}

bool TarArchive::addEntry(const std::filesystem::path &path, const std::filesystem::path &baseDirectory, std::error_code &errorCode)
{
    QFileInfo fileInfo(path);

    if (!fileInfo.exists()) {
        errorCode = std::make_error_code(std::errc::no_such_file_or_directory);
        return false;
    }

    if (m_path.empty() || !fs::is_directory(m_path.parent_path())) {
        errorCode = std::make_error_code(std::errc::not_a_directory);
        return false;
    }

    auto pathString = fs::relative(path, baseDirectory).string();
    if (pathString.size() >= 100) {
        errorCode = std::make_error_code(std::errc::filename_too_long);
        return false;
    }

    TarEntry tarEntry;

    auto &header = tarEntry.header;
    std::format_to_n(header.name, sizeof(header.name), "{}", pathString);
    std::format_to_n(header.mode, sizeof(header.mode), "{:07o}", static_cast<int>(fs::status(path).permissions()));
    std::format_to_n(header.size, sizeof(header.size), "{:011o}", fileInfo.isDir() ? 0 : fileInfo.size());

    std::format_to_n(header.uid, sizeof(header.uid), "{:07o}", fileInfo.ownerId());
    std::format_to_n(header.gid, sizeof(header.uid), "{:07o}", fileInfo.groupId());
    std::format_to_n(header.mtime, sizeof(header.mtime), "{:011o}", fileInfo.lastModified().toSecsSinceEpoch());

    std::format_to_n(header.userName, sizeof(header.userName), "{}", fileInfo.owner().toStdString());
    std::format_to_n(header.groupName, sizeof(header.groupName), "{}", fileInfo.group().toStdString());

    if (fileInfo.isDir()) {
        header.name[pathString.size()] = '/';
        header.type = DirectoryType;
    } else {
        header.type = RegularFileType;

        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            errorCode = std::make_error_code(std::errc::io_error);
            return false;
        }

        tarEntry.data = file.readAll();
        tarEntry.padding = QByteArray{512 - tarEntry.data.size() % 512, '\0'};

        file.close();
    }

    std::format_to_n(header.checksum, sizeof(header.checksum), "        ");

    auto headerData = reinterpret_cast<uint8_t *>(&header);
    uint32_t checksum = 0;
    for (int i = 0; i < 512; ++i) {
        checksum += headerData[i];
    }

    std::format_to_n(header.checksum, sizeof(header.checksum), "{:06o}", checksum);
    header.checksum[6] = '\0';
    header.checksum[7] = ' ';

    m_entries.append(tarEntry);

    if (fileInfo.isDir()) {
        for (auto entry : fs::directory_iterator(path)) {
            if (!addEntry(entry.path(), baseDirectory, errorCode)) {
                return false;
            }
        }
    }

    return true;
}

bool TarArchive::write(std::error_code &errorCode)
{
    if (fs::exists(m_path)) {
        errorCode = std::make_error_code(std::errc::file_exists);
        return false;
    }

    QByteArray data;
    for (const auto &entry : std::as_const(m_entries)) {
        data.append(reinterpret_cast<const char *>(&entry.header), sizeof(USTarHeader));
        data.append(entry.data);
        data.append(entry.padding);
    }

    data.append(QByteArray{1024, '\0'});

    auto outfile = gzopen(m_path.string().data(), "wb");
    if (!outfile) {
        errorCode = std::error_code(errno, std::system_category());
        return false;
    }

    gzwrite(outfile, data.data(), data.size());
    gzclose(outfile);

    return true;
}
