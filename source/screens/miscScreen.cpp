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

void drawCredits(void) {
	volt_draw_on(GFX_TOP, GFX_LEFT);
	volt_draw_texture(CreditsImage, 0, 0);
	
		if (settings.universal.theme == 0) {
		drawBgBot();
		drawBarsBotLight();
	} else if (settings.universal.theme == 1) {
		drawBgBot();
		drawBarsBotDark();
	} else if (settings.universal.theme == 2) {
		drawBgBot();
		drawBarsBotRed();
	}
	volt_draw_texture(BackIcon, 288, 208);
	volt_end_draw();
}

void notImplemented(void) {
	displayMsg("Not implemented Yet.\n");
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}