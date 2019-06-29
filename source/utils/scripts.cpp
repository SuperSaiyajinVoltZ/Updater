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

#include "download.hpp"
#include "extract.hpp"
#include "gui.hpp"
#include "screenCommon.hpp"
#include <fstream>

extern "C" {
#include "cia.h"
}

struct Scpt {
	std::string function;
	std::string param1;
	std::string param2;
	std::string param3;
};

Scpt getScptFromLine(std::string line) {
	Scpt scpt;
	scpt.function = line.substr(0, line.find("	"));
	line = line.substr(line.find("	")+1);
	
	scpt.param1 = line.substr(0, std::min(line.find("	"), line.length()-1));
	line = line.substr(line.find("	")+1);

	scpt.param2 = line.substr(0, std::min(line.find("	"), line.length()-1));
	line = line.substr(line.find("	")+1);

	scpt.param3 = line.substr(0, std::min(line.find("	"), line.length()-1));
	line = line.substr(line.find("	")+1);
	return scpt;
}

void runScript(std::string path) {
	std::ifstream in(path);
	if(in.good()) {
		std::string line;
		while(std::getline(in, line)) {
			Scpt scpt = getScptFromLine(line);

			if(scpt.function == "downloadRelease") {
				downloadFromRelease(scpt.param1, scpt.param2, scpt.param3);
			}

			if(scpt.function == "downloadFile") {
				downloadToFile(scpt.param1, scpt.param2);
			}

			if(scpt.function == "extract") {
				extractArchive(scpt.param1, scpt.param2, scpt.param3);
			}

			if(scpt.function == "install") {
				installCia(scpt.param1.c_str());
			}

			if(scpt.function == "delete") {
				deleteFile(scpt.param1.c_str());
			}
			
			if(scpt.function == "msg") {
				DisplayMsg(scpt.param1.c_str());
			}

			if(scpt.function == "screenTop") {
				set_screen(top);
			}

			if(scpt.function == "screenBot") {
				set_screen(bottom);
			}
		}
	}
}