/*
*   This file is part of Universal-Manager
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz
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

#include "screens/screenCommon.hpp"
#include <algorithm>
#include <fstream>
#include <unistd.h>
#include <vector>
//#include "common.hpp"
#include "fileBrowse.h"
#include "keyboard.hpp"
#include "settings.hpp"

extern "C" {
	#include "music/playback.h"
	#include "music/mp3.h"
	#include "C2D_helper.h"
}

struct ButtonPos {
    int x;
    int y;
    int w;
    int h;
	int link;
};

bool imageLoaded = false;

uint selectedFile = 0;
int keyRepeatDelay = 3;
bool dirChanged = true;
std::vector<DirEntry> dirContents;
std::string scanDir;
std::string currentSong = "";
int musicRepeat = 0;
bool musicShuffle = 0;

int musicPlayerReturn = musicMainScreen;
bool firstSong = true;
int locInPlaylist = 0;
uint selectedPlst = 0;
std::vector<DirEntry> plsts;
std::vector<Playlist> nowPlayingList;
bool decForRepeat2 = false;

uint selectedPlstItem = 0;
int movingPlstItem = -1;
std::vector<std::string> plstContents;


extern bool touching(touchPosition touch, ButtonPos button);

static C2D_Image musicImage;

static bool Draw_Image(void)
{
	return C2D_DrawImageAt(musicImage, 0, 25, 0.5);
}

ButtonPos mainButtonPos[] = {
    {0, 40, 149, 52, musicListScreen},
    {170, 40, 149, 52, musicPlayerScreen},
    {0, 150, 149, 52, musicPlaylistPlayScreen},
	{170, 150, 149, 52, themeSelectorScreen},
};

ButtonPos playerButtonPos[] = {
	{130, 90, 60, 60, -1},
	{80, 100, 40, 40, -1},
	{200, 100, 40, 40, -1},
	{240, 170, 30, 30, -1},
	{280, 170, 30, 30, -1},
};

std::string secondsToString(u64 seconds) {
    int h = 0, m = 0, s = 0;
    h = (seconds / 3600);
    m = (seconds - (3600 * h)) / 60;
    s = (seconds - (3600 * h) - (m * 60));

	char string[35];

    if (h > 0) snprintf(string, sizeof(string), "%02d:%02d:%02d", h, m, s);
    else	snprintf(string, sizeof(string), "%02d:%02d", m, s);
	
	return string;
}

void drawMusicMain() {
	// Theme Stuff.
	Gui::DrawBGTop();
	animatedBGTop();
	Gui::chooseLayoutTop();
	DisplayTime();
	drawBatteryTop();
	Draw_Text(110, 0, FONT_SIZE_18, WHITE, "Music Player Menu");
	Gui::DrawBGBot();
	animatedBGBot();
	Gui::chooseLayoutBot();

	Gui::sprite(sprites_mainMenuButton_idx, mainButtonPos[0].x, mainButtonPos[0].y);
	Gui::sprite(sprites_music_icon_idx, mainButtonPos[0].x+5, mainButtonPos[0].y+10);
	Draw_Text(40, 57, 0.7f, WHITE, "Songs");

	Gui::sprite(sprites_mainMenuButton_idx, mainButtonPos[1].x, mainButtonPos[1].y);
	Gui::Draw_ImageBlend(sprites_play_icon_small_glow_idx, mainButtonPos[1].x+5, mainButtonPos[1].y+10, Config::barColor);
	Gui::sprite(sprites_play_icon_small_normal_idx, mainButtonPos[1].x+5, mainButtonPos[1].y+10);
	Draw_Text(210, 57, 0.65f, WHITE, "Now Playing");

	Gui::sprite(sprites_mainMenuButton_idx, mainButtonPos[2].x, mainButtonPos[2].y);
	Gui::sprite(sprites_playlist_icon_idx, mainButtonPos[2].x+1, mainButtonPos[2].y+6);
	Draw_Text(37, 167, 0.65f, WHITE, "Playlists");

	 Gui::sprite(sprites_mainMenuButton_idx, mainButtonPos[3].x, mainButtonPos[3].y);
	 Gui::sprite(sprites_themes_idx, mainButtonPos[3].x+5, mainButtonPos[3].y+10);
	 Draw_Text(220, 167, 0.7f, WHITE, "Themes");
}

void musicMainLogic(u32 hDown, touchPosition touch) {
	if(hDown & KEY_B) {
		screenMode = mainScreen;
	} else if(hDown & KEY_TOUCH) {
		for(uint i=0;i<(sizeof(mainButtonPos)/sizeof(mainButtonPos[0]));i++) {
			if (touching(touch, mainButtonPos[i])) {
				screenMode = mainButtonPos[i].link;
				if(mainButtonPos[i].link == musicPlayerScreen) {
					musicPlayerReturn = musicMainScreen;
				} else if(mainButtonPos[i].link == musicPlaylistPlayScreen) {
					selectedPlst = 0;
					dirChanged = true;
				}
			}
		}
	}
}

void drawMusicList(void) {
	drawFileBrowser("Music Player Menu");
}

void musicListLogic(u32 hDown, u32 hHeld) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	gspWaitForVBlank();
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
			screenMode = musicMainScreen;
		} else {
		chdir("..");
		selectedFile = 0;
		dirChanged = true;
		}
	} else if (hDown & KEY_Y) {
		dirChanged = true;
		screenMode = musicPlaylistAddScreen;
	} else if (hDown & KEY_X) {
		screenMode = musicMainScreen;
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
	} else if (hDown & KEY_SELECT) {
	screenMode = musicPlayerScreen;
}
} 

void drawMusicPlayer(void) {

	Gui::DrawBGTop();
	animatedBGTop();
	Gui::chooseLayoutTop();

		// Placeholder for Metadata Stuff..
	 	if (Config::musicMode == 0) {
	} else if (Config::musicMode == 1) {
			if ((metadata.has_meta) && (metadata.title[0] != '\0') && (metadata.artist[0] != '\0') && (metadata.album[0] != '\0') && (metadata.year[0] != '\0')) {
			Draw_Text(15, 40, 0.5f, WHITE, strupr(metadata.title));
			Draw_Text(15, 60, 0.45f, WHITE, strupr(metadata.artist));
			Draw_Text(15, 80, 0.45f, WHITE, strupr(metadata.album));
			Draw_Text(15, 100, 0.45f, WHITE, strupr(metadata.year));
		} else {
			Draw_Text(15, 40, 0.5f, WHITE, "No Metadata Found.");
		}
	} else if (Config::musicMode == 2) {
		if (imageLoaded == true) {
		Draw_Image(); 
	} else if (imageLoaded == false) {
			Draw_Text(15, 40, 0.5f, WHITE, "Please load an Image under `Themes`.");
	}
	}
	
	if(isPlaying()) {
		std::string nowPlayingText = "Current Song: " + currentSong.substr(currentSong.find_last_of("/")+1);
		Draw_Text(0, 0, 0.50f, WHITE, nowPlayingText.c_str());
		Draw_Rect(155, 164, 85, 10, GRAY);
		Draw_Text(160, 162, 0.45f, WHITE, (secondsToString(Audio_GetPosition()/Audio_GetRate()) + " / " + secondsToString(Audio_GetLength()/Audio_GetRate())).c_str());
		if (Audio_GetPosition() != -1) {
		Draw_Rect(100, 179, 207, 16, GRAY);
		Draw_Rect(100, 179, (((double)Audio_GetPosition()/(double)Audio_GetLength()) * 207.0), 16, Config::barColor);
		}

	
	if(!isPaused() && isPlaying()) {
		Draw_Text(80, 220, 0.45f, WHITE, "A : Pause   B : Back   X : Stop song");
	} else if(isPaused() && isPlaying()) {
		Draw_Text(80, 220, 0.45f, WHITE, "A : Play   B : Back   X : Stop song");
	} else {
		Draw_Text(0, 0, 0.72f, WHITE, "No Song Selected.");
		Draw_Text(80, 220, 0.45f, WHITE, "B : Back");
	}


	Gui::DrawBGBot();
	animatedBGBot();
	Gui::chooseLayoutBot();
	DisplayTime();
	drawBatteryBot();

	Gui::Draw_ImageBlend(!isPaused() ? sprites_pause_icon_glow_idx : sprites_play_icon_glow_idx, playerButtonPos[0].x, playerButtonPos[0].y, Config::barColor);
	Gui::sprite(!isPaused() ? sprites_pause_icon_normal_idx : sprites_play_icon_normal_idx, playerButtonPos[0].x, playerButtonPos[0].y);

	Gui::Draw_ImageBlend(sprites_left_arrow_glow_idx, playerButtonPos[1].x, playerButtonPos[1].y, Config::barColor);
	Gui::sprite(sprites_left_arrow_normal_idx, playerButtonPos[1].x, playerButtonPos[1].y);

	Gui::Draw_ImageBlend(sprites_right_icon_glow_idx, playerButtonPos[2].x, playerButtonPos[2].y, Config::barColor);
	Gui::sprite(sprites_right_icon_normal_idx, playerButtonPos[2].x, playerButtonPos[2].y);

	Gui::Draw_ImageBlend(sprites_shuffle_icon_idx, playerButtonPos[3].x, playerButtonPos[3].y, (musicShuffle ? WHITE : Config::barColor));
	Gui::Draw_ImageBlend(sprites_repeat_icon_idx, playerButtonPos[4].x, playerButtonPos[4].y, (musicRepeat ? WHITE : Config::barColor));
	if (musicRepeat)	Draw_Text(playerButtonPos[4].x+11, playerButtonPos[4].y+9, 0.5f, BLACK, (musicRepeat == 1 ? "A" : "S"));
}
}

bool playlistSortPredicate(const Playlist &lhs, const Playlist &rhs) {
	if(lhs.position < rhs.position)	return true;
	else	return false;
}

bool playlistShufflePredicate(const Playlist &lhs, const Playlist &rhs) {
	return rand() % 2;
}
void musicPlayerLogic(u32 hDown, touchPosition touch) {
	if (hDown & KEY_A) {
		togglePlayback();
	} else if (hDown & KEY_X) {
		stopPlayback();
	} else if (hDown & KEY_B) {
		screenMode = musicPlayerReturn;
	} else if (hDown & KEY_TOUCH) {
		if (touching(touch, playerButtonPos[0])) {
			togglePlayback();
		} else if (touching(touch, playerButtonPos[1])) {
			goto prevSong;
		} else if (touching(touch, playerButtonPos[2])) {
			goto nextSong;
		} else if (touching(touch, playerButtonPos[3])) {
			goto toggleShuffle;
		} else if (touching(touch, playerButtonPos[4])) {
			goto toggleRepeat;
		}
	} else if (hDown & KEY_LEFT) {
		prevSong:
		if(locInPlaylist > 0) {
			locInPlaylist--;
		} else if (nowPlayingList.size() > 0) {
			locInPlaylist = nowPlayingList.size() - 1;
		}
		currentSong = nowPlayingList[locInPlaylist].name;
		playbackInfo_t playbackInfo;
		changeFile(currentSong.c_str(), &playbackInfo);
	} else if (hDown & KEY_RIGHT) {
		nextSong:
		if(locInPlaylist < (int)nowPlayingList.size()-1) {
			locInPlaylist++;
		} else {
			locInPlaylist = 0;
		}
		currentSong = nowPlayingList[locInPlaylist].name;
		playbackInfo_t playbackInfo;
		changeFile(currentSong.c_str(), &playbackInfo);
	} else if (hDown & KEY_START) {
		toggleRepeat:
		if (musicRepeat < 2 ) {
			musicRepeat++;
		} else {
			musicRepeat = 0;
		}
	} else if (hDown & KEY_SELECT) {
		toggleShuffle:
		musicShuffle = !musicShuffle;
		std::sort(nowPlayingList.begin(), nowPlayingList.end(), musicShuffle ?  playlistShufflePredicate : playlistSortPredicate);
	}
	aptSetSleepAllowed(false);
}

void drawMusicPlaylistAdd(void) {
	// Theme Stuff.
	Gui::DrawBGTop();
	animatedBGTop();
	Gui::chooseLayoutTop();
	DisplayTime();
	drawBatteryTop();
	Draw_Text(100, 0, FONT_SIZE_18, WHITE, "Music Playlist Menu");
	mkdir("sdmc:/Universal-Manager/playlists/", 0777);
	
	if(dirChanged) {
		char startPath[PATH_MAX];
		getcwd(startPath, PATH_MAX);
		chdir("sdmc:/Universal-Manager/playlists/");
		getDirectoryContents(plsts);
		chdir(startPath);
		DirEntry dirEntry;
		dirEntry.name = "Now Playing";
		dirEntry.isDirectory = false;
		plsts.insert(plsts.begin(), dirEntry);
		dirChanged = false;
	}

	std::string plstList;
	std::string plstList2;
	for (uint i=(selectedPlst<12) ? 0 : selectedPlst-12;i<plsts.size()&&i<((selectedPlst<12) ? 13 : selectedPlst+1);i++) {
		if (i == selectedPlst) {
			plstList += "> " + plsts[i].name.substr(0, plsts[i].name.find_last_of(".")) + "\n";
		} else {
			plstList += "  " + plsts[i].name.substr(0, plsts[i].name.find_last_of(".")) + "\n";
		}
	}
	for (uint i=0;i<((plsts.size()<13) ? 13-plsts.size() : 0);i++) {
		plstList += "\n";
	}
	plstList2 += "\n\uE000 : Add to "+plsts[selectedPlst].name+"B : Back   X : Delete   Y : New";
	Draw_Text(170, 32, 0.45f, WHITE, plstList.c_str());
	Draw_Text(26, 220, 0.45f, WHITE, plstList2.c_str());

	Gui::DrawBGBot();
	animatedBGBot();
	Gui::chooseLayoutBot();
}

void musicPlaylistAddLogic(u32 hDown, u32 hHeld) {
	if(keyRepeatDelay)	keyRepeatDelay--;
	gspWaitForVBlank();
	if(hDown & KEY_A) {
		std::vector<DirEntry> selectedFiles;
		if(dirContents[selectedFile].isDirectory) {
			char startPath[PATH_MAX];
			getcwd(startPath, PATH_MAX);
			chdir(dirContents[selectedFile].name.c_str());
			getDirectoryContents(selectedFiles);
			chdir(startPath);
		} else {
			DirEntry dirEntry;
			dirEntry.name = "";
			selectedFiles.push_back(dirEntry);
		}
		for(uint i=0;i<selectedFiles.size();i++) {
			if(selectedPlst == 0) {
				Playlist playlist;
				char path[PATH_MAX];
				getcwd(path, PATH_MAX);
				playlist.name = path + dirContents[selectedFile].name + "/" + selectedFiles[i].name;
				playlist.position = nowPlayingList.size() + 1;
				nowPlayingList.push_back(playlist);
			} else {
				char path[PATH_MAX];
				getcwd(path, PATH_MAX);
				FILE* plst = fopen(("sdmc:/Universal-Manager/playlists/"+plsts[selectedPlst].name).c_str(), "a");
				fputs((path+dirContents[selectedFile].name+(dirContents[selectedFile].isDirectory?"/"+selectedFiles[i].name:"")+"\n").c_str(), plst);
				fclose(plst);
			}
		}
		screenMode = musicListScreen;
	} else if (hDown & KEY_B) {
		screenMode = musicListScreen;
	} else if (hDown & KEY_Y) {
		std::string newPlaylist = Input::getLine();
		if(newPlaylist != "") {
			FILE* plst = fopen(("sdmc:/Universal-Manager/playlists/"+newPlaylist+".plst").c_str(), "w");
			fclose(plst);
			dirChanged = true;
		}
	} else if (hDown & KEY_X) {
		if (selectedPlst != 0) {
			if(confirmPopup("Are you sure you want to delete this playlist?")) {
				remove(("sdmc:/Universal-Manager/playlists/"+plsts[selectedPlst].name).c_str());
				dirChanged = true;
			}
		}
	} else if (hHeld & KEY_UP) {
		if (selectedPlst > 0 && !keyRepeatDelay) {
			selectedPlst--;
			playScrollSfx();
			keyRepeatDelay = 3;
		}
	} else if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selectedPlst < plsts.size()-1) {
			selectedPlst++;
			playScrollSfx();
			keyRepeatDelay = 3;
		}
	}
}

void drawMusicPlaylistPlay(void) {
	// Theme Stuff.
	Gui::DrawBGTop();
	animatedBGTop();
	Gui::chooseLayoutTop();
	DisplayTime();
	drawBatteryTop();
	Draw_Text(100, 0, FONT_SIZE_18, WHITE, "Music Playlist Menu");
	mkdir("sdmc:/Universal-Manager/playlists/", 0777);
	
	if(dirChanged) {
		char startPath[PATH_MAX];
		getcwd(startPath, PATH_MAX);
		chdir("sdmc:/Universal-Manager/playlists/");
		getDirectoryContents(plsts);
		chdir(startPath);
	}

	std::string plstList;
	std::string plstList2;
	for (uint i=(selectedPlst<12) ? 0 : selectedPlst-12;i<plsts.size()&&i<((selectedPlst<12) ? 13 : selectedPlst+1);i++) {
		if (i == selectedPlst) {
			plstList += "> " + plsts[i].name.substr(0, plsts[i].name.find_last_of(".")) + "\n";
		} else {
			plstList += "  " + plsts[i].name.substr(0, plsts[i].name.find_last_of(".")) + "\n";
		}
	}
	for (uint i=0;i<((plsts.size()<13) ? 13-plsts.size() : 0);i++) {
		plstList += "\n";
	}
	plstList2 += "A : Play   B : Back   X : Delete   Y : Edit";
	Draw_Text(26, 32, 0.45f, WHITE, plstList.c_str());
	Draw_Text(26, 220, 0.45f, WHITE, plstList2.c_str());

	Gui::DrawBGBot();
	animatedBGBot();
	Gui::chooseLayoutBot();
}

void musicPlaylistPlayLogic(u32 hDown, u32 hHeld) {
	if(keyRepeatDelay)	keyRepeatDelay--;
	gspWaitForVBlank();
	if(hDown & KEY_A) {
		if(confirmPopup("Would you like to add to these songs to",
						 "Now Playing or play them now?",
						 "Play Now",
						 "Add to Now Playing",
						 100)) {
			nowPlayingList.clear();
		}
		std::ifstream plst("sdmc:/Universal-Manager/playlists/"+plsts[selectedPlst].name);
		if(plst) {
			Playlist song;
			int i = nowPlayingList.size() + 1;
			while(std::getline(plst, song.name)) {
				song.position = i++;
				nowPlayingList.push_back(song);
			}
		}
	} else if (hDown & KEY_B) {
		screenMode = musicMainScreen;
	} else if (hDown & KEY_X) {
		if(confirmPopup("Are you sure you want to delete this playlist?")) {
			remove(("sdmc:/Universal-Manager/playlists/"+plsts[selectedPlst].name).c_str());
			dirChanged = true;
		}
	} else if (hDown & KEY_Y) {
		std::string temp;
		std::ifstream file("sdmc:/Universal-Manager/playlists/"+plsts[selectedPlst].name);
		plstContents.clear();
		while(std::getline(file, temp)) {
			plstContents.push_back(temp);
		}
		selectedPlstItem = 0;
		screenMode = musicPlaylistEditScreen;
	} else if (hHeld & KEY_UP) {
		if (selectedPlst > 0 && !keyRepeatDelay) {
			selectedPlst--;
			playScrollSfx();
			keyRepeatDelay = 3;
		}
	} else if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selectedPlst < plsts.size()-1) {
			selectedPlst++;
			playScrollSfx();
			keyRepeatDelay = 3;
		}
	}
}

void drawMusicPlaylistEdit() {
	Gui::DrawBGTop();
	animatedBGTop();
	Gui::chooseLayoutTop();
	DisplayTime();
	drawBatteryTop();
	Draw_Text(100, 0, FONT_SIZE_18, WHITE, "Music Playlist Menu");

	std::string plstList;
	std::string plstList2;
	for (uint i=(selectedPlstItem<12) ? 0 : selectedPlstItem-12;i<plstContents.size()&&i<((selectedPlstItem<12) ? 13 : selectedPlstItem+1);i++) {
		if (i == selectedPlstItem) {
			plstList += "> " + plstContents[i].substr(plstContents[i].find_last_of("/")+1) + "\n";
		} else if ((int)i == movingPlstItem) {
			plstList += "x " + plstContents[i].substr(plstContents[i].find_last_of("/")+1) + "\n";
		} else {
			plstList += "  " + plstContents[i].substr(plstContents[i].find_last_of("/")+1) + "\n";
		}
	}
	for (uint i=0;i<((plstContents.size()<13) ? 13-plstContents.size() : 0);i++) {
		plstList += "\n";
	}
	plstList2 += "A : Save   B : Back   X : Delete   Y : Move";
	Draw_Text(26, 32, 0.45f, WHITE, plstList.c_str());
	Draw_Text(26, 220, 0.45f, WHITE, plstList2.c_str());
	Gui::DrawBGBot();
	animatedBGBot();
	Gui::chooseLayoutBot();
}

void musicPlaylistEditLogic(u32 hDown, u32 hHeld) {
	if(keyRepeatDelay)	keyRepeatDelay--;
	gspWaitForVBlank();
	if(hDown & KEY_A) {
		FILE* plst = fopen(("sdmc:/Universal-Manager/playlists/"+plsts[selectedPlst].name).c_str(), "w");
		for(uint i=0;i<plstContents.size();i++) {
			fputs((plstContents[i]+"\n").c_str(), plst);
		}
		fclose(plst);
		screenMode = musicPlaylistPlayScreen;
	} else if(hDown & KEY_B) {
		screenMode = musicPlaylistPlayScreen;
	} else if (hDown & KEY_X) {
		if(confirmPopup("Are you sure you want to remove this song?")) {
			plstContents.erase(plstContents.begin()+selectedPlstItem);
			dirChanged = true;
		}
	} else if (hDown & KEY_Y) {
		if(movingPlstItem == -1) {
			movingPlstItem = selectedPlstItem;
		} else {
			std::string item = plstContents[movingPlstItem];
			plstContents.erase(plstContents.begin()+movingPlstItem);
			plstContents.insert(plstContents.begin()+selectedPlstItem, item);
			movingPlstItem = -1;
		}
	} else if (hHeld & KEY_UP) {
		if (selectedPlstItem > 0 && !keyRepeatDelay) {
			selectedPlstItem--;
			playScrollSfx();
			keyRepeatDelay = 3;
		}
	} else if (hHeld & KEY_DOWN && !keyRepeatDelay) {
		if (selectedPlstItem < plstContents.size()-1) {
			selectedPlstItem++;
			playScrollSfx();
			keyRepeatDelay = 3;
		}
	}
}


 void drawThemeSelector(void) {
	drawFileBrowser("Theme Selector");
}

void themeSelectorLogic(u32 hDown, u32 hHeld) {
	if (keyRepeatDelay)	keyRepeatDelay--;
	gspWaitForVBlank();
	if (hDown & KEY_A) {
		if (dirContents[selectedFile].isDirectory) {
			chdir(dirContents[selectedFile].name.c_str());
			selectedFile = 0;
			dirChanged = true;
		} else {
			if(dirContents[selectedFile].name != currentSong) {
			}
			if (Config::musicMode == 0) {
			DisplayMsg("Put the Music Mode to `BG`.");
			for (int i = 0; i < 60*4; i++) {
			gspWaitForVBlank();
			}
		} else if (Config::musicMode == 1) {
			DisplayMsg("Put the Music Mode to `BG`.");
			for (int i = 0; i < 60*4; i++) {
			gspWaitForVBlank();
			}
		} else if (Config::musicMode == 2) {
			if(confirmPopup("Do you want, to use this Image?")) {
			Draw_LoadImageFile(&musicImage, dirContents[selectedFile].name.c_str());
			imageLoaded = true;
			}
		}
		}
	} else if (hDown & KEY_B) {
		char path[PATH_MAX];
		getcwd(path, PATH_MAX);
		if(strcmp(path, "sdmc:/") == 0 || strcmp(path, "/") == 0) {
			screenMode = musicMainScreen;
		} else {
		chdir("..");
		selectedFile = 0;
		dirChanged = true;
		}
	} else if(hDown & KEY_X) {
		screenMode = musicMainScreen;
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
	}
}