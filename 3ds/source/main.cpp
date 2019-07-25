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
#include <citro3d.h>
#include <citro2d.h>
#include <3ds.h>
#include <algorithm>
#include <dirent.h>
#include <malloc.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "gui.hpp"
#include "screenCommon.hpp"
#include "settings.hpp"
#include "ptmu_x.h"
#include "sound.h"

extern "C" {
	#include "music/error.h"
	#include "music/playback.h"
}

struct ButtonPos {
    int x;
    int y;
    int w;
    int h;
	int link;
};

extern std::string currentSong;
extern std::vector<Playlist> nowPlayingList;
extern int locInPlaylist;
extern int musicRepeat;
extern bool musicShuffle;
extern bool firstSong;

//Music and sound effects.
sound *sfx_scroll = NULL;
sound *sfx_pong = NULL;
sound *sfx_score = NULL;

int SCREEN_MODE;
bool dspfirmfound = false;

static touchPosition touch;
extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;

ButtonPos uisettingsScreenButtonPos[] = {
    {293, 213, 27, 27, mainScreen},
};

ButtonPos updaterScreenButtonPos[] = {
    {293, 213, 27, 27, mainScreen},
};



static void loadSoundEffects(void) {
	sfx_scroll = new sound("romfs:/sfx/scroll.wav", 2, false);
	sfx_pong = new sound("romfs:/sfx/pong.wav", 2, false);
	sfx_score = new sound("romfs:/sfx/score.wav", 2, false);
}

bool touching(touchPosition touch, ButtonPos button) {
	if (touch.px >= button.x && touch.px <= (button.x + button.w) && touch.py >= button.y && touch.py <= (button.y + button.h))
		return true;
	else
		return false;
}

int fadealpha = 255;
bool fadein = true;



