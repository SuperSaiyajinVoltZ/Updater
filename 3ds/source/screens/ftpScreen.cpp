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

#include "screens/ftpScreen.hpp"
#include "screens/screenCommon.hpp"

#include <algorithm>
#include <fstream>
#include <unistd.h>

extern "C" {
	#include "ftp.h"
}

void FTP::Draw(void) const
{
	ftp_init();

	Result ret = 0;
	char buf[137], hostname[128];
	u32 wifiStatus = 0;

	int pBar = 0, xlim = 270;

	ret = gethostname(hostname, sizeof(hostname));

	while(ftpEnabled == 1) {
		ftp_loop();
		Gui::clearTextBufs();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		Gui::DrawBGTop();
		Gui::DrawBarsTop();
		DisplayTime();
		drawBatteryTop();
		Gui::DrawString((400-Gui::GetStringWidth(0.72f, "FTP Mode"))/2, 0, 0.72f, WHITE, "FTP Mode");
		Gui::DrawBGBot();
		Gui::DrawBarsBot();
		ret = ACU_GetWifiStatus(&wifiStatus);

		if ((wifiStatus != 0) && R_SUCCEEDED(ret)) {
			Gui::DrawString(((320 - Gui::GetStringWidth(0.48f, "FTP initialized")) / 2), 40, 0.48f, WHITE, "FTP initialized");
			snprintf(buf, 137, "IP: %s:5000", R_FAILED(ret)? "Failed to get IP" : hostname);

			if (strlen(ftp_accepted_connection) != 0)
				Gui::DrawString(((320 - Gui::GetStringWidth(0.48f, ftp_accepted_connection)) / 2), 80, 0.48f, WHITE, ftp_accepted_connection);

			if (strlen(ftp_file_transfer) != 0)
				Gui::DrawString(((320 - Gui::GetStringWidth(0.45f, ftp_file_transfer)) / 2), 150, 0.45f, WHITE, ftp_file_transfer);

			if (isTransfering) {
				C2D_DrawRectSolid(50, 140, 0.5f, 220, 3, BLACK);
				C2D_DrawRectSolid(pBar, 140, 0.5f, 40, 3, WHITE);

				// Boundary stuff
				C2D_DrawRectSolid(0, 140, 0.5f, 50, 3, BLACK);
				C2D_DrawRectSolid(270, 140, 0.5f, 50, 3, BLACK);
				pBar += 4;

				if (pBar >= xlim)
					pBar = 34;
			}
		}
		else {
			Gui::DrawString(((320 - Gui::GetStringWidth(0.48f, "Failed to initialize FTP.")) / 2), 40, 0.48f, WHITE, "Failed to initialize FTP.");
			snprintf(buf, 18, "WiFi not enabled.");
		}

		Gui::DrawString(((320 - Gui::GetStringWidth(0.48f, buf)) / 2), 60, 0.48f, WHITE, buf);
		Gui::DrawString(((320 - Gui::GetStringWidth(0.48f, "Press B to Return to the Main Menu.")) / 2), 220, 0.48f, WHITE, "Press B to Return to the Main Menu.");

		Gui::clearTextBufs();
		C3D_FrameEnd(0);

		hidScanInput();
		u32 hDown = hidKeysDown();

		if (hDown & KEY_B)
			break;
	}
	memset(ftp_accepted_connection, 0, 20); // Empty accepted connection address
	memset(ftp_file_transfer, 0, 50); // Empty transfer status
	ftp_exit();

	Gui::screenBack();
	return;
}

void FTP::Logic(u32 hDown, u32 hHeld, touchPosition touch)
{
}