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

#include "screens/screenCommon.hpp"
#include <algorithm>
#include <unistd.h>
#include "settings.hpp"
#include "updaterScreen.hpp"
#include "mainMenuScreen.hpp"
#include "download.hpp"

extern "C" {
	#include "cia.h"
}

extern bool updatingSelf;

extern bool touching(touchPosition touch, Structs::ButtonPos button);

void Updater::Draw(void) const
{
	Gui::DrawBGTop();
	animatedBGTop();
	Gui::DrawBarsTop();
	DisplayTime();
	drawBatteryTop();

	DrawTitle();

	Gui::DrawBGBot();
	animatedBGBot();
	Gui::DrawBarsBottomBack();

	DrawTitleButtons();
	DrawButtons();
	DrawText();

	DrawSelection();
	Draw_Text(140, 4, 0.50f, WHITE, "Current Page:");
	Draw_Text(240, 4, 0.50, BLACK, "1"); //Draw First Page Number.
	Draw_Text(260, 4, 0.50, BLACK, "2"); //Draw Second Page Number.
	Draw_Text(280, 4, 0.50, BLACK, "3"); //Draw Third Page Number.
	Draw_Text(300, 4, 0.50, BLACK, "4"); //Draw Fourth Page Number.
	DrawCurrentPage();
}

void Updater::Logic(u32 hDown, u32 hHeld, touchPosition touch) 
{
	ButtonLogic(hDown, hHeld);
	TouchLogic(hDown, touch);
}

void Updater::DrawButtons(void) const
{
	if (buttonsAmount == 2) {
			Gui::sprite(sprites_updaterButton_idx, Functions[0].x, Functions[0].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[1].x, Functions[1].y);
	} else if (buttonsAmount == 3) {
			Gui::sprite(sprites_updaterButton_idx, Functions[0].x, Functions[0].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[1].x, Functions[1].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[2].x, Functions[2].y);
	} else if (buttonsAmount == 4) {
			Gui::sprite(sprites_updaterButton_idx, Functions[6].x, Functions[6].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[7].x, Functions[7].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[8].x, Functions[8].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[9].x, Functions[9].y);
	} else if (buttonsAmount == 5) {
			Gui::sprite(sprites_updaterButton_idx, Functions[0].x, Functions[0].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[1].x, Functions[1].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[2].x, Functions[2].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[3].x, Functions[3].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[4].x, Functions[4].y);
	} else if (buttonsAmount == 6) {
			Gui::sprite(sprites_updaterButton_idx, Functions[0].x, Functions[0].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[1].x, Functions[1].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[2].x, Functions[2].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[3].x, Functions[3].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[4].x, Functions[4].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[5].x, Functions[5].y);
	} else if (buttonsAmount == 8) {
			Gui::sprite(sprites_updaterButton_idx, Functions[0].x, Functions[0].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[1].x, Functions[1].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[2].x, Functions[2].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[3].x, Functions[3].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[4].x, Functions[4].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[5].x, Functions[5].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[11].x, Functions[11].y);
			Gui::sprite(sprites_updaterButton_idx, Functions[12].x, Functions[12].y);
	}
}

void Updater::DrawTitleButtons(void) const
{
	if (titleButtons == 0) {
	} else if (titleButtons == 1) {
			Gui::sprite(sprites_titleButton_idx, 0, 48);
	} else if (titleButtons == 2) {
			Gui::sprite(sprites_titleButton_idx, 0, 48);
			Gui::sprite(sprites_titleButton_idx, 0, 88);
	} else if (titleButtons == 3) {
			Gui::sprite(sprites_titleButton_idx, 0, 48);
			Gui::sprite(sprites_titleButton_idx, 0, 88);
			Gui::sprite(sprites_titleButton_idx, 0, 128);
	} else if (titleButtons == 4) {
			Gui::sprite(sprites_titleButton_idx, 0, 48);
			Gui::sprite(sprites_titleButton_idx, 0, 88);
			Gui::sprite(sprites_titleButton_idx, 0, 128);
			Gui::sprite(sprites_titleButton_idx, 0, 168);
	}
}

