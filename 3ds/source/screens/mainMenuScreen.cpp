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

#include "screens/creditsScreen.hpp"
#include "screens/fileManagerScreen.hpp"
#include "screens/ftpScreen.hpp"
#include "screens/gameSubMenu.hpp"
#include "screens/imageScreen.hpp"
#include "screens/mainMenuScreen.hpp"
#include "screens/musicScreen.hpp"
#include "screens/paint.hpp"
#include "screens/screenCommon.hpp"
#include "screens/scriptScreen.hpp"
#include "screens/settingsScreen.hpp"
#include "screens/textScreen.hpp"
#include "screens/updaterScreen.hpp"
#include "screens/utilsScreen.hpp"

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern bool exiting;
extern int fadealpha;
extern bool fadein;
extern bool is3dsx;
extern bool isCitra;

void MainMenu::DrawSelection(void) const
{
	if (currentPage == 1) {
	if (Selection == 0) {
		Gui::drawGUISelector(button_tl_selector_idx, 0, 26, .020f);
	} else if (Selection == 1) {
		Gui::drawGUISelector(button_selector_idx, 166, 31, .020f);
	} else if (Selection == 2) {
		Gui::drawGUISelector(button_selector_idx, 3, 95, .020f);
	} else if (Selection == 3) {
		Gui::drawGUISelector(button_selector_idx, 166, 95, .020f);
	} else if (Selection == 4) {
		Gui::drawGUISelector(button_selector_idx, 3, 158, .020f);
	} else if (Selection == 5) {
		Gui::drawGUISelector(button_br_selector_idx, 161, 153, .020f);
	}


	} else if (currentPage == 2) {
	if (Selection == 0) {
		Gui::drawGUISelector(button_tl_selector_idx, 0, 26, .020f);
	} else if (Selection == 1) {
		Gui::drawGUISelector(button_selector_idx, 166, 31, .020f);
	} else if (Selection == 2) {
		Gui::drawGUISelector(button_selector_idx, 3, 95, .020f);
	} else if (Selection == 3) {
		Gui::drawGUISelector(button_selector_idx, 166, 95, .020f);
	} else if (Selection == 4) {
		Gui::drawGUISelector(button_selector_idx, 3, 158, .020f);
	}
	}
}

