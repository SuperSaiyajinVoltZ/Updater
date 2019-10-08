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
#include "screens/scriptScreen.hpp"
#include "utils/fileBrowse.h"
#include "utils/keyboard.hpp"
#include "utils/scripts.hpp"
#include "utils/settings.hpp"
#include "utils/sound.h"


extern bool touching(touchPosition touch, Structs::ButtonPos button);

void Script::Draw(void) const
{
	if (ScriptMode == 0) {
		DrawScriptBrowse();
	} else if (ScriptMode == 1) {
		DrawScriptCreator();
	}
}

void Script::Logic(u32 hDown, u32 hHeld, touchPosition touch)
{
	if (ScriptMode == 0) {
		ScriptBrowseLogic(hDown, hHeld, touch);
	} else if (ScriptMode == 1) {
		ScriptCreatorLogic(hDown, hHeld, touch);
	}
}


void Script::DrawScriptBrowse(void) const
{
	Gui::DrawFileBrowseBG();
	animatedBGTop();
	Gui::DrawBarsTop();
	DisplayTime();
	drawBatteryTop();
	char path[PATH_MAX];
	getcwd(path, PATH_MAX);
	Gui::DrawString((400-(Gui::GetStringWidth(0.60f, path)))/2, 218, 0.60f, WHITE, path);
	Gui::DrawString((400-Gui::GetStringWidth(0.72f, "Script Main Screen"))/2, 0, 0.72f, WHITE, "Script Main Screen");
	mkdir("sdmc:/Universal-Manager/scripts/", 0777);

	std::string dirs;
	for (uint i=(selectedFile<5) ? 0 : selectedFile-5;i<dirContents.size()&&i<((selectedFile<5) ? 6 : selectedFile+1);i++) {
		if (i == selectedFile) {
			dirs += "> " + dirContents[i].name + "\n\n";
		} else {
			dirs +=  dirContents[i].name + "\n\n";
		}
	}
	for (uint i=0;i<((dirContents.size()<6) ? 6-dirContents.size() : 0);i++) {
		dirs += "\n\n";
	}

	Gui::DrawString(26, 32, 0.53f, BLACK, dirs.c_str());

	Gui::DrawBGBot();
	animatedBGBot();
	Gui::DrawBarsBot();
}