void Updater::ButtonLogic(u32 hDown, u32 hHeld)
{
	if (hHeld & KEY_SELECT) {
		if (updaterMode == 0) {
		} else {
			helperBox(" Press \uE052 / \uE053 to switch Pages.");
		}
	} else if (hDown & KEY_UP) {
		if(Selection > 0)	Selection--;
		} else if (hDown & KEY_DOWN) {
			if (updaterMode == 0) {
				if(Selection < 3)	Selection++;

			} else if (updaterMode == 1) {
				if(Selection < 2)	Selection++;

			} else if (updaterMode == 2) {
				if(Selection < 4)	Selection++;

			} else if (updaterMode == 3) {
				if(Selection < 1)	Selection++;

			} else if (updaterMode == 4) {
				if(Selection < 7)	Selection++;
			}

	} else if (hDown & KEY_A) {
		if (updaterMode == 0) {
			switch(Selection) {
				case 0: {
					Selection = 0;
					buttonsAmount = 3;
					titleButtons = 2;
					updaterMode = 1;
					break;
				} case 1:
					Selection = 0;
					buttonsAmount = 5;
					titleButtons = 3;
					updaterMode = 2;
					break;
				  case 2: {
					Selection = 0;
					buttonsAmount = 2;
					titleButtons = 1;
					updaterMode = 3;
					break;
				} case 3: {
					Selection = 0;
					buttonsAmount = 8;
					titleButtons = 4;
					updaterMode = 4;
					break;
				}
			}

		} else if (updaterMode == 1) {
			switch(Selection) {
				case 0: {
					if(confirmPopup("Are you sure you want to update Luma3DS\nTo Release?")) {
					updateLuma(false);
					}
					break;
				} case 1:
					if(confirmPopup("Are you sure you want to update Luma3DS\nTo Nightly?")) {
					updateLuma(true);
					}
					break;
				  case 2: {
					if(confirmPopup("Are you sure you want to update GodMode9\nTo Release?")) {
					downloadGodMode9();
					}
					break;
				  }
			}

		} else if (updaterMode == 2) {
			switch(Selection) {
				case 0: {
					if(confirmPopup("Are you sure you want to update TWiLightMenu\nTo Release?")) {
					updateTWiLight(false);
					}
					break;
				} case 1:
					if(confirmPopup("Are you sure you want to update TWiLightMenu\nTo Nightly?")) {
					updateTWiLight(true);
					}
					break;
				  case 2: {
					if(confirmPopup("Are you sure you want to update NDS-Bootstrap\nTo Release?")) {
					updateBootstrap(false);
					}
					break;
				} case 3: {
					if(confirmPopup("Are you sure you want to update NDS-Bootstrap\nTo Nightly?")) {
					updateBootstrap(true);
					}
					break;
				} case 4: {
					if(confirmPopup("Are you sure you want to download the Usrcheat.dat?")) {
					updateCheats();
					}
					break;
				}
			}

		} else if (updaterMode == 3) {
			switch(Selection) {
				case 0: {
					downloadThemes();
					break;
				} case 1:
					downloadScripts();
					break;
			}
		} else if (updaterMode == 4) {
			switch(Selection) {
				case 0: {
					if(confirmPopup("Are you sure you want to update Universal-Manager\nTo Release?")) {
					updatingSelf = true;
					updateUniversalManager(false);
					updatingSelf = false;
					}
					break;
				} case 1:
					if(confirmPopup("Are you sure you want to update Universal-Manager\nTo Nightly?")) {
					updatingSelf = true;
					updateUniversalManager(true);
					updatingSelf = false;
					}
					break;
				  case 2: {
					if(confirmPopup("Are you sure you want to update PKMN-Chest\nTo Release?")) {
					updatePKMNChestRelease(); 
					}
					break;
				} case 3: {
					if(confirmPopup("Are you sure you want to update PKMN-Chest\nTo Nightly?")) {
					updatePKMNChestNightly(); 
					}
					break;
				} case 4: {
					if(confirmPopup("Are you sure you want to update Relaunch\nTo Release?")) {
					updateRelaunchRelease(); 
					}
					break;
				} case 5:
					if(confirmPopup("Are you sure you want to update Relaunch\nTo Nightly?")) {
					updateRelaunchNightly(); 
					}
					break;
				case 6: {
					if(confirmPopup("Are you sure you want to update LeafEdit\nTo Release?")) {
					updateLeafEditRelease();
					}
					break;
				} case 7:
					if(confirmPopup("Are you sure you want to update LeafEdit\nTo Nightly?")) {
					updateLeafEdit(); 
					}
					break;
			}
		}
	} else if (hDown & KEY_B) {
		if (updaterMode == 0) {
			Selection = 0;
			Gui::screenBack();
			return;

		} else if (updaterMode == 1) {
			Selection = 0;
			buttonsAmount = 4;
			titleButtons = 0;
			updaterMode = 0;

		} else if (updaterMode == 2) {
			Selection = 0;
			buttonsAmount = 4;
			titleButtons = 0;
			updaterMode = 0;

		} else if (updaterMode == 3) {
			Selection = 0;
			buttonsAmount = 4;
			titleButtons = 0;
			updaterMode = 0;

		} else if (updaterMode == 4) {
			Selection = 0;
			buttonsAmount = 4;
			titleButtons = 0;
			updaterMode = 0;
		}

	} else if (hDown & KEY_L) {
		if (updaterMode == 2) {
			Selection = 0;
			buttonsAmount = 3;
			titleButtons = 2;
			updaterMode = 1;

		} else if (updaterMode == 3) {
			Selection = 0;
			buttonsAmount = 5;
			titleButtons = 3;
			updaterMode = 2;

		} else if (updaterMode == 4) {
			Selection = 0;
			buttonsAmount = 2;
			titleButtons = 1;
			updaterMode = 3;
		}

	} else if (hDown & KEY_R) {
		if (updaterMode == 1) {
			Selection = 0;
			buttonsAmount = 5;
			titleButtons = 3;
			updaterMode = 2;

		} else if (updaterMode == 2) {
			Selection = 0;
			buttonsAmount = 2;
			titleButtons = 1;
			updaterMode = 3;

		} else if (updaterMode == 3) {
			Selection = 0;
			buttonsAmount = 8;
			titleButtons = 4;
			updaterMode = 4;
		}
	}
}

