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

#include "screenCommon.hpp"
#include "musicListScreen.hpp"
#include "settings.hpp"
#include <algorithm>
#include <fstream>
#include <unistd.h>
#include "sound.h"

#include "playlistAddScreen.hpp"

extern "C" {
	#include "music/error.h"
	#include "music/playback.h"
}

void MusicList::Draw(void) const
{
	Gui::DrawBGTop();
	animatedBGTop();
	Gui::DrawBarsTop();
	DisplayTime();
	drawBatteryTop();
	char path[PATH_MAX];
	getcwd(path, PATH_MAX);
	Draw_Text((400-(Draw_GetTextWidth(0.68f, path)))/2, 0, 0.68f, WHITE, path);
	std::string dirs;
	for (uint i=(selectedFile<5) ? 0 : selectedFile-5;i<dirContents.size()&&i<((selectedFile<5) ? 6 : selectedFile+1);i++) {
		(i == selectedFile);

		if (selectedFile == 0) {
			Gui::sprite(sprites_selected_idx, 0, 25);
			dirs +=  dirContents[i].name + "\n\n";

		} else if (selectedFile == 1) {
			Gui::sprite(sprites_selected_idx, 0, 56);
			dirs +=  dirContents[i].name + "\n\n";

		} else if (selectedFile == 2) {
			Gui::sprite(sprites_selected_idx, 0, 91);
			dirs +=  dirContents[i].name + "\n\n";

		} else if (selectedFile == 3) {
			Gui::sprite(sprites_selected_idx, 0, 125);
			dirs +=  dirContents[i].name + "\n\n";

		} else if (selectedFile == 4) {
			Gui::sprite(sprites_selected_idx, 0, 160);
			dirs +=  dirContents[i].name + "\n\n";

		} else if (selectedFile == 5) {
			Gui::sprite(sprites_selected_idx, 0, 190);
			dirs +=  dirContents[i].name + "\n\n";
		} else {
			Gui::sprite(sprites_selected_idx, 0, 190);
			dirs +=  dirContents[i].name + "\n\n";
		}
	}
	for (uint i=0;i<((dirContents.size()<6) ? 6-dirContents.size() : 0);i++) {
		dirs += "\n\n";
	}
	Draw_Text(26, 32, 0.53f, WHITE, dirs.c_str());

	Gui::DrawBGBot();
	animatedBGBot();
	Gui::DrawBarsBot();
}

void MusicList::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	gspWaitForVBlank();

			if (dirChanged) {
            dirContents.clear();
            std::vector<DirEntry> dirContentsTemp;
            getDirectoryContents(dirContentsTemp);
            for(uint i=0;i<dirContentsTemp.size();i++) {
                  dirContents.push_back(dirContentsTemp[i]);
        }
		dirChanged = false;
	}


	if (hDown & KEY_A) {
		if (dirContents[selectedFile].isDirectory) {
			chdir(dirContents[selectedFile].name.c_str());
			selectedFile = 0;
			dirChanged = true;
		} else {
			if ((strcasecmp(dirContents[selectedFile].name.substr(dirContents[selectedFile].name.length()-3, 3).c_str(), "mp3") == 0) ||
			(strcasecmp(dirContents[selectedFile].name.substr(dirContents[selectedFile].name.length()-3, 3).c_str(), "wav") == 0) ||
			(strcasecmp(dirContents[selectedFile].name.substr(dirContents[selectedFile].name.length()-3, 3).c_str(), "ogg") == 0)) {
			if(dirContents[selectedFile].name != currentSong) {
				nowPlayingList.clear();
				char path[PATH_MAX];
				getcwd(path, PATH_MAX);
				currentSong = path + dirContents[selectedFile].name;
				Playlist song;
				song.name = currentSong;
				song.position = nowPlayingList.size() + 1;
				nowPlayingList.push_back(song);
				playbackInfo_t playbackInfo;
				changeFile(dirContents[selectedFile].name.c_str(), &playbackInfo);
			}
			togglePlayback(); // Since it would otherwise pause it in main.cpp
		} else {
			DisplayMsg("This is not a valid Music File!\nThe supported Formats are :\nMP3, WAV and OGG.");
			for (int i = 0; i < 60*2; i++) {
				gspWaitForVBlank();
			}
		}
		}
	} else if (hDown & KEY_B) {
		char path[PATH_MAX];
		getcwd(path, PATH_MAX);
		if(strcmp(path, "sdmc:/") == 0 || strcmp(path, "/") == 0) {
			Gui::screenBack();
			return;
		} else {
		chdir("..");
		selectedFile = 0;
		dirChanged = true;
		}
	} else if (hDown & KEY_Y) {
		dirChanged = true;
		Gui::setScreen(std::make_unique<PlaylistAdd>());
	} else if (hDown & KEY_X) {
			Gui::screenBack();
			return;
	} else if (hHeld & KEY_UP) {
		if (selectedFile > 0 && !keyRepeatDelay) {
			selectedFile--;
			playScrollSfx();
			keyRepeatDelay = 3;
		}
	} else if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selectedFile < dirContents.size()-1) {
			selectedFile++;
			playScrollSfx();
			keyRepeatDelay = 3;
		}
//	} else if (hDown & KEY_START) {
//		Gui::setScreen(std::make_unique<MUSICPLAYER>());
	}
}