void Script::ScriptBrowseLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (keyRepeatDelay)	keyRepeatDelay--;

			if (dirChanged) {
				dirContents.clear();
				std::vector<DirEntry> dirContentsTemp;
				getDirectoryContents(dirContentsTemp, {"scpt"});
				for(uint i=0;i<dirContentsTemp.size();i++) {
					dirContents.push_back(dirContentsTemp[i]);
				}
			dirChanged = false;
			}

			if (refresh) {
				dirContents.clear();
				std::vector<DirEntry> dirContentsTemp;
				getDirectoryContents(dirContentsTemp, {"scpt"});
				for(uint i=0;i<dirContentsTemp.size();i++) {
					dirContents.push_back(dirContentsTemp[i]);
				}
			refresh = false;
			}

		if(hDown & KEY_A) {
			if (dirContents[selectedFile].isDirectory) {
				chdir(dirContents[selectedFile].name.c_str());
				selectedFile = 0;
				dirChanged = true;
			} else {
				if (dirContents.size() == 0) {
					DisplayTimeMessage("What are you trying to do? :P");
				} else {
					char path[PATH_MAX];
					getcwd(path, PATH_MAX);
					std::string scriptPath = path;
					std::string newPath;
					if (scriptPath == "/") {
						newPath = "sdmc:";
						newPath += scriptPath;
					} else {
						newPath = scriptPath;
					}
					newPath += dirContents[selectedFile].name;
					if(confirmPopup("Do you want to run this Script : \n\n "+dirContents[selectedFile].name+"")) {
						runScript(newPath);
					}
				}
			}
		} else if (hDown & KEY_B) {
			char path[PATH_MAX];
			getcwd(path, PATH_MAX);
			if(strcmp(path, "sdmc:/") == 0 || strcmp(path, "/") == 0) {
				Gui::screenBack();
				return;
			} else {
				chdir("..");
				selectedFile = 0;
				dirChanged = true;
			}
		} else if (hDown & KEY_Y) {
			char path[PATH_MAX];
			getcwd(path, PATH_MAX);
			std::string scriptPath = path;
			std::string newPath;
			if (scriptPath == "/") {
				newPath = "sdmc:";
				newPath += scriptPath;
			} else {
				newPath = scriptPath;
			}

			std::string newScript = Input::getLine("Please type in the new Script's name.");
			if(newScript != "") {
				newPath += "/";
				newPath += newScript;
				newPath += ".scpt";
				FILE* scpt = fopen(newPath.c_str(), "w");
				fclose(scpt);
				refresh = true;
		}
		} else if (hDown & KEY_X) {
			if (dirContents.size() == 0) {
				DisplayTimeMessage("What are you trying to do? :P");
			} else {
				if ((strcasecmp(dirContents[selectedFile].name.substr(dirContents[selectedFile].name.length()-4, 4).c_str(), "scpt") == 0)) {
						char path[PATH_MAX];
						getcwd(path, PATH_MAX);
						std::string scriptPath = path;
						std::string newPath;
						if (scriptPath == "/") {
							newPath = "sdmc:";
							newPath += scriptPath;
						} else {
							newPath = scriptPath;
						}
						newPath += dirContents[selectedFile].name;
					if(confirmPopup("Are you sure you want to delete this Script?")) {
						remove(newPath.c_str());
						refresh = true;
					}
				} else {
					DisplayTimeMessage("This is not a '.scpt' File!");
				}
			}
		} else if (hHeld & KEY_UP) {
			if (selectedFile > 0 && !keyRepeatDelay) {
				selectedFile--;
				playScrollSfx();
			if (fastMode == true) {
				keyRepeatDelay = 3;
			} else if (fastMode == false){
				keyRepeatDelay = 6;
			}
			}
		} else if (hHeld & KEY_DOWN && !keyRepeatDelay) {
			if (selectedFile < dirContents.size()-1) {
				selectedFile++;
				playScrollSfx();
			if (fastMode == true) {
				keyRepeatDelay = 3;
			} else if (fastMode == false){
				keyRepeatDelay = 6;
			}
			}
		} else if (hDown & KEY_START) {
			if ((strcasecmp(dirContents[selectedFile].name.substr(dirContents[selectedFile].name.length()-4, 4).c_str(), "scpt") == 0)) {
				char path[PATH_MAX];
				getcwd(path, PATH_MAX);
				std::string scriptPath = path;
				std::string newPath;
				if (scriptPath == "/") {
					newPath = "sdmc:";
					newPath += scriptPath;
				} else {
					newPath = scriptPath;
				}
				newPath += dirContents[selectedFile].name;
				if(confirmPopup("Do you want to edit this Script : \n\n "+dirContents[selectedFile].name+"")) {
					scpt.open(newPath.c_str(), std::ofstream::app);
					Selection = 0;
					ScriptPage = 1;
					ScriptMode = 1;
				}
			} else {
				DisplayTimeMessage("This is not a '.scpt' File!");
			}
		} else if (hDown & KEY_R) {
			fastMode = true;
		} else if (hDown & KEY_L) {
			fastMode = false;
		} else if (hHeld & KEY_SELECT) {
			helperBox(" Press A to start the selected Script. \n \n Press B to return to the Main Menu Screen. \n \n Press X to Delete the selected scpt File. \n \n Press Y to create scpt Files.");
		}
}

void Script::DrawScriptCreator(void) const
{
	Gui::DrawBGTop();
	animatedBGTop();
	Gui::DrawBarsTop();
	DisplayTime();
	drawBatteryTop();
	Gui::DrawString((400-Gui::GetStringWidth(0.72f, "Script Creator"))/2, 0, 0.72f, WHITE, "Script Creator");
	Gui::DrawBGBot();
	animatedBGBot();
	Gui::DrawBarsBot();
	DrawBottom();
	Gui::DrawString(150, 0, 0.50f, WHITE, "Current Page:");
	Gui::DrawString(260, 4, 0.50, BLACK, "1");
	Gui::DrawString(280, 4, 0.50, BLACK, "2");
	DrawSelection();
	DrawCurrentPage();
}

