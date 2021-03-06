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
#include "screens/mainMenuScreen.hpp"
#include "screens/screenCommon.hpp"

#include "utils/settings.hpp"
#include "utils/sound.h"

#include <algorithm>
#include <citro2d.h>
#include <fstream>
#include <unistd.h>


// Color Stuff.
#define VOLTZCOLORBAR RGBA8(0, 108, 255, 255)
#define VOLTZCOLORBG RGBA8(0, 205, 199, 255)

#define FLAMECOLORBAR RGBA8(50, 50, 50, 255)
#define FLAMECOLORBG RGBA8(66, 233, 66, 255)

#define PK11COLORBAR RGBA8(100, 100, 255, 255)
#define PK11COLORBG RGBA8(255, 100, 150, 255)

#define ROCKETROBZCOLORBAR RGBA8(127, 127, 127, 255)
#define ROCKETROBZCOLORBG RGBA8(0, 0, 31, 255)

#define TNGCOLORBAR RGBA8(127, 127, 127, 255)
#define TNGCOLORBG RGBA8(0, 0, 31, 255)

extern bool touching(touchPosition touch, Structs::ButtonPos button);
extern int fadealpha;
extern bool fadein;

void drawTextBox(void) {
	Draw_Rect(175, 30, 220, 190, C2D_Color32(128, 128, 128, 255));
	Draw_Rect(180, 35, 210, 180, C2D_Color32(0, 0, 0, 190));
}

void drawDiscordQR(void) {
	set_screen(top);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, C2D_Color32(7, 54, 66, 255));
	C2D_DrawRectSolid(0, 215, 0.5f, 400, 25, C2D_Color32(0, 44, 57, 255));
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 25, C2D_Color32(0, 44, 57, 255));
	Gui::sprite(0, sprites_top_screen_top_idx, 0, 0);
	Gui::sprite(0, sprites_top_screen_bot_idx, 0, 215);
	Draw_Rect(0, 0, 400, 240, DIM);
	Gui::sprite(1, credits_discord_idx, 115, 35);
	set_screen(bottom);
	C2D_DrawRectSolid(0, 0, 0.5f, 320, 240, C2D_Color32(7, 54, 66, 255));
	C2D_DrawRectSolid(0, 215, 0.5f, 320, 25, C2D_Color32(0, 44, 57, 255));
	C2D_DrawRectSolid(0, 0, 0.5f, 320, 25, C2D_Color32(0, 44, 57, 255));
	Gui::sprite(0, sprites_bottom_screen_top_idx, 0, 0);
	Gui::sprite(0, sprites_bottom_screen_bot_idx, 0, 215);
}

void Credits::drawEasterEgg(void) const {
	if (isVoltZAlready == true) {
		playEasteregg();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		set_screen(top);
		drawTextBox();
		Gui::DrawString(185, 65, 0.60, WHITE, "IT'S OVER 9000!!");
		C3D_FrameEnd(0);
		for (float i = 0.0; i < 60*2.6; i++) {
			gspWaitForVBlank();
		}
	}
}


void drawVoltZ(void) {
	// Top BG Stuff.
	set_screen(top);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, VOLTZCOLORBG);
	Gui::sprite(0, sprites_universal_bg_top_idx, 0, 25);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 25, VOLTZCOLORBAR);
	Gui::sprite(0, sprites_top_screen_top_idx, 0, 0);
	Gui::DrawString(50, 0, 0.72f, WHITE, "Welcome to Universal-Manager!");

	drawTextBox();

	Gui::sprite(1, credits_voltZ_idx, -10, 40);
	Gui::DrawString(250, 50, 0.40, WHITE, "Hello There!");
	Gui::DrawString(185, 65, 0.40, WHITE, "Thank you for using Universal-Manager!\nI'm the Main Developer, VoltZ.\n I'm also the Main Developer of LeafEdit,\nanother project from our Team which you \ncan download Using: Universal-Manager! \nI hope You enjoy Universal-Manager\nand don't be shy:\nTo join the Universal-Server on Discord!\nAlso visit our site here: \nhttps://universal-team.github.io/\nYou can always hold Select, to see the \nInstructions.");

	// BG Bottom Stuff.
	set_screen(bottom);
	Draw_Rect(0, 0, 400, 240, VOLTZCOLORBG);
	C2D_DrawRectSolid(0, 0, 0.5f, 320, 25, VOLTZCOLORBAR);
	Gui::sprite(0, sprites_universal_bg_bottom_idx, 0, 25);
	Gui::sprite(0, sprites_bottom_screen_top_idx, 0, 0);
	C2D_DrawRectSolid(0, 215, 0.5f, 320, 25, VOLTZCOLORBAR);
	Gui::sprite(0, sprites_bottom_screen_bot_idx, 0, 215);
	Gui::DrawString((320-Gui::GetStringWidth(0.7f, "VoltZ"))/2, 0, 0.7f, WHITE, "VoltZ");
}