void Updater::TouchLogic(u32 hDown, touchPosition touch)
{
	if (hDown & KEY_TOUCH) {
		if (updaterMode == 0) {
			if (touching(touch, Functions[6])) {
				Selection = 0;
				buttonsAmount = 3;
				titleButtons = 2;
				updaterMode = 1;

		} else if (touching(touch, Functions[7])) {
			Selection = 0;
			buttonsAmount = 5;
			titleButtons = 3;
			updaterMode = 2;

		} else if (touching(touch, Functions[8])) {
			Selection = 0;
			buttonsAmount = 2;
			titleButtons = 1;
			updaterMode = 3;

		} else if (touching(touch, Functions[9])) {
			Selection = 0;
			buttonsAmount = 8;
			titleButtons = 4;
			updaterMode = 4;

		} else if(touching(touch, Functions[10])) {
			Gui::screenBack();
			return;
		}


		} else if (updaterMode == 1) {
			if (touching(touch, Functions[0])) {
				if(confirmPopup("Are you sure you want to update Luma3DS\nTo Release?")) {
					updateLuma(false);
				}

		} else if (touching(touch, Functions[1])) {
			if(confirmPopup("Are you sure you want to update Luma3DS\nTo Nightly?")) {
			updateLuma(true);
			}

		} else if (touching(touch, Functions[2])) {
			if(confirmPopup("Are you sure you want to update GodMode9\nTo Release?")) {
			downloadGodMode9();
			}

		} else if (touching(touch, Functions[10])) {
			Selection = 0;
			buttonsAmount = 4;
			titleButtons = 0;
			updaterMode = 0;
		}


		} else if (updaterMode == 2) {
		if (touching(touch, Functions[0])) {
			if(confirmPopup("Are you sure you want to update TWiLightMenu\nTo Release?")) {
			updateTWiLight(false);
			}

		} else if (touching(touch, Functions[1])) {
			if(confirmPopup("Are you sure you want to update TWiLightMenu\nTo Nightly?")) {
			updateTWiLight(true);
			}

		} else if (touching(touch, Functions[2])) {
			if(confirmPopup("Are you sure you want to update NDS-Bootstrap\nTo Release?")) {
			updateBootstrap(false);
			}

		} else if (touching(touch, Functions[3])) {
			if(confirmPopup("Are you sure you want to update NDS-Bootstrap\nTo Nightly?")) {
			updateBootstrap(true);
			}

			} else if (touching(touch, Functions[4])) {
			if(confirmPopup("Are you sure you want to download the Usrcheat.dat?")) {
			updateCheats();
			}

		} else if (touching(touch, Functions[10])) {
			Selection = 0;
			buttonsAmount = 4;
			titleButtons = 0;
			updaterMode = 0;
		}


		} else if (updaterMode == 3) {
			if (touching(touch, Functions[0])) {
			downloadThemes();

		} else if (touching(touch, Functions[1])) {
			downloadScripts();

		} else if(touching(touch, Functions[10])) {
			Selection = 0;
			buttonsAmount = 4;
			titleButtons = 0;
			updaterMode = 0;
		}


		} else if (updaterMode == 4) {
		if (touching(touch, Functions[0])) {
			if(confirmPopup("Are you sure you want to update Universal-Manager\nTo Release?")) {
			updatingSelf = true;
			updateUniversalManager(false);
			updatingSelf = false;
			}

		} else if (touching(touch, Functions[1])) {
			if(confirmPopup("Are you sure you want to update Universal-Manager\nTo Nightly?")) {
			updatingSelf = true;
			updateUniversalManager(true);
			updatingSelf = false;
			}

		} else if (touching(touch, Functions[10])) {
			Selection = 0;
			buttonsAmount = 4;
			titleButtons = 0;
			updaterMode = 0;

		} else if (touching(touch, Functions[2])) {
			if(confirmPopup("Are you sure you want to update PKMN-Chest\nTo Release?")) {
			updatePKMNChestRelease(); 
			}

		} else if (touching(touch, Functions[3])) {
			if(confirmPopup("Are you sure you want to update PKMN-Chest\nTo Nightly?")) {
			updatePKMNChestNightly(); 
			}

			} else if (touching(touch, Functions[4])) {
			if(confirmPopup("Are you sure you want to update Relaunch\nTo Release?")) {
			updateRelaunchRelease(); 
			}

		} else if (touching(touch, Functions[5])) {
			if(confirmPopup("Are you sure you want to update Relaunch\nTo Nightly?")) {
			updateRelaunchNightly(); 
			}
		} else if (touching(touch, Functions[11])) {
			if(confirmPopup("Are you sure you want to update LeafEdit\nTo Release?")) {
			updateLeafEditRelease();
			}
		} else if (touching(touch, Functions[12])) {
			if(confirmPopup("Are you sure you want to update LeafEdit\nTo Nightly?")) {
			updateLeafEdit(); 
			}
		}
	}
}
}