void Script::DrawBottom(void) const
{
	if (ScriptPage == 1) {
		Gui::sprite(3, button_button_tl_idx, 1, 27);
		Gui::DrawString(8, 42, 0.65f, WHITE, "downloadRelease");

		Gui::sprite(3, button_button_2_idx, 162, 27);
		Gui::DrawString(190, 42, 0.7f, WHITE, "downloadFile");

		Gui::sprite(3, button_button_3_idx, 1, 91);
		Gui::DrawString(45, 107, 0.65f, WHITE, "extract");

		Gui::sprite(3, button_button_4_idx, 162, 91);
		Gui::DrawString(222, 104, 0.7f, WHITE, "install");

		Gui::sprite(3, button_button_5_idx, 1, 154);
		Gui::DrawString(45, 177, 0.7f, WHITE, "delete");

		Gui::sprite(3, button_button_br_idx, 162, 151);
		Gui::DrawString(225, 177, 0.7f, WHITE, "msg");


	} else if (ScriptPage == 2) {
		Gui::sprite(3, button_button_tl_idx, 1, 27);
		Gui::DrawString(50, 42, 0.65f, WHITE, "mkdir");

		Gui::sprite(3, button_button_2_idx, 162, 27);
		Gui::DrawString(190, 42, 0.7f, WHITE, "progressFile");

		Gui::sprite(3, button_button_3_idx, 1, 91);
		Gui::DrawString(16, 107, 0.65f, WHITE, "progressRelease");

		Gui::sprite(3, button_button_4_idx, 162, 91);
		Gui::DrawString(180, 104, 0.7f, WHITE, "progressExtract");

		Gui::sprite(3, button_button_5_idx, 1, 154);
		Gui::DrawString(45, 174, 0.7f, WHITE, "waitMsg");

		Gui::sprite(3, button_button_br_idx, 162, 151);
		Gui::DrawString(210, 174, 0.7f, WHITE, "timeMsg");
	}
}

void Script::DrawSelection(void) const
{
		if (ScriptPage == 1) {
			if (Selection == 0) {
				Gui::drawGUISelector(button_tl_selector_idx, 0, 26, .020f);
			} else if (Selection == 1) {
				Gui::drawGUISelector(button_selector_idx, 166, 31, .020f);
			} else if (Selection == 2) {
				Gui::drawGUISelector(button_selector_idx, 3, 95, .020f);
			} else if (Selection == 3) {
				Gui::drawGUISelector(button_selector_idx, 166, 95, .020f);
			} else if (Selection == 4) {
				Gui::drawGUISelector(button_selector_idx, 3, 158, .020f);
			} else if (Selection == 5) {
				Gui::drawGUISelector(button_br_selector_idx, 161, 153, .020f);
			}
		} else if (ScriptPage == 2) {
			if (Selection == 0) {
				Gui::drawGUISelector(button_tl_selector_idx, 0, 26, .020f);
			} else if (Selection == 1) {
				Gui::drawGUISelector(button_selector_idx, 166, 31, .020f);
			} else if (Selection == 2) {
				Gui::drawGUISelector(button_selector_idx, 3, 95, .020f);
			} else if (Selection == 3) {
				Gui::drawGUISelector(button_selector_idx, 166, 95, .020f);
			} else if (Selection == 4) {
				Gui::drawGUISelector(button_selector_idx, 3, 158, .020f);
			} else if (Selection == 5) {
				Gui::drawGUISelector(button_br_selector_idx, 161, 153, .020f);
			}
		}
}

void Script::DrawCurrentPage(void) const
{
	if (ScriptPage == 1) {
		Gui::DrawString(260, 4, 0.50, WHITE, "1");
		Gui::sprite(0, sprites_frame_idx, 256, 4);
	} else if (ScriptPage == 2) {
		Gui::DrawString(280, 4, 0.50, WHITE, "2");
		Gui::sprite(0, sprites_frame_idx, 276, 4);
	}
}

