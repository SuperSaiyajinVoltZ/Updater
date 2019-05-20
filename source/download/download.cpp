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

#include "download/download.hpp"
#include <sys/stat.h>
#include <vector>
#include <unistd.h>

#include "download/extract.hpp"
#include "inifile.h"
#include "graphic.h"
#include "fileBrowse.h"
#include "voltlib/volt.h"
#include "universal-Settings.hpp"
#include "colors.hpp"
#include "download/thread.hpp"

extern "C" {
	#include "download/cia.h"
}

#define  USER_AGENT   APP_TITLE "-" VERSION_STRING

static char* result_buf = NULL;
static size_t result_sz = 0;
static size_t result_written = 0;
std::vector<std::string> _topText;
std::string jsonName;

extern bool downloadNightlies;
extern int filesExtracted;
extern std::string extractingFile;

char progressBarMsg[64] = "";
bool showProgressBar = false;
bool progressBarType = 0; // 0 = Download | 1 = Extract

// following function is from 
// https://github.com/angelsl/libctrfgh/blob/master/curl_test/src/main.c
static size_t handle_data(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    (void) userdata;
    const size_t bsz = size*nmemb;

    if (result_sz == 0 || !result_buf)
    {
        result_sz = 0x1000;
        result_buf = (char*)malloc(result_sz);
    }

    bool need_realloc = false;
    while (result_written + bsz > result_sz) 
    {
        result_sz <<= 1;
        need_realloc = true;
    }

    if (need_realloc)
    {
        char *new_buf = (char*)realloc(result_buf, result_sz);
        if (!new_buf)
        {
            return 0;
        }
        result_buf = new_buf;
    }

    if (!result_buf)
    {
        return 0;
    }

    memcpy(result_buf + result_written, ptr, bsz);
    result_written += bsz;
    return bsz;
}

static Result setupContext(CURL *hnd, const char * url)
{
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL, url);
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, handle_data);
    curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(hnd, CURLOPT_STDERR, stdout);

    return 0;
}

Result downloadToFile(std::string url, std::string path)
{
	Result ret = 0;	
	printf("Downloading from:\n%s\nto:\n%s\n", url.c_str(), path.c_str());

    void *socubuf = memalign(0x1000, 0x100000);
    if (!socubuf)
    {
        return -1;
    }

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
    {
		free(socubuf);
        return ret;
    }

	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, url.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return ret;
	}

	Handle fileHandle;
	u64 offset = 0;
	u32 bytesWritten = 0;
	
	ret = openFile(&fileHandle, path.c_str(), true);
	if (R_FAILED(ret)) {
		printf("Error: couldn't open file to write.\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return DL_ERROR_WRITEFILE;
	}
	
	u64 startTime = osGetTime();

	CURLcode cres = curl_easy_perform(hnd);
    curl_easy_cleanup(hnd);

	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return -1;
	}
	
	FSFILE_Write(fileHandle, &bytesWritten, offset, result_buf, result_written, 0);

	u64 endTime = osGetTime();
	u64 totalTime = endTime - startTime;
	printf("Download took %llu milliseconds.\n", totalTime);

    socExit();
    free(result_buf);
    free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;
	FSFILE_Close(fileHandle);
	return 0;
}

Result downloadFromRelease(std::string url, std::string asset, std::string path)
{
	Result ret = 0;
    void *socubuf = memalign(0x1000, 0x100000);
    if (!socubuf)
    {
        return -1;
    }

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
    {
		free(socubuf);
        return ret;
    }

	std::regex parseUrl("github\\.com\\/(.+)\\/(.+)");
	std::smatch result;
	regex_search(url, result, parseUrl);
	
	std::string repoOwner = result[1].str(), repoName = result[2].str();
	
	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repoOwner << "/" << repoName << "/releases/latest";
	std::string apiurl = apiurlStream.str();
	
	printf("Downloading latest release from repo:\n%s\nby:\n%s\n", repoName.c_str(), repoOwner.c_str());
	printf("Crafted API url:\n%s\n", apiurl.c_str());
	
	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return ret;
	}

	CURLcode cres = curl_easy_perform(hnd);
    curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string
	
	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return -1;
	}
	
	printf("Looking for asset with matching name:\n%s\n", asset.c_str());
	std::string assetUrl;
	json parsedAPI = json::parse(result_buf);
	if (parsedAPI["assets"].is_array()) {
		for (auto jsonAsset : parsedAPI["assets"]) {
			if (jsonAsset.is_object() && jsonAsset["name"].is_string() && jsonAsset["browser_download_url"].is_string()) {
				std::string assetName = jsonAsset["name"];
				if (matchPattern(asset, assetName)) {
					assetUrl = jsonAsset["browser_download_url"];
					break;
				}
			}
		}
	}
    socExit();
    free(result_buf);
    free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;
	
	if (assetUrl.empty())
		ret = DL_ERROR_GIT;
	else
		ret = downloadToFile(assetUrl, path);
	
	return ret;
}