void Updater::DrawText(void) const
{
	if (updaterMode == 0) {
	Draw_Text(49, 58, 0.7f, WHITE, "CFW");
	Draw_Text(140, 58, 0.7f, WHITE, "TWL");
	Draw_Text(229, 58, 0.7f, WHITE, "Other");
	Draw_Text(49, 98, 0.7f, WHITE, "UNIV");

	} else if (updaterMode == 1) {
	Draw_Text(18, 58, 0.65f, WHITE, "Luma3DS");
	Draw_Text(140, 58, 0.7f, WHITE, "Release");
	Draw_Text(229, 58, 0.7f, WHITE, "Nightly");

	Draw_Text(15, 98, 0.65f, WHITE, "GodMode9");
	Draw_Text(140, 98, 0.7f, WHITE, "Release");

	} else if (updaterMode == 2) {
	Draw_Text(8, 58, 0.65f, WHITE, "TWLMENU++");
	Draw_Text(140, 58, 0.7f, WHITE, "Release");
	Draw_Text(229, 58, 0.7f, WHITE, "Nightly");

	Draw_Text(8, 98, 0.60f, WHITE, "NDS-Bootstrap");
	Draw_Text(140, 98, 0.7f, WHITE, "Release");
	Draw_Text(229, 98, 0.7f, WHITE, "Nightly");

	Draw_Text(30, 138, 0.7f, WHITE, "Extras");
	Draw_Text(140, 138, 0.7f, WHITE, "Cheats");

	} else if (updaterMode == 3) {
	Draw_Text(30, 58, 0.7f, WHITE, "Extras");
	Draw_Text(140, 58, 0.7f, WHITE, "Themes");
	Draw_Text(229, 58, 0.7f, WHITE, "Scripts");

	} else if (updaterMode == 4) {
	Draw_Text(5, 58, 0.65f, WHITE, "Univ-Manager");
	Draw_Text(140, 58, 0.7f, WHITE, "Release");
	Draw_Text(229, 58, 0.7f, WHITE, "Nightly");

	Draw_Text(13, 98, 0.65f, WHITE, "pkmn-chest");
	Draw_Text(140, 98, 0.7f, WHITE, "Release");
	Draw_Text(229, 98, 0.7f, WHITE, "Nightly");

	Draw_Text(18, 138, 0.65f, WHITE, "Relaunch");
	Draw_Text(140, 138, 0.7f, WHITE, "Release");
	Draw_Text(229, 138, 0.7f, WHITE, "Nightly");

	Draw_Text(18, 178, 0.65f, WHITE, "LeafEdit");
	Draw_Text(140, 178, 0.7f, WHITE, "Release");
	Draw_Text(229, 178, 0.7f, WHITE, "Nightly");
	}
}