void Script::ScriptCreatorSelectionLogic(u32 hDown, u32 hHeld) {
		if (hHeld & KEY_SELECT) {
			if (ScriptPage == 1) {
				helperBox("downloadRelease : Download a File from Github's Release.\n\ndownloadFile : Download a File from a URL.\n\nextract : Extract an Archive.\n\ninstall : Install a CIA File from the SD Card.\n\ndelete : Delete a FILE from the SD Card.\n\nmsg : Displays a Message on the Top Screen.");
			} else if (ScriptPage == 2) {
				helperBox("mkdir : Creates a Folder.\n\nprogressFile: Same as downloadFile, but with\n a progressbar.\n\nprogressRelease: Same as downloadRelease\n but with a progressbar.");
			}
		} else if (hDown & KEY_UP) {
			if(Selection > 1)	Selection -= 2;

		} else if (hDown & KEY_DOWN) {
				if(Selection < 4)	Selection += 2;

		} else if (hDown & KEY_LEFT) {
			if (Selection%2) Selection--;
		} else if (hDown & KEY_RIGHT) {
				if (!(Selection%2)) Selection++;
		} else if (hDown & KEY_A) {
			if (ScriptPage == 1) {
			switch(Selection) {
				case 0: {
					std::string Function = "downloadRelease	";
					std::string param1 = Input::getLine("Please type in the Github Repo URL.");
					std::string param2 = Input::getLine("Please type in the Asset, which you want to download.");
					std::string param3 = Input::getLine("Please type in the Path. Like '/test/example.3dsx'.");
					scpt << Function << param1 << "	" << param2 << "	" << param3 << " " << std::endl;
					break;


				} case 1: {
					std::string Function = "downloadFile	";
					std::string param1 = Input::getLine("Please type in the download URL.");
					std::string param2 = Input::getLine("Please type in the path. Like '/test/example.3dsx'.");
					scpt << Function << param1 << "	" << param2 << " " << std::endl;
					break;


				} case 2: {
					std::string Function = "extract	";
					std::string param1 = Input::getLine("Please type in the archive path. Like 'test/example.zip'.");
					std::string param2 = Input::getLine("Please type in the stuff, which you want to extract.");
					std::string param3 = Input::getLine("Please type the path, where the files should be extracted.");
					scpt << Function << param1 << "	" << param2 << "	" << param3 << " " << std::endl;
					break;


				} case 3: {
					std::string Function = "install	";
					std::string param1 = Input::getLine("Please type in the path to the cia. like: '/test.cia'.");
					scpt << Function << param1 << " " << std::endl;
					break;


				} case 4: {
					std::string Function = "delete	";
					std::string param1 = Input::getLine("Please type in the path to the delete file. 'sdmc:/test.cia'.");
					scpt << Function << param1 << " " << std::endl;
					break;


				} case 5: {
					std::string Function = "msg	";
					std::string param1 = Input::getLine("Please type in your Message.");
					scpt << Function << param1 << " " << std::endl;
					break;
				}
			}
		} else if (ScriptPage == 2) {
			switch(Selection) {
				case 0: {
					std::string Function = "mkdir	";
					std::string param1 = Input::getLine("Please type in the new folder path. like 'sdmc:/Test'.");
					scpt << Function << param1 << " " << std::endl;
					break;


				} case 1: {
					std::string Function = "progressDownloadFile	";
					std::string param1 = Input::getLine("Please type in the download URL.");
					std::string param2 = Input::getLine("Please type in the path. Like '/test/example.3dsx'.");
					std::string param3 = Input::getLine("Please type in your Displayed Message.");
					scpt << Function << param1 << "	" << param2 << "	" << param3 << " " << std::endl;
					break;


				} case 2: {
					std::string Function = "progressDownloadRelease	";
					std::string param1 = Input::getLine("Please type in the Github Repo URL.");
					std::string param2 = Input::getLine("Please type in the Asset, which you want to download.");
					std::string param3 = Input::getLine("Please type in the Path. Like '/test/example.3dsx'.");
					std::string param4 = Input::getLine("Please type in your Displayed Message.");
					scpt << Function << param1 << "	" << param2 << "	" << param3 << "	" << param4 << " " << std::endl;
					break;


				} case 3: {
					std::string Function = "progressExtract	";
					std::string param1 = Input::getLine("Please type in the archive path. Like 'test/example.zip'.");
					std::string param2 = Input::getLine("Please type in the stuff, which you want to extract.");
					std::string param3 = Input::getLine("Please type the path, where the files should be extracted.");
					std::string param4 = Input::getLine("Please type in your Displayed Message.");
					scpt << Function << param1 << "	" << param2 << "	" << param3 << "	" << param4 << " " << std::endl;
					break;


				} case 4: {
					std::string Function = "waitMsg	";
					std::string param1 = Input::getLine("Please type in your Displayed Message.");
					scpt << Function << param1 << " " << std::endl;
					break;


				} case 5: {
					std::string Function = "timeMsg	";
					std::string param1 = Input::getLine("Please type in your Displayed Message.");
					std::string param2 = Input::Numpad("Please type in the amount of seconds.");
					scpt << Function << param1 << "	" << param2 << " " << std::endl;
					break;
				}
			}
		}
	}
}

