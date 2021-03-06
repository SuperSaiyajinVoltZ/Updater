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

#include "colors.hpp"

#include "lang/lang.h"

#include "utils/inifile.h"
#include "utils/settings.hpp"

#include <3ds.h>
#include <string>
#include <unistd.h>

using std::string;
using std::wstring;


static CIniFile settingsini( "sdmc:/Universal-Manager/Settings.ini" );

int Config::barColor, Config::bgColor, Config::musicMode, Config::percentDisplay, Config::layout, Config::layoutBG; // [UI]
int Config::animation, Config::animationColor; // [ANIMATED]
int Config::selectedText, Config::unselectedText; // [TEXTCOLOR]
int Config::Credits; // [CREDITS]
int Config::selector;
int Config::Points;
int Config::FastMode; // Animation speed of the animations.
int Config::Logging; // Enable / Disable Logging.
int Config::Language;

void Config::loadConfig() {
	// [UI]
	Config::barColor = settingsini.GetInt("UI", "BARS", BARCOLOR);
	Config::bgColor = settingsini.GetInt("UI", "BG", GRAY);
	Config::musicMode = settingsini.GetInt("UI", "MUSIC", 0);
	Config::percentDisplay = settingsini.GetInt("UI", "BATTERY", 0);
	Config::layout = settingsini.GetInt("UI", "LAYOUT", 0);
	Config::layoutBG = settingsini.GetInt("UI", "LAYOUTBG", 0);
	Config::selector = settingsini.GetInt("UI", "SELECTOR", 0);
	Config::Language = settingsini.GetInt("UI", "LANGUAGE", 1); // English by default.

	// [ANIMATED]
	Config::animation = settingsini.GetInt("ANIMATED", "ENABLE", 1);
	Config::animationColor = settingsini.GetInt("ANIMATED", "COLOR", BLUE);
	Config::FastMode = settingsini.GetInt("ANIMATED", "FASTMODE", 0);

	// [TEXTCOLOR]
	Config::selectedText = settingsini.GetInt("TEXTCOLOR", "SELECTED", BLUE);
	Config::unselectedText = settingsini.GetInt("TEXTCOLOR", "UNSELECTED", BLACK);

	// [MISC]
	Config::Logging = settingsini.GetInt("MISC", "LOGGING", 0);

	// [CREDITS]
	Config::Credits = settingsini.GetInt("CREDITS", "ENABLE", 1); // Show's the Credits Screen at startup if 1.
}

void Config::saveConfig() {
	// [UI]
	settingsini.SetInt("UI", "BARS", Config::barColor);
	settingsini.SetInt("UI", "BG", Config::bgColor);
	settingsini.SetInt("UI", "MUSIC", Config::musicMode);
	settingsini.SetInt("UI", "BATTERY", Config::percentDisplay);
	settingsini.SetInt("UI", "LAYOUT", Config::layout);
	settingsini.SetInt("UI", "LAYOUTBG", Config::layoutBG);
	settingsini.SetInt("UI", "SELECTOR", Config::selector);
	settingsini.SetInt("UI", "LANGUAGE", Config::Language);

	// [ANIMATED]
	settingsini.SetInt("ANIMATED", "ENABLE", Config::animation);
	settingsini.SetInt("ANIMATED", "COLOR", Config::animationColor);
	settingsini.SetInt("ANIMATED", "FASTMODE", Config::FastMode);

	// [TEXTCOLOR]
	settingsini.SetInt("TEXTCOLOR", "SELECTED", Config::selectedText);
	settingsini.SetInt("TEXTCOLOR", "UNSELECTED", Config::unselectedText);

	// [MISC]
	settingsini.SetInt("MISC", "LOGGING", Config::Logging);

	settingsini.SaveIniFile("sdmc:/Universal-Manager/Settings.ini");
}

void Config::setCredits() {
	// [CREDITS]
	settingsini.SetInt("CREDITS", "ENABLE", Config::Credits);
	settingsini.SaveIniFile("sdmc:/Universal-Manager/Settings.ini");
}

void Config::loadPoints() {
	Config::Points = settingsini.GetInt("PONG", "POINTS", 0); // The Points from Pong.
}


void Config::setPongPoints(int points) {
	settingsini.SetInt("PONG", "POINTS", points);
	settingsini.SaveIniFile("sdmc:/Universal-Manager/Settings.ini");
}

void Config::setLang() {
	settingsini.SetInt("UI", "LANGUAGE", Config::Language);
	settingsini.SaveIniFile("sdmc:/Universal-Manager/Settings.ini");
	Lang::refreshLanguage(Config::Language);
}