void drawFlame(void) {
	// Top BG Stuff.
	set_screen(top);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, FLAMECOLORBG);
	Gui::sprite(0, sprites_universal_bg_top_idx, 0, 25);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 25, FLAMECOLORBAR);
	Gui::sprite(0, sprites_top_screen_top_idx, 0, 0);
	Gui::DrawString(50, 0, 0.72f, WHITE, "Welcome to Universal-Manager!");


	drawTextBox();
	Gui::sprite(1, credits_flame_idx, 0, 40);
	Gui::DrawString(240, 50, 0.40, WHITE, "Hey, I’m Flame!");
	Gui::DrawString(185, 65, 0.40, WHITE, "I’m the main developer of Relaunch!\nI have helped VoltZ with a small bit\n       of code in Universal-Manager.");

	// BG Bottom Stuff.
	set_screen(bottom);
	Draw_Rect(0, 0, 400, 240, FLAMECOLORBG);
	C2D_DrawRectSolid(0, 0, 0.5f, 320, 25, FLAMECOLORBAR);
	Gui::sprite(0, sprites_universal_bg_bottom_idx, 0, 25);
	Gui::sprite(0, sprites_bottom_screen_top_idx, 0, 0);
	C2D_DrawRectSolid(0, 215, 0.5f, 320, 25, FLAMECOLORBAR);
	Gui::sprite(0, sprites_bottom_screen_bot_idx, 0, 215);
	Gui::DrawString((320-Gui::GetStringWidth(0.7f, "Flame"))/2, 0, 0.7f, WHITE, "Flame");
}

void drawPk11(void) {
	// Top BG Stuff.
	set_screen(top);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, PK11COLORBG);
	Gui::sprite(0, sprites_universal_bg_top_idx, 0, 25);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 25, PK11COLORBAR);
	Gui::sprite(0, sprites_top_screen_top_idx, 0, 0);
	Gui::DrawString(50, 0, 0.72f, WHITE, "Welcome to Universal-Manager!");


	drawTextBox();
	Gui::sprite(1, credits_pk11_idx, -20, 40);
	Gui::DrawString(270, 50, 0.40, WHITE, "Hi .o/");
	Gui::DrawString(185, 65, 0.40, WHITE, "I'm Pk11, I've helped out with this app\na bit and am the main dev on pkmn-chest,\nanother Universal-Team project which\nyou can install using Universal Manager!");

	// BG Bottom Stuff.
	set_screen(bottom);
	Draw_Rect(0, 0, 400, 240, PK11COLORBG);
	C2D_DrawRectSolid(0, 0, 0.5f, 320, 25, PK11COLORBAR);
	Gui::sprite(0, sprites_universal_bg_bottom_idx, 0, 25);
	Gui::sprite(0, sprites_bottom_screen_top_idx, 0, 0);
	C2D_DrawRectSolid(0, 215, 0.5f, 320, 25, PK11COLORBAR);
	Gui::sprite(0, sprites_bottom_screen_bot_idx, 0, 215);
	Gui::DrawString((320-Gui::GetStringWidth(0.7f, "Pk11"))/2, 0, 0.7f, WHITE, "Pk11");
}