void MainMenu::Draw(void) const
{
	std::string running = Lang::Misc[0];
	Gui::DrawBGTop();
	animatedBGTop();
	Gui::DrawBarsTop();
	DisplayTime();
	drawBatteryTop();
	Gui::DrawString((400-Gui::GetStringWidth(0.72f, Lang::Title))/2, 0, 0.72f, WHITE, Lang::Title);
	Gui::DrawString(395-Gui::GetStringWidth(FONT_SIZE_18, VERSION_STRING), 218, FONT_SIZE_18, WHITE, VERSION_STRING);

	if (isCitra == true) {
		running += " Citra.";
	} else if (isCitra == false) {
		if (is3dsx == true) {
			running += " 3dsx.";
		} else if (is3dsx == false) {
			running += " cia.";
		}
	}

	Gui::DrawString(0, 218, FONT_SIZE_18, WHITE, running.c_str());

	if (fadealpha > 0) Draw_Rect(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in out effect

	Gui::DrawBGBot();
	animatedBGBot();
	Gui::DrawBarsBot();

	DrawBottom();
	Gui::DrawString(150, 4, 0.50f, WHITE, Lang::CurrentPage);
	Gui::DrawString(260, 4, 0.50, BLACK, "1"); //Draw First Page Number.
	Gui::DrawString(280, 4, 0.50, BLACK, "2"); //Draw Second Page Number.
	DrawSelection();
	DrawCurrentPage();
	if (fadealpha > 0) Draw_Rect(0, 0, 320, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
}

void MainMenu::DrawBottom(void) const
{
	if (currentPage == 1) {
		Gui::sprite(3, button_button_tl_idx, 1, 27);
		Gui::sprite(0, sprites_fileManagerIcon_idx, 9, 41);
		Gui::DrawString(43, 48, 0.65f, WHITE, Lang::MainMenu[0], 100);

		Gui::sprite(3, button_button_2_idx, 162, 27);
		Gui::sprite(0, sprites_ftpIcon_idx, 172, 40);
		Gui::DrawString(210, 45, 0.65f, WHITE, Lang::MainMenu[1], 100);

		Gui::sprite(3, button_button_3_idx, 1, 91);
		Gui::sprite(0, sprites_scriptIcon_idx, 4, 102);
		Gui::DrawString(43, 110, 0.65f, WHITE, Lang::MainMenu[2], 100);

		Gui::sprite(3, button_button_4_idx, 162, 91);
		Gui::sprite(0, sprites_music_icon_idx, 175, 106);
		Gui::DrawString(210, 110, 0.65f, WHITE, Lang::MainMenu[3], 100);

		Gui::sprite(3, button_button_5_idx, 1, 154);
		Gui::sprite(0, sprites_updaterIcon_idx, 9, 167);
		Gui::DrawString(43, 173, 0.65f, WHITE, Lang::MainMenu[4], 100);

		Gui::sprite(3, button_button_br_idx, 162, 151);
		Gui::sprite(0, sprites_settingsIcon_idx, 172, 165);
		Gui::DrawString(210, 173, 0.65f, WHITE, Lang::MainMenu[5], 100);


	} else if (currentPage == 2) {
		Gui::sprite(3, button_button_tl_idx, 1, 27);
		Gui::sprite(0, sprites_image_icon_idx, 6, 37);
		Gui::DrawString(43, 45, 0.65f, WHITE, Lang::MainMenu[6], 100);

		Gui::sprite(3, button_button_2_idx, 162, 27);
		Gui::DrawString(210, 45, 0.65f, WHITE, Lang::MainMenu[7], 100);

		Gui::sprite(3, button_button_3_idx, 1, 91);
		Gui::sprite(0, sprites_textEditorIcon_idx, 9, 103);
		Gui::DrawString(43, 110, 0.65f, WHITE, Lang::MainMenu[8], 100);

		Gui::sprite(3, button_button_4_idx, 162, 91);
		Gui::sprite(0, sprites_utilsIcon_idx, 175, 102);
		Gui::DrawString(210, 110, 0.65f, WHITE, Lang::MainMenu[9], 100);

		Gui::sprite(3, button_button_5_idx, 1, 154);
		Gui::DrawString(43, 173, 0.65f, WHITE, Lang::MainMenu[10], 100);
	}
}

void MainMenu::DrawCurrentPage(void) const
{
	if (currentPage == 1) {
		Gui::DrawString(260, 4, 0.50, WHITE, "1");
		Gui::sprite(0, sprites_frame_idx, 256, 4);
	} else if (currentPage == 2) {
		Gui::DrawString(280, 4, 0.50, WHITE, "2");
		Gui::sprite(0, sprites_frame_idx, 276, 4);
	}
}

void MainMenu::SelectionLogic(u32 hDown, u32 hHeld) {

		if (hDown & KEY_UP) {
			if(Selection > 1)	Selection -= 2;

		} else if (hDown & KEY_DOWN) {
			if (currentPage == 1) {
				if(Selection < 4)	Selection += 2;

			} else if (currentPage == 2) {
				if(Selection < 4 && Selection != 3)	Selection += 2;
			}

		} else if (hDown & KEY_LEFT) {
			if (Selection%2) Selection--;
		} else if (hDown & KEY_RIGHT) {
			if(currentPage == 1) {
				if (!(Selection%2)) Selection++;
			} else if(currentPage == 2) {
				if (!(Selection%2) && Selection != 4)	Selection++; // Special case, because of only 5 Buttons.
			}


		} else if (hHeld & KEY_SELECT) {
			helperBox("Press L/R to switch the Pages.\n\nPress D-Pad Up/Down to switch Selection.\n\nPress A to select.\n\nTouch a Button to select.");

		} else if (hDown & KEY_X) {
			Gui::setScreen(std::make_unique<Paint>());

		}  else if (hDown & KEY_A) {
			if (currentPage == 1) {
				switch(Selection) {
					case 0: {
						Gui::setScreen(std::make_unique<FileManager>());
						break;
					}   case 1:
						Gui::setScreen(std::make_unique<FTP>());
						break;
				 	 case 2: {
						Gui::setScreen(std::make_unique<Script>());
						break;
					} case 3: {
						Gui::setScreen(std::make_unique<Music>());
						break;
					} case 4: {
						Gui::setScreen(std::make_unique<Updater>());
						break;
					} case 5:
						Gui::setScreen(std::make_unique<Settings>());
						break;
				}
			} else if (currentPage == 2) {
				switch(Selection) {
					case 0: {
						Gui::setScreen(std::make_unique<Image>());
						break;
					} case 1:
						Gui::setScreen(std::make_unique<Credits>());
						break;
				 	 case 2: {
						Gui::setScreen(std::make_unique<Text>());
						break;
					} case 3: {
						Gui::setScreen(std::make_unique<Utils>());
						break;
					} case 4: {
						Gui::setScreen(std::make_unique<GameSub>());
						break;
					}
				}
			}
		}
}

void MainMenu::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	SelectionLogic(hDown, hHeld);
		// Page Switching.
	if (currentPage == 1 && hDown & KEY_R) {
			Selection = 0;
			currentPage = 2;
		} else if (currentPage == 2 && hDown & KEY_L) {
			Selection = 0;
			currentPage = 1;

		} else if (hDown & KEY_START) {
			exiting = true;

		// First Page.
		} else if (currentPage == 1 && hDown & KEY_TOUCH) {
			if (touching(touch, mainScreenButtonPos[0])) {
				Gui::setScreen(std::make_unique<FileManager>());
			} else if (touching(touch, mainScreenButtonPos[1])) {
				Gui::setScreen(std::make_unique<FTP>());
			} else if (touching(touch, mainScreenButtonPos[2])) {
				Gui::setScreen(std::make_unique<Script>());
			} else if (touching(touch, mainScreenButtonPos[3])) {
				Gui::setScreen(std::make_unique<Music>());
			} else if (touching(touch, mainScreenButtonPos[4])) {
				Gui::setScreen(std::make_unique<Updater>());
			} else if (touching(touch, mainScreenButtonPos[5])) {
				Gui::setScreen(std::make_unique<Settings>());
			}

			// Second Page.
		} else if (currentPage == 2 && hDown & KEY_TOUCH) {
			if (touching(touch, mainScreenButtonPos[6])) {
				Gui::setScreen(std::make_unique<Image>());
			} else if (touching(touch, mainScreenButtonPos[7])) {
				Gui::setScreen(std::make_unique<Credits>());
			} else if (touching(touch, mainScreenButtonPos[8])) {
				Gui::setScreen(std::make_unique<Text>());
 			} else if (touching(touch, mainScreenButtonPos[9])) {
				Gui::setScreen(std::make_unique<Utils>());
			} else if (touching(touch, mainScreenButtonPos[10])) {
				Gui::setScreen(std::make_unique<GameSub>());
		}
	}
}