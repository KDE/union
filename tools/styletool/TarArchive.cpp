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

bool TarArchive::read(std::error_code &errorCode)
{
    m_entries.clear();

    if (!fs::exists(m_path)) {
        errorCode = std::make_error_code(std::errc::no_such_file_or_directory);
        return false;
    }

    auto infile = gzopen(m_path.string().data(), "rb");
    if (!infile) {
        errorCode = std::error_code(errno, std::system_category());
        return false;
    }

    constexpr int BufferSize = 1024;
    QByteArray data;
    std::array<char, BufferSize> buffer;
    while (true) {
        int read = gzread(infile, buffer.data(), buffer.size());
        if (read == BufferSize) {
            data.append(buffer);
        } else if (read > 0) {
            data.append(buffer);
            break;
        } else {
            errorCode = std::make_error_code(std::errc::io_error);
            return false;
        }
    }

    if (data.size() == 0) {
        errorCode = std::make_error_code(std::errc::io_error);
        return false;
    }

    constexpr int ChunkSize = 512;
    auto offset = 0;
    auto remainingSize = data.size();
    auto view = QByteArrayView(data);
    TarEntry currentEntry;
    bool first = true;
    while (remainingSize > 0) {
        auto chunk = view.sliced(offset, ChunkSize);
        remainingSize -= ChunkSize;
        offset += ChunkSize;

        if (chunk.sliced(257, 5) == "ustar") {
            if (!first) {
                currentEntry.data = currentEntry.data.sliced(0, currentEntry.header.actualSize());
                m_entries.append(currentEntry);
                currentEntry = TarEntry{};
            } else {
                currentEntry = TarEntry{};
                first = false;
            }

            memcpy(&currentEntry.header, chunk.data(), ChunkSize);
        } else {
            currentEntry.data.append(chunk);
        }
    }

    currentEntry.data = currentEntry.data.sliced(0, currentEntry.header.actualSize());
    m_entries.append(currentEntry);
    currentEntry = TarEntry{};

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

bool Tar::TarArchive::extract(const std::string &name, const std::filesystem::path &destination, std::error_code &errorCode)
{
    if (!fs::is_directory(destination)) {
        errorCode = std::make_error_code(std::errc::not_a_directory);
        return false;
    }

    auto itr = std::ranges::find_if(m_entries, [&name](const TarEntry &entry) {
        return entry.header.name == name;
    });
    if (itr == m_entries.end()) {
        errorCode = std::make_error_code(std::errc::no_such_file_or_directory);
        return false;
    }

    return extractEntry(destination, *itr, errorCode);
}

bool Tar::TarArchive::extractAll(const std::filesystem::path &destination, std::error_code &errorCode)
{
    if (!fs::is_directory(destination)) {
        errorCode = std::make_error_code(std::errc::not_a_directory);
        return false;
    }

    for (const auto &entry : std::as_const(m_entries)) {
        if (!extractEntry(destination, entry, errorCode)) {
            return false;
        }
    }

    return true;
}

bool TarArchive::extractEntry(const std::filesystem::path &destination, const TarEntry &entry, std::error_code &errorCode)
{
    Q_ASSERT(fs::is_directory(destination));

    auto destinationPath = destination / entry.header.name;
    if (fs::exists(destinationPath)) {
        errorCode = std::make_error_code(std::errc::file_exists);
        return false;
    }

    if (entry.header.type == RegularFileType) {
        if (!fs::exists(destinationPath.parent_path())) {
            errorCode = std::make_error_code(std::errc::not_a_directory);
            return false;
        }

        std::ofstream stream(destinationPath, std::ios::binary);
        stream.write(entry.data.data(), entry.data.size());
        stream.close();

        if (stream.fail()) {
            errorCode = std::make_error_code(std::errc::io_error);
            return false;
        } else {
            return true;
        }
    } else if (entry.header.type == DirectoryType) {
        fs::create_directories(destinationPath, errorCode);
        if (errorCode) {
            return false;
        } else {
            return true;
        }
    }

    errorCode = std::make_error_code(std::errc::operation_not_supported);
    return false;
}