/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void) {
	u32 wifiStatus;
	bool res = false;

	if (R_SUCCEEDED(ACU_GetWifiStatus(&wifiStatus)) && wifiStatus) {
		res = true;
	}
	
	return res;
}

void downloadFailed(void) {
	displayMsg("Download failed!\n");
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

void notImplemented(void) {
	displayMsg("Not implemented Yet.\n");
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

void doneMsg(void) {
	displayMsg("Done!");
	for (int i = 0; i < 60*2; i++) {
		gspWaitForVBlank();
	}
}

std::string getLatestRelease(std::string repo, std::string item)
{
	Result ret = 0;
    void *socubuf = memalign(0x1000, 0x100000);
    if (!socubuf)
    {
        return "";
    }

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
    {
		free(socubuf);
        return "";
    }
	
	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/releases/latest";
	std::string apiurl = apiurlStream.str();
	
	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	CURLcode cres = curl_easy_perform(hnd);
    curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string
	
	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}
	
	std::string jsonItem;
	json parsedAPI = json::parse(result_buf);
	if (parsedAPI[item].is_string()) {
		jsonItem = parsedAPI[item];
	}
    socExit();
    free(result_buf);
    free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItem;
}

std::string getLatestCommit(std::string repo, std::string item)
{
	Result ret = 0;
    void *socubuf = memalign(0x1000, 0x100000);
    if (!socubuf)
    {
        return "";
    }

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
    {
		free(socubuf);
        return "";
    }
	
	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/commits/master";
	std::string apiurl = apiurlStream.str();
	
	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	CURLcode cres = curl_easy_perform(hnd);
    curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string
	
	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}
	
	std::string jsonItem;
	json parsedAPI = json::parse(result_buf);
	if (parsedAPI[item].is_string()) {
		jsonItem = parsedAPI[item];
	}
    socExit();
    free(result_buf);
    free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItem;
}

std::string getLatestCommit(std::string repo, std::string array, std::string item)
{
	Result ret = 0;
    void *socubuf = memalign(0x1000, 0x100000);
    if (!socubuf)
    {
        return "";
    }

	ret = socInit((u32*)socubuf, 0x100000);
	if (R_FAILED(ret))
    {
		free(socubuf);
        return "";
    }
	
	std::stringstream apiurlStream;
	apiurlStream << "https://api.github.com/repos/" << repo << "/commits/master";
	std::string apiurl = apiurlStream.str();
	
	CURL *hnd = curl_easy_init();
	ret = setupContext(hnd, apiurl.c_str());
	if (ret != 0) {
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}

	CURLcode cres = curl_easy_perform(hnd);
    curl_easy_cleanup(hnd);
	char* newbuf = (char*)realloc(result_buf, result_written + 1);
	result_buf = newbuf;
	result_buf[result_written] = 0; //nullbyte to end it as a proper C style string
	
	if (cres != CURLE_OK) {
		printf("Error in:\ncurl\n");
		socExit();
		free(result_buf);
		free(socubuf);
		result_buf = NULL;
		result_sz = 0;
		result_written = 0;
		return "";
	}
	
	std::string jsonItem;
	json parsedAPI = json::parse(result_buf);
	if (parsedAPI[array][item].is_string()) {
		jsonItem = parsedAPI[array][item];
	}
    socExit();
    free(result_buf);
    free(socubuf);
	result_buf = NULL;
	result_sz = 0;
	result_written = 0;

	return jsonItem;
}

bool showReleaseInfo(std::string repo, bool showExitText)
{
	jsonName = getLatestRelease(repo, "name");
	std::string jsonBody = getLatestRelease(repo, "body");
	
	setMessageText(jsonBody);
	int textPosition = 0;
	bool redrawText = true;

	while(1) {
		if(redrawText) {
			drawMessageText(textPosition, showExitText);
			redrawText = false;
		}

		gspWaitForVBlank();
		hidScanInput();
		const u32 hDown = hidKeysDown();
		const u32 hHeld = hidKeysHeld();

		if(hHeld & KEY_UP || hHeld & KEY_DOWN) {
			for(int i=0;i<9;i++)
				gspWaitForVBlank();
		}
		
		if (hDown & KEY_A || (hDown & KEY_Y && !showExitText) || (hDown & KEY_TOUCH && !showExitText)) {
			return true;
		} else if (hDown & KEY_B) {
			return false;
		} else if (hHeld & KEY_UP) {
			if(textPosition > 0) {
				textPosition--;
				redrawText = true;
			}
		} else if (hHeld & KEY_DOWN) {
			if(textPosition < (int)(_topText.size() - 10)) {
				textPosition++;
				redrawText = true;
			}
		}
	}
}