void Script::ScriptCreatorLogic(u32 hDown, u32 hHeld, touchPosition touch) {
	ScriptCreatorSelectionLogic(hDown, hHeld);
	if (hDown & KEY_START) {
		if(confirmPopup("Do you want to save this Script?")) {
			scpt.close();
			Selection = 0;
			ScriptPage = 1;
			ScriptMode = 0;
		}
	} else if (hDown & KEY_B) {
		if(confirmPopup("Do you want to exit without saving?")) {
			scpt.close();
			Selection = 0;
			ScriptPage = 1;
			ScriptMode = 0;
		}
	} else if (ScriptPage == 2 && hDown & KEY_L) {
		Selection = 0;
		ScriptPage = 1;
	} else if (ScriptPage == 1 && hDown & KEY_R) {
		Selection = 0;
		ScriptPage = 2;
	} else if (ScriptPage == 1 && hDown & KEY_TOUCH) {
	if (touching(touch, Functions[0])) {

		std::string Function = "downloadRelease	";
		std::string param1 = Input::getLine("Please type in the Github Repo URL.");
		std::string param2 = Input::getLine("Please type in the Asset, which you want to download.");
		std::string param3 = Input::getLine("Please type in the Path. Like '/test/example.3dsx'.");
		scpt << Function << param1 << "	" << param2 << "	" << param3 << " " << std::endl;


	} else if (touching(touch, Functions[1])) {

		std::string Function = "downloadFile	";
		std::string param1 = Input::getLine("Please type in the download URL.");
		std::string param2 = Input::getLine("Please type in the path. Like '/test/example.3dsx'.");
		scpt << Function << param1 << "	" << param2 << " " << std::endl;


	} else if (touching(touch, Functions[2])) {

		std::string Function = "extract	";
		std::string param1 = Input::getLine("Please type in the archive path. Like 'test/example.zip'.");
		std::string param2 = Input::getLine("Please type in the stuff, which you want to extract.");
		std::string param3 = Input::getLine("Please type the path, where the files should be extracted.");
		scpt << Function << param1 << "	" << param2 << "	" << param3 << " " << std::endl;


	} else if (touching(touch, Functions[3])) {
		std::string Function = "install	";
		std::string param1 = Input::getLine("Please type in the path to the cia. like: '/test.cia'.");
		scpt << Function << param1 << " " << std::endl;


	} else if (touching(touch, Functions[4])) {
		std::string Function = "delete	";
		std::string param1 = Input::getLine("Please type in the path to the delete file. 'sdmc:/test.cia'.");
		scpt << Function << param1 << " " << std::endl;


	} else if (touching(touch, Functions[5])) {
		std::string Function = "msg	";
		std::string param1 = Input::getLine("Please type in your Message.");
		scpt << Function << param1 << " " << std::endl;
	}

	// Page 2.
	} else if (ScriptPage == 2 && hDown & KEY_TOUCH) {
		if (touching(touch, Functions[0])) {
			std::string Function = "mkdir	";
			std::string param1 = Input::getLine("Please type in the new folder path. like 'sdmc:/Test'.");
			scpt << Function << param1 << " " << std::endl;


		} else if (touching(touch, Functions[1])) {
			std::string Function = "progressDownloadFile	";
			std::string param1 = Input::getLine("Please type in the download URL.");
			std::string param2 = Input::getLine("Please type in the path. Like '/test/example.3dsx'.");
			std::string param3 = Input::getLine("Please type in your Displayed Message.");
			scpt << Function << param1 << "	" << param2 << "	" << param3 << " " << std::endl;


		} else if (touching(touch, Functions[2])) {
			std::string Function = "progressDownloadRelease	";
			std::string param1 = Input::getLine("Please type in the Github Repo URL.");
			std::string param2 = Input::getLine("Please type in the Asset, which you want to download.");
			std::string param3 = Input::getLine("Please type in the Path. Like '/test/example.3dsx'.");
			std::string param4 = Input::getLine("Please type in your Displayed Message.");
			scpt << Function << param1 << "	" << param2 << "	" << param3 << "	" << param4 << " " << std::endl;


		} else if(touching(touch, Functions[3])) {
			std::string Function = "progressExtract	";
			std::string param1 = Input::getLine("Please type in the archive path. Like 'test/example.zip'.");
			std::string param2 = Input::getLine("Please type in the stuff, which you want to extract.");
			std::string param3 = Input::getLine("Please type the path, where the files should be extracted.");
			std::string param4 = Input::getLine("Please type in your Displayed Message.");
			scpt << Function << param1 << "	" << param2 << "	" << param3 << "	" << param4 << " " << std::endl;


		} else if (touching(touch, Functions[4])) {
			std::string Function = "waitMsg	";
			std::string param1 = Input::getLine("Please type in your Displayed Message.");
			scpt << Function << param1 << " " << std::endl;


		} else if (touching(touch, Functions[5])) {
			std::string Function = "timeMsg	";
			std::string param1 = Input::getLine("Please type in your Displayed Message.");
			std::string param2 = Input::Numpad("Please type in the amount of seconds.");
			scpt << Function << param1 << "	" << param2 << " " << std::endl;
		}
	}
}