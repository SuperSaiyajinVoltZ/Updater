	/*
*   This file is part of Universal-Manager-NX
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

#include "screens/fileManagerScreen.hpp"
#include "screens/mainMenuScreen.hpp"
#include "screens/screenCommon.hpp"

#include "utils/download.hpp"

#include <stdio.h>
#include <stdlib.h>


extern "C" {
	#include "touch_helper.h"
}

extern int fadealpha;
extern bool fadein;


void MainMenu::Draw(void) const
{
	Gui::DrawBG();
	Gui::DrawBars();
	Gui::DrawText(250, 0, 72, WHITE, "Universal-Manager-NX");
	Gui::DrawText(1000, 630, 72, WHITE, VERSION_STRING);

	// Buttons.
	Gui::DrawImage(MainMenuButton, 40, 150);
	Gui::DrawImage(MainMenuButton, 40, 300);
	Gui::DrawImage(MainMenuButton, 40, 450);
	if (fadealpha > 0) Gui::DrawRect(0, 0, 1280, 720, FC_MakeColor(0, 0, 0, fadealpha)); // Fade in/out effect
}

void MainMenu::Logic(u64 hDown) {
	if (hDown & KEY_A)
		Gui::setScreen(std::make_unique<FileManager>());

	if (hDown & KEY_Y)
		updateUniversalManager();
}