bool showCommitInfo(std::string repo, bool showExitText)
{
	jsonName = getLatestCommit(repo, "sha").substr(0,7);
	std::string jsonBody = getLatestCommit(repo, "commit", "message");
	setMessageText(jsonBody);
	int textPosition = 0;
	bool redrawText = true;

	while(1) {
		if(redrawText) {
			drawMessageText(textPosition, showExitText);
			redrawText = false;
		}

		gspWaitForVBlank();
		hidScanInput();
		const u32 hDown = hidKeysDown();
		const u32 hHeld = hidKeysHeld();

		if(hHeld & KEY_UP || hHeld & KEY_DOWN) {
			for(int i=0;i<9;i++)
				gspWaitForVBlank();
		}
		
		if (hDown & KEY_A || (hDown & KEY_Y && !showExitText) || (hDown & KEY_TOUCH && !showExitText)) {
			return true;
		} else if (hDown & KEY_B) {
			return false;
		} else if (hHeld & KEY_UP) {
			if(textPosition > 0) {
				textPosition--;
				redrawText = true;
			}
		} else if (hHeld & KEY_DOWN) {
			if(textPosition < (int)(_topText.size() - 10)) {
				textPosition++;
				redrawText = true;
			}
		}
	}
}

void setMessageText(const std::string &text)
{
	std::string _topTextStr(text);
	std::vector<std::string> words;
	std::size_t pos;
	// std::replace( _topTextStr.begin(), _topTextStr.end(), '\n', ' ');
	_topTextStr.erase(std::remove(_topTextStr.begin(), _topTextStr.end(), '\r'), _topTextStr.end());
	while((pos = _topTextStr.find(' ')) != std::string::npos) {
		words.push_back(_topTextStr.substr(0, pos));
		_topTextStr = _topTextStr.substr(pos + 1);
	}
	if(_topTextStr.size())
		words.push_back(_topTextStr);
	std::string temp;
	_topText.clear();
	for(auto word : words)
	{
		int width = volt_get_text_width((temp + " " + word).c_str(), 0.45f, 0.45f);
		if(word.find('\n') != -1u)
		{
			word.erase(std::remove(word.begin(), word.end(), '\n'), word.end());
			temp += " " + word;
			_topText.push_back(temp);
			temp = "";
		}
		else if(width > 256)
		{
			_topText.push_back(temp);
			temp = word;
		}
		else
		{
			temp += " " + word;
		}
	}
	if(temp.size())
	   _topText.push_back(temp);
}

void drawMessageText(int position, bool showExitText)
{
	if(showExitText)
	volt_draw_text(30, 200, 0.5f, 0.5f, WHITE, "\uE001: Cancel   \uE000: Update");
	volt_begin_draw(GFX_TOP, GFX_LEFT);
	volt_draw_text(25, 24, .7, .7, WHITE, jsonName.c_str());
	for (int i = 0; i < (int)_topText.size() && i < (showExitText ? 9 : 10); i++) {
	volt_draw_text(24, ((i * 16) + 48), 0.45f, 0.45f, WHITE, _topText[i+position].c_str());
	volt_end_draw();
}
}

void displayProgressBar() {
	char str[256];
	while(showProgressBar) {
		snprintf(str, sizeof(str), "%s\n%s%s\n%i %s", progressBarMsg, (!progressBarType ? "" : "\nCurrently extracting:\n"), (!progressBarType ? "" : extractingFile.c_str()), (!progressBarType ? (int)round(result_written/1000) : filesExtracted), (!progressBarType ? "KB downloaded." : (filesExtracted == 1 ? "file extracted." :"files extracted.")));
		displayMsg(str);
		gspWaitForVBlank();
	}
}