void drawRocketRobz(void) {
	// Top BG Stuff.
	set_screen(top);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, ROCKETROBZCOLORBG);
	Gui::sprite(0, sprites_universal_bg_top_idx, 0, 25);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 25, ROCKETROBZCOLORBAR);
	Gui::sprite(0, sprites_top_screen_top_idx, 0, 0);
	Gui::DrawString(50, 0, 0.72f, WHITE, "Welcome to Universal-Manager!");


	drawTextBox();
	Gui::sprite(1, credits_rocketRobz_idx, 0, 40);
	Gui::DrawString(270, 50, 0.40, WHITE, "Hi!");
	Gui::DrawString(185, 65, 0.40, WHITE, "I'm RocketRobz, main developer of\n           TWiLight Menu++.\nSome code I made (not from TWLM++)\n       has made it into this app.\nEnjoy this universally good manager!");

	// BG Bottom Stuff.
	set_screen(bottom);
	Draw_Rect(0, 0, 400, 240, ROCKETROBZCOLORBG);
	C2D_DrawRectSolid(0, 0, 0.5f, 320, 25, ROCKETROBZCOLORBAR);
	Gui::sprite(0, sprites_universal_bg_bottom_idx, 0, 25);
	Gui::sprite(0, sprites_bottom_screen_top_idx, 0, 0);
	C2D_DrawRectSolid(0, 215, 0.5f, 320, 25, ROCKETROBZCOLORBAR);
	Gui::sprite(0, sprites_bottom_screen_bot_idx, 0, 215);
	Gui::DrawString((320-Gui::GetStringWidth(0.7f, "RocketRobz"))/2, 0, 0.7f, WHITE, "RocketRobz");
}

void drawTotallyNotGuy(void) {
	// Top BG Stuff.
	set_screen(top);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, TNGCOLORBG);
	Gui::sprite(0, sprites_universal_bg_top_idx, 0, 25);
	C2D_DrawRectSolid(0, 0, 0.5f, 400, 25, TNGCOLORBAR);
	Gui::sprite(0, sprites_top_screen_top_idx, 0, 0);
	Gui::DrawString(50, 0, 0.72f, WHITE, "Welcome to Universal-Manager!");


	drawTextBox();
	Gui::sprite(1, credits_totallyNotGuy_idx, 0, 40);
	Gui::DrawString(250, 50, 0.40, WHITE, "Hello!");
	Gui::DrawString(185, 65, 0.40, WHITE, "I'm TotallyNotGuy and I did \nmost of the graphics in \nUniversal-Manager and pkmn-chest. \nI hope you enjoy the graphics \nin both Apps!");

	// BG Bottom Stuff.
	set_screen(bottom);
	Draw_Rect(0, 0, 400, 240, TNGCOLORBG);
	C2D_DrawRectSolid(0, 0, 0.5f, 320, 25, TNGCOLORBAR);
	Gui::sprite(0, sprites_universal_bg_bottom_idx, 0, 25);
	Gui::sprite(0, sprites_bottom_screen_top_idx, 0, 0);
	C2D_DrawRectSolid(0, 215, 0.5f, 320, 25, TNGCOLORBAR);
	Gui::sprite(0, sprites_bottom_screen_bot_idx, 0, 215);
	Gui::DrawString((320-Gui::GetStringWidth(0.7f, "TotallyNotGuy"))/2, 0, 0.7f, WHITE, "TotallyNotGuy");
}

