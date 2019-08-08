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
#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "screens/screen.hpp"
#include "structs.hpp"
#include <vector>

class MainMenu : public SCREEN 
{
public:
	MainMenu();
	void Draw(void) const override;
	void Logic(u32 hDown, u32 hHeld, touchPosition touch) override;

private:

	mutable char universal_manager_vertext[13];
	int currentPage = 0;
	int Selection1 = 0;
	int Selection2 = 0;

	// Functions.
	void drawSelection1(void) const;
	void drawSelection2(void) const;
	void SelectionLogic1(u32 hDown);
	void SelectionLogic2(u32 hDown);

	// Structs.
	std::vector<Structs::ButtonPos> mainScreenButtonPos = {
    	{0, 25, 149, 52, -1},
    	{170, 25, 149, 52, -1},
		{0, 90, 149, 52, -1},
		{170, 90, 149, 52, -1},
		{0, 150, 149, 52, -1},
    	{170, 150, 149, 52, -1},

    	{0, 25, 149, 52, -1},
		{170, 25, 149, 52, -1},
		{0, 90, 149, 52, -1},
		{170, 90, 149, 52, -1},
		{0, 150, 149, 52, -1},
	};
};

#endif