void updateBootstrap(bool nightly) {
	if(nightly) {
		snprintf(progressBarMsg, sizeof(progressBarMsg), "Downloading nds-bootstrap...\n(Nightly)");
		showProgressBar = true;
		progressBarType = 0;
		 Threads::create((ThreadFunc)displayProgressBar);
		if (downloadToFile("https://github.com/TWLBot/Builds/blob/master/nds-bootstrap.7z?raw=true", "/nds-bootstrap-nightly.7z") != 0) {
			showProgressBar = false;
			downloadFailed();
			return;
		}

		snprintf(progressBarMsg, sizeof(progressBarMsg), "Extracting nds-bootstrap...\n(Nightly)");
		filesExtracted = 0;
		progressBarType = 1;
		extractArchive("/nds-bootstrap-nightly.7z", "nds-bootstrap/", "/_nds/");
		showProgressBar = false;

		deleteFile("sdmc:/nds-bootstrap-nightly.7z");
	} else {	
		displayMsg("Downloading nds-bootstrap...\n(Release)");
		snprintf(progressBarMsg, sizeof(progressBarMsg), "Downloading nds-bootstrap...\n(Release)");
		showProgressBar = true;
		progressBarType = 0;
		Threads::create((ThreadFunc)displayProgressBar);
		if (downloadFromRelease("https://github.com/ahezard/nds-bootstrap", "nds-bootstrap\\.zip", "/nds-bootstrap-release.zip") != 0) {
			showProgressBar = false;
			downloadFailed();
			return;
		}

		snprintf(progressBarMsg, sizeof(progressBarMsg), "Extracting nds-bootstrap...\n(Release)");
		filesExtracted = 0;
		progressBarType = 1;
		extractArchive("/nds-bootstrap-release.zip", "/", "/_nds/");
		showProgressBar = false;

		deleteFile("sdmc:/nds-bootstrap-release.zip");
	}
	doneMsg();
}


void updateTWiLight(bool nightly) {
	if(nightly) {
		snprintf(progressBarMsg, sizeof(progressBarMsg),"Now Downloading TWiLightMenu++\n"
						"(Nightly)\n\nThis may take a while.");
		showProgressBar = true;
		progressBarType = 0;
		 Threads::create((ThreadFunc)displayProgressBar);
		if (downloadToFile("https://github.com/TWLBot/Builds/blob/master/TWiLightMenu.7z?raw=true", "/TWiLightMenu-nightly.7z") != 0) {
			showProgressBar = false;
			downloadFailed();
			return;
		}

		snprintf(progressBarMsg, sizeof(progressBarMsg), "Now extracting.\n"
						"(Nightly)\n\nThis may take a while.");
		filesExtracted = 0;
		progressBarType = 1;
		extractArchive("/TWiLightMenu-nightly.7z", "TWiLightMenu/_nds/", "/_nds/");
		extractArchive("/TWiLightMenu-nightly.7z", "3DS - CFW users/", "/");
		showProgressBar = false;

		displayMsg("Now installing the CIAs.\n"
						"(Nightly)");
		// installCia("/TWiLight Menu.cia");
		installCia("/TWiLight Menu - Game booter.cia");

		deleteFile("sdmc:/TWiLightMenu-nightly.7z");
		deleteFile("sdmc:/TWiLight Menu.cia");
		deleteFile("sdmc:/TWiLight Menu - Game booter.cia");
	} else {
		displayMsg("Downloading TWiLightMenu++\n"
						"(Release)\n\nThis may take a while.");
		snprintf(progressBarMsg, sizeof(progressBarMsg), "Downloading TWiLightMenu++\n"
						"(Release)\n\nThis may take a while.");
		showProgressBar = true;
		progressBarType = 0;
		Threads::create((ThreadFunc)displayProgressBar);
		if (downloadFromRelease("https://github.com/DS-Homebrew/TWiLightMenu", "TWiLightMenu\\.7z", "/TWiLightMenu-release.7z") != 0) {
			showProgressBar = false;
			downloadFailed();
			return;
		}

		snprintf(progressBarMsg, sizeof(progressBarMsg), "Now extracting.\n"
						"(Release)\n\nThis may take a while.");
		filesExtracted = 0;
		progressBarType = 1;
		extractArchive("/TWiLightMenu-release.7z", "_nds/", "/_nds/");
		extractArchive("/TWiLightMenu-release.7z", "3DS - CFW users/", "/");
		extractArchive("/TWiLightMenu-release.7z", "DSi&3DS - SD card users/", "/");
		showProgressBar = false;

		displayMsg("Now installing the CIAs.\n"
						"(Release)");
		installCia("/TWiLight Menu.cia");
		installCia("/TWiLight Menu - Game booter.cia");

		deleteFile("sdmc:/TWiLightMenu-release.7z");
		deleteFile("sdmc:/TWiLight Menu.cia");
		deleteFile("sdmc:/TWiLight Menu - Game booter.cia");
	}
	doneMsg();
}

void updateUniversalManager(bool nightly) {
	if(nightly) {
		notImplemented();
	} else {
		notImplemented();
}
}