void Credits::drawCredits(void) const
{
	set_screen(top);
	Gui::sprite(1, credits_universal_credits_idx, 0, 0);
	if (fadealpha > 0) Draw_Rect(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect

	set_screen(bottom);
	Draw_Rect(0, 0, 400, 240, GRAY);
	C2D_DrawRectSolid(0, 0, 0.5f, 320, 25, BLUE);
	Gui::sprite(0, sprites_universal_bg_bottom_idx, 0, 25);
	Gui::sprite(0, sprites_bottom_screen_top_idx, 0, 0);
	C2D_DrawRectSolid(0, 215, 0.5f, 320, 25, BLUE);
	Gui::sprite(0, sprites_bottom_screen_bot_idx, 0, 215);
	Gui::DrawString(20, 0, 0.7f, WHITE, "Welcome to Universal-Manager!");
	if (fadealpha > 0) Draw_Rect(0, 0, 320, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
}

void Credits::drawCreditsDialogs(void) const
{
	if (dialog == 0) {
		drawVoltZ();
	} else if (dialog == 1) {
		drawFlame();
	} else if (dialog == 2) {
		drawPk11();
	} else if (dialog == 3) {
		drawRocketRobz();
	} else if (dialog == 4) {
		drawTotallyNotGuy();
	} else if (dialog == 5) {
		drawCredits();
	} else if (dialog == 6) {
		drawDiscordQR();
	}
}

void Credits::drawButtons(void) const
{
	Gui::sprite(3, button_button_tl_idx, 1, 27);
	Gui::sprite(3, button_button_2_idx, 162, 27);
	Gui::sprite(3, button_button_3_idx, 1, 91);
	Gui::sprite(3, button_button_4_idx, 162, 91);
	Gui::sprite(3, button_button_5_idx, 1, 154);
	Gui::sprite(3, button_button_br_idx, 162, 151);


	// Buttons.
	for(int i=0;i<3;i++) {
		Gui::DrawString(((320-Gui::GetStringWidth(0.65f, names[(2*i)].c_str()))/2)-85, 46+(i*65), 0.65f, WHITE, names[(2*i)].c_str());
		Gui::DrawString(((320-Gui::GetStringWidth(0.65f, names[(2*i)+1].c_str()))/2)+85, 46+(i*65), 0.65f, WHITE, names[(2*i)+1].c_str());
	}
}

void Credits::Loop() {
	gspWaitForVBlank();
	if(delay > 0) {
		delay--;
	} else {
		delay = 120;
		discordText = !discordText;
	}
}

void Credits::Draw(void) const
{
	C2D_TargetClear(top, GRAY);
	C2D_TargetClear(bottom, GRAY);
	set_screen(top);
	Gui::sprite(0, sprites_universal_bg_top_idx, 0, 25);
	Gui::sprite(0, sprites_top_screen_top_idx, 0, 0);
	Gui::DrawString(50, 0, 0.72f, WHITE, "Welcome to Universal-Manager!");
	drawCreditsDialogs();
	drawButtons();
	Gui::DrawString((320-Gui::GetStringWidth(0.55, discordText ? "Click here to show the QR Code." : "Join our Discord: https://discord.gg/KDJCfGF"))/2, 220, 0.55, WHITE, discordText ? "Click here to show the QR Code." : "Join our Discord: https://discord.gg/KDJCfGF", 320);
}

void Credits::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	Loop();
	if (hDown & KEY_TOUCH) {
		if (touching(touch, creditsButtonPos[0])) {
			if (dialog == 0) {
				isVoltZAlready = true;
			} else {
				isVoltZAlready = false;
			}
			dialog = 0;
			drawEasterEgg();
		} else if (touching(touch, creditsButtonPos[1])) {
			dialog = 1;
		} else if (touching(touch, creditsButtonPos[2])) {
			dialog = 2;
		} else if (touching(touch, creditsButtonPos[3])) {
			dialog = 3;
		} else if (touching(touch, creditsButtonPos[4])) {
			dialog = 4;
		} else if (touching(touch, creditsButtonPos[5])) {
			if (Config::Credits == 0) {
			} else if (Config::Credits == 1) {
				Config::Credits = 0;
				Config::setCredits();
			}
			Gui::setScreen(std::make_unique<MainMenu>());
		} else if (touching(touch, creditsButtonPos[6])) {
			dialog = 6;
		}
	} else if (hHeld & KEY_SELECT) {
		helperBox("Hello! I'm a Test Instruction! :P");
	}
}