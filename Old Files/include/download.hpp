

#pragma once

#include "common.hpp"

#define APP_TITLE "Universal-Updater"
#define VERSION_STRING "3.2.0"

enum DownloadError {
	DL_ERROR_NONE = 0,
	DL_ERROR_WRITEFILE,
	DL_ERROR_ALLOC,
	DL_ERROR_STATUSCODE,
	DL_ERROR_GIT,
};

Result downloadToFile(std::string url, std::string path);
Result downloadFromRelease(std::string url, std::string asset, std::string path);

/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void);

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
 * Show the latest release's name and message.
 * repo is where to get from. (Ex. "RocketRobz/TWiLightMenu")
 */
void showReleaseInfo(std::string repo);

/**
 * Show the latest commit's name and message.
 * repo is where to get from. (Ex. "RocketRobz/TWiLightMenu")
 */
void showCommitInfo(std::string repo);

/**
 * Prepare text for showing a release/commit message.
 * text is the text you want to show.
 */
void setMessageText(const std::string &text);

/**
 * Draw text prepared by setMessageText.
 * position is which line start on.
 */
void drawMessageText(int position);

/**
 * Check for updates.
 */
void checkForUpdates(void);

/**
 * Update nds-bootstrap to the latest build.
 */
void updateBootstrap(bool nightly);

/**
 * Update TWiLight Menu++ to the latest build.
 */
void updateTWiLight(bool nightly);


/**
 * Update DeadSkullzJr's cheat DB to the latest version.
 */
void updateCheats(void);

/**
 * Download boxart from gametdb for all roms found on SD.
 */
void downloadBoxart(void);

/**
 * Update PKSM!
 */
void updatePKSM(bool nightly);

/**
 * Update die CFW/Boot.firm!
 */
void updateLuma(bool nightly);