int main()
{
	acInit();
	amInit();
	ptmuInit();	// For battery status
	ptmuxInit();	// For AC adapter status
	sdmcInit();
	Config::loadConfig();
	if (Config::Citra == 0) {
	mcuInit();
	} else if (Config::Citra == 1) {
	}
	romfsInit();
	cfguInit();
    gfxInitDefault();
	Gui::init();
	srand(time(NULL));

	if (Config::Credits == 0) { // Credits Screen if 1 and mainScreen if 0.
		SCREEN_MODE = 0;
	} else if (Config::Credits == 1) {
		SCREEN_MODE = 1;
	}

	osSetSpeedupEnable(true);	// Enable speed-up for New 3DS users


	// make folders if they don't exist
	mkdir("sdmc:/3ds", 0777);	// For DSP dump
	mkdir("sdmc:/Universal-Manager", 0777); // main Path.
	mkdir("sdmc:/Universal-Manager/Screenshots", 0777); // Create the Screenshot Path.

 	if( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
		ndspInit();
		dspfirmfound = true;
	 }
	 
	loadSoundEffects();

	// Loop as long as the status is not exit
    while (aptMainLoop())
    {
        hidScanInput();
        u32 hHeld = hidKeysHeld();
        u32 hDown = hidKeysDown();
		hidTouchRead(&touch);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, BLUE2);
        C2D_TargetClear(bottom, BLUE2);
		Gui::clearTextBufs();	

		// Draws a screen based on screenMode
		switch(SCREEN_MODE) {
//#########################################################################################################
			case mainScreen:
				drawMainMenu();
				break;
//#########################################################################################################
			case creditsScreen:
				drawCreditsScreen();
				break;
//#########################################################################################################
			case musicMainScreen:
				drawMusicMain();
				break;
			case musicListScreen:
				drawMusicList();
				break;
			case musicPlayerScreen:
				drawMusicPlayer();
				break;
			case musicPlaylistAddScreen:
				drawMusicPlaylistAdd();
				break;
			case musicPlaylistPlayScreen:
				drawMusicPlaylistPlay();
				break;
			case musicPlaylistEditScreen:
				drawMusicPlaylistEdit();
				break;
			case themeSelectorScreen:
				drawThemeSelector();
				break;
//#########################################################################################################
			case SettingsScreen:
				drawSettingsScreen();
				break;
//#########################################################################################################
			case ImageSelectorScreen:
				drawImageSelectorScreen();
				break;
			case showImageScreen:
				showImage();
				break;
//#########################################################################################################
			case ftpScreen:
				drawFTPScreen();
				break;
//#########################################################################################################
			case updaterSubMenu:
				drawUpdaterSubMenu();
				break;
			case TWLScreen:
				drawUpdaterTWL();
				break;
			case OtherScreen:
				drawUpdaterOther();
				break;
			case CFWScreen:
				drawUpdaterCFW();
				break;
			case UniversalScreen:
				drawUniversalScreen();
				break;
//#########################################################################################################
			case fileManager:
				drawFileBrowse();
				break;
//#########################################################################################################
			case scriptMainScreen:
				drawScriptMainScreen();
				break;
			case scriptCreatorFunctions:
				drawScriptsCreatorFunctions();
				break;
//#########################################################################################################
			case TextEditorScreen:
				drawTextEditorScreen();
				break;
			case textFileBrowse:
				drawTextFileBrowse();
				break;
//#########################################################################################################
			case buttonTesterScreen:
				drawButtonTesterScreen();
				break;
//#########################################################################################################
			case calendarScreen:
				drawCalendarScreen();
				break;
//#########################################################################################################
			case gameSubMenuScreen:
				drawGamesSubMenuScreen();
				break;
//#########################################################################################################
			case pongScreen:
				drawPongScreen();
				break;
//#########################################################################################################
			case tictactoeScreen:
				drawTicTacToeScreen();
				break;
//#########################################################################################################
			case utilsScreen:
				drawUtilsScreen();
				break;
//#########################################################################################################
			case calculatorScreen:
				drawCalculatorScreen();
				break;
		}

		// Scans inputs for the current screen
		switch(SCREEN_MODE) {
//#########################################################################################################
			case mainScreen:
				MainMenuLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case creditsScreen:
				creditsLogic(hDown, touch);
				break;
//#########################################################################################################
			case musicMainScreen:
				musicMainLogic(hDown, hHeld, touch);
				break;
			case musicListScreen:
				musicListLogic(hDown, hHeld);
				break;
			case musicPlayerScreen:
				musicPlayerLogic(hDown, hHeld, touch);
				break;
			case musicPlaylistAddScreen:
				musicPlaylistAddLogic(hDown, hHeld);
				break;
			case musicPlaylistPlayScreen:
				musicPlaylistPlayLogic(hDown, hHeld);
				break;
			case musicPlaylistEditScreen:
				musicPlaylistEditLogic(hDown, hHeld);
				break;
			case themeSelectorScreen:
				themeSelectorLogic(hDown, hHeld);
				break;
//#########################################################################################################
			case SettingsScreen:
				SettingsLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case ImageSelectorScreen:
			ImageSelectorLogic(hDown, hHeld);
				break;
			case showImageScreen:
			showImageLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case ftpScreen:
				break;
//#########################################################################################################
			case updaterSubMenu:
				updaterSubMenuLogic(hDown, hHeld, touch);
				break;
			case TWLScreen:
				updaterTWLLogic(hDown, hHeld, touch);
				break;
			case OtherScreen:
				updaterOtherLogic(hDown, hHeld, touch);
				break;
			case CFWScreen:
				updaterCFWLogic(hDown, hHeld, touch);
				break;
			case UniversalScreen:
				UniversalLogic(hDown, hHeld, touch);
				break;
//########################################################################################################
			case fileManager:
				fileManagerLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case scriptMainScreen:
				scriptMainScreenLogic(hDown, hHeld);
				break;
			case scriptCreatorFunctions:
				scriptCreatorFunctionsLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case TextEditorScreen:
				TextEditorLogic(hDown, hHeld);
				break;
			case textFileBrowse:
				textFileBrowseLogic(hDown, hHeld);
				break;
//#########################################################################################################
			case buttonTesterScreen:
				buttonTesterLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case calendarScreen:
				calendarLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case gameSubMenuScreen:
				gamesSubMenuLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case pongScreen:
				pongLogic(hDown, hHeld);
				break;
//#########################################################################################################
			case tictactoeScreen:
				ticTacToeLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case utilsScreen:
				utilsLogic(hDown, hHeld, touch);
				break;
//#########################################################################################################
			case calculatorScreen:
				calculatorLogic(hDown, hHeld, touch);
				break;
		}
//#########################################################################################################
		if (!isPlaying() && ((int)nowPlayingList.size()-1 > locInPlaylist || ((int)nowPlayingList.size() > 0 && musicRepeat))) {
			if (locInPlaylist > (int)nowPlayingList.size()-2 && musicRepeat != 2)	locInPlaylist = -1;
			if (musicRepeat != 2 && !firstSong) {
				locInPlaylist++;
			}
			firstSong = false;
			currentSong = nowPlayingList[locInPlaylist].name;
			playbackInfo_t playbackInfo;
			changeFile(currentSong.c_str(), &playbackInfo);
		} else if (isPlaying() && currentSong == "") {
			stopPlayback();
		} else if (!isPlaying() && currentSong != "") {
			currentSong = "";
		}
		if (hDown & KEY_START && SCREEN_MODE == mainScreen) 
		{
			break;
		}
        C3D_FrameEnd(0);
        Gui::clearTextBufs();

		if (fadein == true) {
			fadealpha -= 3;
			if (fadealpha < 0) {
				fadealpha = 0;
				fadein = false;
			}
		}
    }

	Config::saveConfig();
	delete sfx_scroll;
	delete sfx_pong;
	delete sfx_score;
	if (isPlaying()) {
	stopPlayback(); // This seems to do `ndspExit();` already. I hope the Crash is finally fixed?
	} else if (!isPlaying()) {
		ndspExit();
	}
	
	Gui::exit();
	gfxExit();
	cfguExit();
	romfsExit();
	sdmcExit();
	acExit();
	amExit();

    return 0;
}
