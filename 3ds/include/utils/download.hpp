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

#pragma once

#include "common.hpp"

#define APP_TITLE "Universal-Manager"
#define V_STRING "3.1.0"

enum DownloadError {
	DL_ERROR_NONE = 0,
	DL_ERROR_WRITEFILE,
	DL_ERROR_ALLOC,
	DL_ERROR_STATUSCODE,
	DL_ERROR_GIT,
};

struct ThemeEntry {
	std::string downloadUrl;
	std::string name;
	std::string path;
	std::string sdPath;
};

Result downloadToFile(std::string url, std::string path);
Result downloadFromRelease(std::string url, std::string asset, std::string path);

void displayProgressBar();

/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void);

/**
 * Display "Please connect to Wi-Fi" for 2s.
 */
void notConnectedMsg(void);

/**
 * Display "Not Implemented Yet" for 2s.
 */
void notImplemented(void);

/**
 * Get info from the GitHub API about a Release.
 * repo is where to get from. (Ex. "RocketRobz/TWiLightMenu")
 * item is that to get from the API. (Ex. "tag_name")
 * @return the string from the API.
 */
std::string getLatestRelease(std::string repo, std::string item);

/**
 * Get info from the GitHub API about a Commit.
 * repo is where to get from. (Ex. "RocketRobz/TWiLightMenu")
 * item is that to get from the API. (Ex. "sha")
 * @return the string from the API.
 */
std::string getLatestCommit(std::string repo, std::string item);

/**
 * Get info from the GitHub API about a Commit.
 * repo is where to get from. (Ex. "RocketRobz/TWiLightMenu")
 * array is the array the item is in. (Ex. "commit")
 * item is that to get from the API. (Ex. "message")
 * @return the string from the API.
 */
std::string getLatestCommit(std::string repo, std::string array, std::string item);

/**
 * Get a GitHub directory's contents with the GitHub API.
 * repo is where to get from. (Ex. "DS-Homebrew/twlmenu-extras")
 * path is the path within the repo (Ex. "contents/_nds/TWiLightMenu/dsimenu/themes")
 * @return the string from the API.
 */
std::vector<ThemeEntry> getThemeList(std::string repo, std::string path);

/**
 * Update nds-bootstrap to the latest build.
 */
void updateBootstrap(bool nightly);

/**
 * Update TWiLight Menu++ to the latest build.
 */
void updateTWiLight(bool nightly);

/**
 * Update Universal-Manager to the latest build.
 */
void updateUniversalManager(bool nightly);

/**
 * Update Luma's boot.firm to latest build.
 */
void updateLuma(bool nightly);

/**
 * Update GodMode9.firm to latest Release.
 */
void downloadGodMode9(void);

/**
 * Download themes from Universal-Team/extras.
 */
void downloadThemes(void);

/**
 * Download scripts from Universal-Team/extras.
 */
void downloadScripts(void);

/**
 * Update DeadSkullzJr's cheat DB to the latest version.
 */
void updateCheats(void);

/**
 * Update PKMN-Chest to the latest Release Version.
 */
void updatePKMNChestRelease(void);

/**
 * Update PKMN-Chest to the latest Nightly Build.
 */
void updatePKMNChestNightly(void);

/**
 * Update Relaunch to the latest Nightly Build.
 */
void updateRelaunchNightly(void);

/**
 * Update Relaunch to the latest Release Version.
 */
void updateRelaunchRelease(void);

void downloadFailed(void);


/**
 * Update LeafEdit to the latest Nightly.
 */
void updateLeafEdit(void);

/**
 * Update LeafEdit to the latest Release.
 */
void updateLeafEditRelease(void);

/**
 * Download boxart from gametdb for all roms found on SD.
 */
void downloadBoxart(void);

/**
 * Update GBARunner2 to it's latest Nightly.
 */
void updateGBARunner2(void);

/**
 * Download Spritesheets for LeafEdit's custom Spritesheet Feature.
 */
void downloadSheets(void);

// Download the default LeafEdit Spritesheet package, if the files do not exist.
void leafEditSheets(void);