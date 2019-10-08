/*
*   This file is part of Universal-Manager
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz, TotallyNotGuy
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "utils/extract.hpp"

#include <archive.h>
#include <archive_entry.h>


int filesExtracted = 0;
std::string extractingFile = "";

Result extractArchive(std::string archivePath, std::string wantedFile, std::string outputPath)
{
	int r;
	struct archive_entry *entry;

	struct archive *a = archive_read_new();
	archive_read_support_format_all(a);
	archive_read_support_format_raw(a);

	r = archive_read_open_filename(a, archivePath.c_str(), 0x4000);
	if (r != ARCHIVE_OK) {
		return EXTRACT_ERROR_ARCHIVE;
	}

	Result ret = EXTRACT_ERROR_FIND;
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		std::string entryName(archive_entry_pathname(entry));
		if (wantedFile == "/")	wantedFile = "";
		if (matchPattern(wantedFile, entryName.substr(0,wantedFile.length())) || wantedFile == "") {
			extractingFile = (entryName.length() > wantedFile.length() ? entryName.substr(wantedFile.length()) : wantedFile);
			ret = EXTRACT_ERROR_NONE;

			Handle fileHandle;
			std::string outputPathFinal = outputPath;
			if (entryName.length() > wantedFile.length())
				outputPathFinal += entryName.substr(wantedFile.length());
			if (outputPathFinal.substr(outputPathFinal.length()-1) == "/")	continue;
			Result res = openFile(&fileHandle, outputPathFinal.c_str(), true);
			if (R_FAILED(res)) {
				ret = EXTRACT_ERROR_OPENFILE;
				break;
			}

			u64 fileSize = archive_entry_size(entry);
			u32 toRead = 0x4000;
			u8 * buf = (u8 *)malloc(toRead);
			if (buf == NULL) {
				ret = EXTRACT_ERROR_ALLOC;
				FSFILE_Close(fileHandle);
				break;
			}

			u32 bytesWritten = 0;
			u64 offset = 0;
			do {
				if (toRead > fileSize) toRead = fileSize;
				ssize_t size = archive_read_data(a, buf, toRead);
				if (size < 0) {
					ret = EXTRACT_ERROR_READFILE;
					break;
				}

				res = FSFILE_Write(fileHandle, &bytesWritten, offset, buf, toRead, 0);
				if (R_FAILED(res)) {
					ret = EXTRACT_ERROR_WRITEFILE;
					break;
				}

				offset += bytesWritten;
				fileSize -= bytesWritten;
			} while(fileSize);

			FSFILE_Close(fileHandle);
			free(buf);
			filesExtracted++;
		}
	}

	archive_read_free(a);

	return ret;
}