void Updater::DrawTitle(void) const
{
	if (updaterMode == 0) {
		Draw_Text((400-Draw_GetTextWidth(0.72f, "Updater Sub Menu"))/2, 0, 0.72f, WHITE, "Updater Sub Menu");
	} else if (updaterMode == 1) {
		Draw_Text((400-Draw_GetTextWidth(0.72f, "CFW Updater Screen"))/2, 0, 0.72f, WHITE, "CFW Updater Screen");
	} else if (updaterMode == 2) {
		Draw_Text((400-Draw_GetTextWidth(0.72f, "TWL Updater Screen"))/2, 0, 0.72f, WHITE, "TWL Updater Screen");
	} else if (updaterMode == 3) {
		Draw_Text((400-Draw_GetTextWidth(0.72f, "Other Updater Screen"))/2, 0, 0.72f, WHITE, "Other Updater Screen");
	} else if (updaterMode == 4) {
		Draw_Text((400-Draw_GetTextWidth(0.72f, "Universal Updater Screen"))/2, 0, 0.72f, WHITE, "Universal Updater Screen");
	}
}

void Updater::DrawCurrentPage(void) const
{
	if (updaterMode == 1) {
		Draw_Text(240, 4, 0.50, WHITE, "1");
	} else if (updaterMode == 2) {
		Draw_Text(260, 4, 0.50, WHITE, "2");
	} else if (updaterMode == 3) {
		Draw_Text(280, 4, 0.50, WHITE, "3");
	} else if (updaterMode == 4) {
		Draw_Text(300, 4, 0.50, WHITE, "4");
	}
}

void Updater::DrawSelection(void) const
{
	if (updaterMode == 0) {
	if (Selection == 0) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 100, 38, Config::barColor);
	} else if (Selection == 1) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 38, Config::barColor);
	} else if (Selection == 2) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 282, 38, Config::barColor);
	} else if (Selection == 3) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 100, 78, Config::barColor);
	}


	} else if (updaterMode == 1) {
	if (Selection == 0) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 30, Config::barColor);
	} else if (Selection == 1) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 282, 30, Config::barColor);
	} else if (Selection == 2) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 70, Config::barColor);
	}


	} else if (updaterMode == 2) {
	if (Selection == 0) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 30, Config::barColor);
	} else if (Selection == 1) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 282, 30, Config::barColor);
	} else if (Selection == 2) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 70, Config::barColor);
	} else if (Selection == 3) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 282, 70, Config::barColor);
	} else if (Selection == 4) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 110, Config::barColor);
	}


	} else if (updaterMode == 3) {
	if (Selection == 0) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 30, Config::barColor);
	} else if (Selection == 1) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 282, 30, Config::barColor);
	}


	} else if (updaterMode == 4) {
	if (Selection == 0) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 30, Config::barColor);
	} else if (Selection == 1) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 282, 30, Config::barColor);
	} else if (Selection == 2) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 70, Config::barColor);
	} else if (Selection == 3) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 282, 70, Config::barColor);
	} else if (Selection == 4) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 110, Config::barColor);
	} else if (Selection == 5) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 282, 110, Config::barColor);
	} else if (Selection == 6) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 191, 140, Config::barColor);
	} else if (Selection == 7) {
		Gui::Draw_ImageBlend(sprites_arrow_idx, 282, 140, Config::barColor);
	}
	}
}