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
#include <unistd.h>
#include <vector>


void drawImageViewerUI(void) {
	 // Theme Stuff.
	if (settings.universal.theme == 0) {
		drawBgTop();
		drawBarsTopLight();
	} else if (settings.universal.theme == 1) {
		drawBgTop();
		drawBarsTopDark();
	}
	volt_draw_text(110, 4, 0.72f, 0.72f, WHITE, "Image Viewer Menu");

		if (settings.universal.theme == 0) {
		drawBgBot();
		drawBarsBotLight();
	} else if (settings.universal.theme == 1) {
		drawBgBot();
		drawBarsBotDark();
	}
	volt_end_draw();

}