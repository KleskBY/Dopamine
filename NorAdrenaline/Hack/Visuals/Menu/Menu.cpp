#include "../../../Required.h"
#include "../../../images.h"

CMenu g_Menu;

#define MAX_TABS 5
#define TAB_WEIGHT 30
#define TAB_HEIGHT 30
#define BORDER_SIZE 4


typedef std::basic_string<TCHAR> tstring;

tstring VirtualKeyCodeToString(UCHAR virtualKey)
{
	UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

	TCHAR szName[128];
	int result = 0;
	switch (virtualKey)
	{
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
	case VK_PRIOR: case VK_NEXT:
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE:
	case VK_NUMLOCK:
		scanCode |= KF_EXTENDED;
	default:
		result = GetKeyNameText(scanCode << 16, szName, 128);
	}
	if (result == 0)
		throw std::system_error(std::error_code(GetLastError(), std::system_category()),
			"WinAPI Error occured.");
	return szName;
}

void CMenu::Init()
{
	if (bInitialised)
		return;

	//if (!background.index) background.index = g_pISurface->CreateNewTextureID(true);
	/*
	if (!icon_headshot.index) icon_headshot.index = g_pISurface->CreateNewTextureID(true);

	if (!icon_aimbot.index) icon_aimbot.index = g_pISurface->CreateNewTextureID(true);

//	if (!icon_systems.index) icon_systems.index = g_pISurface->CreateNewTextureID(true);

	if (!icon_options.index) icon_options.index = g_pISurface->CreateNewTextureID(true);

	if (!icon_visuals.index) icon_visuals.index = g_pISurface->CreateNewTextureID(true);

	if (!icon_members.index) icon_members.index = g_pISurface->CreateNewTextureID(true);

	if (!icon_headshot_not_selected.index) icon_headshot_not_selected.index = g_pISurface->CreateNewTextureID(true);

	if (!icon_aimbot_not_selected.index) icon_aimbot_not_selected.index = g_pISurface->CreateNewTextureID(true);

	//if (!icon_systems_not_selected.index) icon_systems_not_selected.index = g_pISurface->CreateNewTextureID(true);

	if (!icon_options_not_selected.index) icon_options_not_selected.index = g_pISurface->CreateNewTextureID(true);

	if (!icon_visuals_not_selected.index) icon_visuals_not_selected.index = g_pISurface->CreateNewTextureID(true);

	if (!icon_members_not_selected.index) icon_members_not_selected.index = g_pISurface->CreateNewTextureID(true);
	*/

	background.width = 492;//ШИРИНА МЕНЮ
	background.height = 500;//ВЫСОТА МЕНЮ

	/*icon_headshot.width = icon_headshot.width = 45;
	icon_aimbot.width = icon_aimbot.height = 45;
	//icon_systems.width = icon_systems.height = 45;
	icon_options.width = icon_options.height = 45;
	icon_visuals.width = icon_visuals.height = 45;
	icon_members.width = icon_members.height = 45;

	icon_headshot_not_selected.width = icon_headshot_not_selected.width = 45;
	icon_aimbot_not_selected.width = icon_aimbot_not_selected.height = 45;
	//icon_systems_not_selected.width = icon_systems_not_selected.height = 45;
	icon_options_not_selected.width = icon_options_not_selected.height = 45;
	icon_visuals_not_selected.width = icon_visuals_not_selected.height = 45;
	icon_members_not_selected.width = icon_members_not_selected.height = 45;

	g_pISurface->DrawSetTextureRGBA(background.index, gui_background, background.width, background.height, false, true);

	g_pISurface->DrawSetTextureRGBA(icon_headshot.index, gui_headshot, icon_headshot.width, icon_headshot.width, false, true);
	g_pISurface->DrawSetTextureRGBA(icon_aimbot.index, gui_aim, icon_aimbot.width, icon_aimbot.height, false, true);
	g_pISurface->DrawSetTextureRGBA(icon_options.index, gui_options, icon_options.width, icon_options.height, false, true);
	g_pISurface->DrawSetTextureRGBA(icon_members.index, gui_members, icon_members.width, icon_members.height, false, true);
	g_pISurface->DrawSetTextureRGBA(icon_visuals.index, gui_visuals, icon_visuals.width, icon_visuals.height, false, true);
//	g_pISurface->DrawSetTextureRGBA(icon_systems.index, gui_system, icon_systems.width, icon_systems.height, false, true);

	g_pISurface->DrawSetTextureRGBA(icon_headshot_not_selected.index, gui_headshot_off, icon_headshot.width, icon_headshot.width, false, true);
	g_pISurface->DrawSetTextureRGBA(icon_aimbot_not_selected.index, gui_aim_off, icon_aimbot.width, icon_aimbot.height, false, true);
	g_pISurface->DrawSetTextureRGBA(icon_options_not_selected.index, gui_options_off, icon_options.width, icon_options.height, false, true);
	g_pISurface->DrawSetTextureRGBA(icon_members_not_selected.index, gui_members_off, icon_members.width, icon_members.height, false, true);
	g_pISurface->DrawSetTextureRGBA(icon_visuals_not_selected.index, gui_visuals_off, icon_visuals.width, icon_visuals.height, false, true);
	//g_pISurface->DrawSetTextureRGBA(icon_systems_not_selected.index, gui_system_off, icon_systems.width, icon_systems.height, false, true);
	*/
	bInitialised = true;
	bOpened = false;
	dwCheckboxBlockedTime = 0;
	dwSliderBlockedTime = 0;
	dwPaletteBlockedTime = 0;
	dwListBlockedTime = 0;

	//default menu position
	MenuX = 60;
	MenuY = 20;

	iCurrentTab = 2;//default tab
	cvar.menu_color_r = 255;
	cvar.menu_color_g = 0;
	cvar.menu_color_b = 0;

}

void CMenu::Run()
{
	Init();

	if (g_pGlobals.bSnapshot || g_pGlobals.bScreenshot)
		return;

	if (cvar.hide_from_obs && g_Local.status.connected && g_Local.iTeam == 0)
		return;

	static bool m_bMouseCapture = false;

	static bool m_bCursorState = false;

	static DWORD dwTemporaryBlockTimer = 0;

	int key = VK_INSERT;
	if (cvar.menu_key > 0 && cvar.menu_key < 255)
		key = cvar.menu_key;

	if (GetTickCount() - dwTemporaryBlockTimer > 200 && keys[key])
	{
		bOpened = !bOpened;
		dwTemporaryBlockTimer = GetTickCount();
	}

	if (!bOpened && m_bMouseCapture && g_pIRunGameEngine->IsInGame() && !g_pGameUI->IsGameUIActive())
	{
		g_pISurface->SetCursor(dc_none);
		g_pISurface->LockCursor();
		m_bMouseCapture = false;

		if (m_bCursorState)
		{
			g_pISurface->SurfaceSetCursorPos(g_Screen.iWidth / 2, g_Screen.iHeight / 2);
			m_bCursorState = false;
		}
	}

	if (!bOpened) //||
		//!g_pISurface->IsTextureIDValid(background.index)) ||
		//!g_pISurface->IsTextureIDValid(icon_headshot.index) || !g_pISurface->IsTextureIDValid(icon_aimbot.index) || !g_pISurface->IsTextureIDValid(icon_options.index) || !g_pISurface->IsTextureIDValid(icon_visuals.index)  || !g_pISurface->IsTextureIDValid(icon_members.index) ||
		//!g_pISurface->IsTextureIDValid(icon_headshot_not_selected.index) || !g_pISurface->IsTextureIDValid(icon_aimbot_not_selected.index) || !g_pISurface->IsTextureIDValid(icon_options_not_selected.index) || !g_pISurface->IsTextureIDValid(icon_visuals_not_selected.index) || !g_pISurface->IsTextureIDValid(icon_members_not_selected.index))
		return;

	if (g_pIRunGameEngine->IsInGame() && !g_pGameUI->IsGameUIActive())
	{
		g_pISurface->SetCursor(dc_arrow);
		g_pISurface->UnlockCursor();
		m_bMouseCapture = true;

		if (!m_bCursorState)
		{
			g_pISurface->SurfaceSetCursorPos(CursorX, CursorY);
			m_bCursorState = true;
		}
	}

	g_Engine.GetMousePosition(&CursorX, &CursorY);

	//g_Drawing.DrawTexture(background.index, MenuX, MenuY, background.width + MenuX, background.height + MenuY);//Draw background
	g_pISurface->DrawSetColor(255, 255, 255, 255);
	//	 float fas[2];
	//	fas[1] = MenuX;


		/*g_Drawing.DrawRect(MenuX + 1, MenuY + 490, 1, 1, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 2, MenuY + 491, 1, 1, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 3, MenuY + 492, 1, 1, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 4, MenuY + 493, 1, 1, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 5, MenuY + 494, 1, 1, 213, 209, 202, 150);
		//g_Drawing.DrawRect(MenuX + 6, MenuY + 495, 1, 1, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 6, MenuY + 490, 500, 5, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 1, MenuY + 491, 1, 1, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 2, MenuY + 490, 1, 2, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 3, MenuY + 490, 1, 3, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 4, MenuY + 490, 1, 4, 213, 209, 202, 150);
		g_Drawing.DrawRect(MenuX + 5, MenuY + 490, 1, 5, 213, 209, 202, 150);*/


		//g_Drawing.DrawRect(MenuX + 6, MenuY + 490, 1, 6, 213, 209, 202, 150);
	g_pISurface->DrawFilledRect(MenuX, MenuY, background.width + MenuX, background.height + MenuY);

	if (DrawButton(MenuX + 2, MenuY + 2, "Legitbot"))
	{
		iCurrentTab = 2;
	}
	if (DrawButton(MenuX + 166, MenuY + 2, "Visuals"))
	{
		iCurrentTab = 3;
	}
	if (DrawButton(MenuX + 330, MenuY + 2, "Misc"))
	{
		iCurrentTab = 4;
	}
	if (DrawButton(MenuX + 2, MenuY + 490, "Ragebot"))
	{
		iCurrentTab = 1;
	}
	if (DrawButton(MenuX + 330, MenuY + 490, "Load"))
	{
		func.LoadCvars();
	}
	if (DrawButton(MenuX + 166, MenuY + 490, "Save"))
	{
		func.SaveCvars();
	}

	//Reset all
	bCursorInPalette = false;
	bCursorInList = false;
	iPaletteIndex = 0;
	iListIndex = 0;

	//SelectTab();
   // DrawMenuTabs();
	Tabs();
	Drag();
}

void CMenu::Tabs()
{
	static int m_iOldTab = iCurrentTab;

	if (m_iOldTab != iCurrentTab)
	{
		for (unsigned int i = 0; i < LIMIT_LISTBOX; i++)
			bListBoxOpened[i] = false;

		for (unsigned int i = 0; i < LIMIT_PALETTE; i++)
			bPaletteOpened[i] = false;

		m_iOldTab = iCurrentTab;
	}

	static unsigned int indent_x = 10;
	static unsigned int indent_y = 14;

	if (iCurrentTab == 1)//Rage aimbot
	{
		unsigned int x = MenuX + 10;
		unsigned int y = MenuY + 30;

		unsigned int line_y = indent_y;

		unsigned int save[LIMIT_SAVE];

		{//BOX1
			DrawBox(x, y, 230, 230);//Aimbot

			g_pISurface->DrawSetColor(255, 255, 255, 255);//(239, 239, 238, 150);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 60, y + 2);
			g_Drawing.DrawString(MENU, x + 14, y, 255, 0, 0, 255, FONT_LEFT, "Aimbot");

			Checkbox(x + indent_x, y + line_y, cvar.aim, "Enabled");
			line_y += 30;

			static char *szTargetSelection[] = { "Field of view", "Distance", "Cycle" };
			save[7] = y + line_y;
			line_y += 40;

			static char *szTargetHitbox[] = { "Head", "Neck", "Chest", "Stomach", "All hitboxes", "Vital hitboxes" };
			save[8] = y + line_y;
			line_y += 40;

			static char *szMultipoint[] = { "Low", "Medium", "High" };
			save[9] = y + line_y;
			line_y += 30;

			Checkbox(x + indent_x, y + line_y, cvar.aim_teammates, "Teammates");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.aim_penetration, "Automatic penetration");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.aim_silent, "Silent aim");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.aim_perfect_silent, "Perfect silent aim");
			line_y += 20;

			ListBox(iListIndex++, x + indent_x, save[9], "Multi-point", cvar.aim_multi_point, szMultipoint, 3, true);
			ListBox(iListIndex++, x + indent_x, save[8], "Target hitbox", cvar.aim_hitbox, szTargetHitbox, 6, false);
			ListBox(iListIndex++, x + indent_x, save[7], "Target selection", cvar.aim_target_selection, szTargetSelection, 3, false);
		}

		{//BOX1.2
			y = y + 250;
			line_y = 15;

			DrawBox(x, y, 230, 180);//Fakelag

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 60, y + 2);
			g_Drawing.DrawString(MENU, x + 16, y, 255, 0, 0, 255, FONT_LEFT, "Fake lag");

			Checkbox(x + indent_x, y + line_y, cvar.fakelag, "Enabled");
			line_y += 20;

			char *tmp4[] = { "On land", "On move", "In air", "All" };
			save[4] = y + line_y;
			line_y += 30;

			char *tmp3[] = { "Dynamic", "Maximum", "Flucture", "Break lag compensation" };
			save[3] = y + line_y;
			line_y += 40;

			Slider(x + indent_x, y + line_y, 0, 100, cvar.fakelag_variance, "Variance");
			line_y += 30;

			Slider(x + indent_x, y + line_y, 0, 30, cvar.fakelag_limit, "Limit");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.fakelag_while_shooting, "Fake lag while shooting");
			line_y += 20;

			ListBox(iListIndex++, x + indent_x, save[3], "", cvar.fakelag_move, tmp4, 4, false);
			ListBox(iListIndex++, x + indent_x, save[4], "", cvar.fakelag_type, tmp3, 4, false);
		}

		{//BOX2
			y = MenuY + 30;
			x = x + 240;
			line_y = 15;

			DrawBox(x, y, 230, 200); //Other

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 54, y + 2);
			g_Drawing.DrawString(MENU, x + 16, y, 255, 0, 0, 255, FONT_LEFT, "Other");

			Checkbox(x + indent_x, y + line_y, cvar.norecoil, "Remove recoil");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.nospread, "Remove spread");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.quick_stop, "Quick stop");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.quick_stop_duck, "Quick stop + duck");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.aim_autoscope, "Automatic scope");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.aim_delay_shot, "Anti-aim resolver");
			line_y += 20;

			Checkbox(x + indent_x, y + line_y, cvar.aim_delay_shot, "Delay shot");
			line_y += 30;

			char *tmp[] = { "Pitch / Yaw", "Roll" };
			ListBox(iListIndex++, x + indent_x, y + line_y, "Remove spread method", cvar.nospread_method, tmp, 2, false);
		}

		{//BOX3
			y = y + 220;
			line_y = 30;

			DrawBox(x, y, 230, 210);//Anti-Aimbot

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 86, y + 2);
			g_Drawing.DrawString(MENU, x + 14, y, 255, 0, 0, 255, FONT_LEFT, "Anti-Aimbot");

			char *tmp1[] = { "Fakedown", "Down", "Up", "Jitter", "Random" };
			save[0] = y + line_y;
			//ListBox(iListIndex++, x + box_indent_x, y + line_y, "Pitch", cvar.esp_weapon, tmp1, 6, false);
			line_y += 40;

			char *tmp2[] = { "180", "180 jitter", "Spin", "Jitter", "Sideways", "Random", "Static" };
			save[1] = y + line_y;
			//ListBox(iListIndex++, x + box_indent_x, y + line_y, "Yaw", cvar.esp_weapon, tmp2, 7, false);
			line_y += 40;

			char *tmp3[] = { "180", "180 jitter", "Spin", "Jitter", "Sideways", "Random", "Static" };
			save[2] = y + line_y;
			//ListBox(iListIndex++, x + box_indent_x, y + line_y, "Yaw while running", cvar.esp_weapon, tmp3, 7, false);
			line_y += 40;

			char *tmp4[] = { "Standing", "In move", "All", "All + Fake" };
			save[5] = y + line_y;
			line_y += 40;

			Slider(x + indent_x, y + line_y, -180, 180, cvar.aa_yaw_static, "Static yaw");
			line_y += 20;

			ListBox(iListIndex++, x + indent_x, save[5], "Edge", cvar.aa_edge, tmp4, 4, true);

			ListBox(iListIndex++, x + indent_x, save[2], "Yaw while running", cvar.aa_yaw_while_running, tmp3, 7, true);

			ListBox(iListIndex++, x + indent_x, save[1], "Yaw", cvar.aa_yaw, tmp2, 7, true);

			ListBox(iListIndex++, x + indent_x, save[0], "Pitch", cvar.aa_pitch, tmp1, 5, true);
		}
	}
	else if (iCurrentTab == 2)//Legit
	{
		int x = MenuX + 10;
		int y = MenuY + 30;
		int box_indent_x = 10;
		int box_indent_y = 15;

		int save[256];

		int line_y = 15;

		int weapon_id = 0;

		if (cvar.menu_legit_global_section == 1)
		{
			if (cvar.menu_legit_sub_section == 1)
				weapon_id = WEAPON_GLOCK18;
			else if (cvar.menu_legit_sub_section == 2)
				weapon_id = WEAPON_P228;
			else if (cvar.menu_legit_sub_section == 3)
				weapon_id = WEAPON_DEAGLE;
			else if (cvar.menu_legit_sub_section == 4)
				weapon_id = WEAPON_ELITE;
			else if (cvar.menu_legit_sub_section == 5)
				weapon_id = WEAPON_FIVESEVEN;
			else if (cvar.menu_legit_sub_section == 6)
				weapon_id = WEAPON_USP;
		}
		else if (cvar.menu_legit_global_section == 2)
		{
			if (cvar.menu_legit_sub_section == 1)
				weapon_id = WEAPON_AWP;
			else if (cvar.menu_legit_sub_section == 2)
				weapon_id = WEAPON_SCOUT;
			else if (cvar.menu_legit_sub_section == 3)
				weapon_id = WEAPON_G3SG1;
			else if (cvar.menu_legit_sub_section == 4)
				weapon_id = WEAPON_SG550;
		}
		else if (cvar.menu_legit_global_section == 3)
		{
			if (cvar.menu_legit_sub_section == 1)
				weapon_id = WEAPON_M4A1;
			else if (cvar.menu_legit_sub_section == 2)
				weapon_id = WEAPON_GALIL;
			else if (cvar.menu_legit_sub_section == 3)
				weapon_id = WEAPON_FAMAS;
			else if (cvar.menu_legit_sub_section == 4)
				weapon_id = WEAPON_AUG;
			else if (cvar.menu_legit_sub_section == 5)
				weapon_id = WEAPON_AK47;
			else if (cvar.menu_legit_sub_section == 6)
				weapon_id = WEAPON_SG552;
		}
		else if (cvar.menu_legit_global_section == 4)
		{
			if (cvar.menu_legit_sub_section == 1)
				weapon_id = WEAPON_XM1014;
			else if (cvar.menu_legit_sub_section == 2)
				weapon_id = WEAPON_M3;
		}
		else if (cvar.menu_legit_global_section == 5)
		{
			if (cvar.menu_legit_sub_section == 1)
				weapon_id = WEAPON_M249;
		}
		else if (cvar.menu_legit_global_section == 6)
		{
			if (cvar.menu_legit_sub_section == 1)
				weapon_id = WEAPON_TMP;
			else if (cvar.menu_legit_sub_section == 2)
				weapon_id = WEAPON_P90;
			else if (cvar.menu_legit_sub_section == 3)
				weapon_id = WEAPON_MP5N;
			else if (cvar.menu_legit_sub_section == 4)
				weapon_id = WEAPON_MAC10;
			else if (cvar.menu_legit_sub_section == 5)
				weapon_id = WEAPON_UMP45;
		}

		if (weapon_id > 0)
		{
			y = y + 45 + 20;

			{//BOX2
				DrawBox(x, y, 225, 385);//Aimbot

				g_pISurface->DrawSetColor(255, 255, 255, 255);
				g_pISurface->DrawFilledRect(x + 14, y - 1, x + 60, y + 2);
				g_Drawing.DrawString(MENU, x + 15, y, 255, 0, 0, 255, FONT_LEFT, "Aimbot");

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].aim, "Enabled");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 100, cvar.legit[weapon_id].aim_speed, "Smooth in Auto-aim");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 100, cvar.legit[weapon_id].aim_speed_in_attack, "Smooth in Attack");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 100, cvar.legit[weapon_id].aim_speed_scale_fov, "Speed scale - FOV", true, "%");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 1000, cvar.legit[weapon_id].aim_reaction_time, "Delay", true, "ms");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 180, cvar.legit[weapon_id].aim_fov, "Maximum FOV", false, "°", true);
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 1, cvar.legit[weapon_id].aim_psilent, "Perfect silent", false, "°", true);
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 100, cvar.legit[weapon_id].aim_recoil_compensation_pitch, "Recoil compensation (P/Y)", true, "%");
				line_y += 20;

				Slider(x + box_indent_x, y + line_y, 0, 100, cvar.legit[weapon_id].aim_recoil_compensation_yaw, "", true, "%");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].aim_humanize, "Humanize");
				line_y += 19;

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].aim_quick_stop, "Quick stop");
				line_y += 19;

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].aim_head, "Head");
				line_y += 19;

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].aim_chest, "Chest");
				line_y += 19;

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].aim_stomach, "Stomach");
				line_y += 19;
			}

			x = x + 250;
			line_y = 15;

			{//BOX3

				DrawBox(x, y, 220, 175);//Triggerbot

				g_pISurface->DrawSetColor(255, 255, 255, 255);
				g_pISurface->DrawFilledRect(x + 14, y - 1, x + 80, y + 2);
				g_Drawing.DrawString(MENU, x + 14, y, 255, 0, 0, 255, FONT_LEFT, "Triggerbot");

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].trigger, "Enabled");
				KeyBind(x + box_indent_x + 120, y + line_y - 10, cvar.trigger_key);
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].trigger_penetration, "Automatic penetration");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].trigger_head, "Head");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].trigger_chest, "Chest");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.legit[weapon_id].trigger_stomach, "Stomach");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.trigger_only_zoomed, "Only zoomed");
				line_y += 30;

				char *tmp[] = { "Recoil", "Recoil / Spread" };
				save[0] = y + line_y;
				ListBox(iListIndex++, x + box_indent_x, save[0], "Accuracy boost", cvar.legit[weapon_id].trigger_accuracy, tmp, 2, true);
			}

			y = y + 175 + 20;
			line_y = 15;

			{//BOX4

				DrawBox(x, y, 220, 170);//Other

				g_pISurface->DrawSetColor(255, 255, 255, 255);
				g_pISurface->DrawFilledRect(x + 14, y - 1, x + 54, y + 2);
				g_Drawing.DrawString(MENU, x + 16, y, 255, 0, 0, 255, FONT_LEFT, "Other");

				Checkbox(x + box_indent_x, y + line_y, cvar.legit_teammates, "Teammates");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 1000, cvar.block_attack_after_kill, "Block attack after kill", true, "ms");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 10, cvar.legit[weapon_id].aim_recoil_compensation_after_shots_fired, "RCS after shots fired", true);
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.fastzoom, "Automatic scope");
				line_y += 30;

				char *tmp[] = { "Aiming", "Recoil", "Recoil / Spread" };
				save[0] = y + line_y;
				ListBox(iListIndex++, x + box_indent_x, y + line_y, "Accuracy boost", cvar.legit[weapon_id].aim_accuracy, tmp, 3, true);
				line_y += 30;
			}
		}

		x = MenuX + 10;
		y = MenuY + 30;
		line_y = 15;

		{//BOX1
			DrawBox(x, y, 470, 45);//Section

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 60, y + 2);
			g_Drawing.DrawString(MENU, x + 14, y, 255, 0, 0, 255, FONT_LEFT, "Section");

			char *tmp[] = { "Pistol", "Sniper", "Rifle", "Shotgun", "Machine gun", "Submachine gun" };
			ListBox(iListIndex++, x + box_indent_x, y + line_y, "", cvar.menu_legit_global_section, tmp, 6, false);

			if (cvar.menu_legit_global_section == 1) {
				char *tmp2[] = { "GLOCK18", "P228", "DEAGLE", "ELITE", "FIVESEVEN", "USP" };
				ListBox(iListIndex++, x + box_indent_x + 240, y + line_y, "", cvar.menu_legit_sub_section, tmp2, 6, false);

				if (cvar.menu_legit_sub_section > 6)
					cvar.menu_legit_sub_section = 6;
			}
			else if (cvar.menu_legit_global_section == 2) {
				char *tmp2[] = { "AWP", "SCOUT", "G3SG1", "SG550", };
				ListBox(iListIndex++, x + box_indent_x + 240, y + line_y, "", cvar.menu_legit_sub_section, tmp2, 4, false);

				if (cvar.menu_legit_sub_section > 4)
					cvar.menu_legit_sub_section = 4;
			}
			else if (cvar.menu_legit_global_section == 3) {
				char *tmp2[] = { "M4A1", "GALIL", "FAMAS", "AUG", "AK47", "SG552" };
				ListBox(iListIndex++, x + box_indent_x + 240, y + line_y, "", cvar.menu_legit_sub_section, tmp2, 6, false);

				if (cvar.menu_legit_sub_section > 6)
					cvar.menu_legit_sub_section = 6;
			}
			else if (cvar.menu_legit_global_section == 4) {
				char *tmp2[] = { "XM1014", "M3" };
				ListBox(iListIndex++, x + box_indent_x + 240, y + line_y, "", cvar.menu_legit_sub_section, tmp2, 2, false);

				if (cvar.menu_legit_sub_section > 2)
					cvar.menu_legit_sub_section = 2;
			}
			else if (cvar.menu_legit_global_section == 5) {
				char *tmp2[] = { "M249" };
				ListBox(iListIndex++, x + box_indent_x + 240, y + line_y, "", cvar.menu_legit_sub_section, tmp2, 1, false);

				if (cvar.menu_legit_sub_section > 1)
					cvar.menu_legit_sub_section = 1;
			}
			else if (cvar.menu_legit_global_section == 6) {
				char *tmp2[] = { "TMP", "P90", "MP5N", "MAC10", "UMP45" };
				ListBox(iListIndex++, x + box_indent_x + 240, y + line_y, "", cvar.menu_legit_sub_section, tmp2, 5, false);

				if (cvar.menu_legit_sub_section > 5)
					cvar.menu_legit_sub_section = 5;
			}
		}
	}

	else if (iCurrentTab == 3)//Visuals
	{
		int x = MenuX + 10;
		int y = MenuY + 30;
		int box_indent_x = 10;
		int box_indent_y = 15;

		int save[256];

		int line_y = 15;

		{
			DrawBox(x, y, 235, 215);

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 50, y + 2);
			g_Drawing.DrawString(MENU, x + 16, y, 255, 0, 0, 255, FONT_LEFT, "Boxes");

			Checkbox(x + box_indent_x, y + line_y, cvar.esp, "Enabled");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_teammates, "Teammates");
			line_y += 30;

			static char *typebox[] = { "Box", "Coal", "Corner" };
			ListBox(iListIndex++, x + box_indent_x, y + line_y, "Type", cvar.esp_box, typebox, 3, true);
			line_y += 30;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_box_outline, "Outline");
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 0, 255, cvar.esp_boxfill, "Fill", true);
			line_y += 20;

			save[0] = y + line_y; // visible
			g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y, 0, 0, 0, 255, FONT_LEFT, "Color visible");
			line_y += 15;

			save[5] = y + line_y; // invisible
			g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y, 0, 0, 0, 255, FONT_LEFT, "Color invisible");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_behind, "Behind wall");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_drawhistory, "Draw history");
			line_y += 20;

			/*

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_flags, "Flags");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_line_of_sight, "Line of sight");
			save[1] = y + line_y;
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_shots_fired, "Shots fired");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_hitboxes, "Hitboxes");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_screen, "Screen");
			line_y += 20;

			char *wweapon[] = { "Text", "Icon" };
			ListBox(iListIndex++, x + box_indent_x, save[6], "Weapon", cvar.esp_weapon, wweapon, 2, true);*/
		}

		{
			y = y + 230;
			line_y = 15;

			DrawBox(x, y, 235, 140);

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 50, y + 2);
			g_Drawing.DrawString(MENU, x + 16, y, 255, 0, 0, 255, FONT_LEFT, "Player");

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_name, "Name");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_weapon, "Weapon");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_sound, "Sound box");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_distance, "Distance");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_health, "Health bar");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_bomb, "Bomb");
			save[8] = y + line_y;
			line_y += 20;
		}

		{
			y = MenuY + 30;
			x = x + 250;
			line_y = 15;

			DrawBox(x, y, 215, 130);

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 55, y + 2);
			g_Drawing.DrawString(MENU, x + 17, y, 255, 0, 0, 255, FONT_LEFT, "Chams");

			Checkbox(x + box_indent_x, y + line_y, cvar.chams, "Active");
			save[3] = y + line_y;
			line_y += 30;

			char *tmp[] = { "Flat", "Texture", "Material" };
			ListBox(iListIndex++, x + box_indent_x, y + line_y, "Type", cvar.chams_type, tmp, 3);
			line_y += 30;

			Checkbox(x + box_indent_x, y + line_y, cvar.chams_behind_wall, "Behind wall");
			line_y += 25;

			g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y, 0, 0, 0, 255, FONT_LEFT, "Color visible");
			Palette(x + 195, y + line_y, cvar.chams_ct_vis_r, cvar.chams_ct_vis_g, cvar.chams_ct_vis_b);
			Palette(x + 175, y + line_y, cvar.chams_t_vis_r, cvar.chams_t_vis_g, cvar.chams_t_vis_b);
			line_y += 15;

			g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y, 0, 0, 0, 255, FONT_LEFT, "Color invisible");
			Palette(x + 195, y + line_y, cvar.chams_ct_invis_r, cvar.chams_ct_invis_g, cvar.chams_ct_invis_b);
			Palette(x + 175, y + line_y, cvar.chams_t_invis_r, cvar.chams_t_invis_g, cvar.chams_t_invis_b);
			line_y += 20;

		//	Checkbox(x + box_indent_x, y + line_y, cvar.grenade_trajectory, "Grenade trajectory");
		//	line_y += 20;


			//Checkbox(x + box_indent_x, y + line_y, cvar.recoil_overlay, "Recoil overlay");
			//Palette(x + 195, y + line_y, cvar.recoil_overlay_r, cvar.recoil_overlay_g, cvar.recoil_overlay_b);
			//line_y += 20;

			//Checkbox(x + box_indent_x, y + line_y, cvar.penetration_info, "Penetration info");
			//line_y += 20;


		}

		{
			y = y + 145;
			line_y = 15;

			DrawBox(x, y, 215, 220);

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 90, y + 2);
			g_Drawing.DrawString(MENU, x + 17, y, 255, 0, 0, 255, FONT_LEFT, "Effects & Info");

			line_y += 10;
			Slider(x + box_indent_x, y + line_y, 0, 255, cvar.noflash, "Disable render flash", true);
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 0, 150, cvar.custom_fov, "FOV Changer", true);
			line_y += 30;

			Palette(x + 195, y + line_y, cvar.brightness_r, cvar.brightness_g, cvar.brightness_b);
			Slider(x + box_indent_x, y + line_y, 0, 255, cvar.brightness, "Brightness", true);
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.nosmoke, "Disable render smoke");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.remove_scope, "Disable /Add scope");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.disable_render_teammates, "Disable render teammaters");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.spread_overlay, "Spread FOV");
			Palette(x + 195, y + line_y, cvar.draw_aim_fov_r, cvar.draw_aim_fov_g, cvar.draw_aim_fov_b);
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.draw_aim_fov, "Aimbot FOV");
			Palette(x + 195, y + line_y, cvar.draw_aim_fov_r, cvar.draw_aim_fov_g, cvar.draw_aim_fov_b);
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.esp_world_weapon, "Dropped weapon");
	        line_y += 20;

		}

		{
			y = y + 235;
			line_y = 15;

			DrawBox(x, y, 215, 40);

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 45, y + 2);
			g_Drawing.DrawString(MENU, x + 16, y, 255, 0, 0, 255, FONT_LEFT, "Glow");

			Checkbox(x + box_indent_x, y + line_y, cvar.glow_players, "Enabled");
			Palette(x + 175, y + line_y, cvar.glow_players_t_r, cvar.glow_players_t_g, cvar.glow_players_t_b);
			Palette(x + 195, y + line_y, cvar.glow_players_ct_r, cvar.glow_players_ct_g, cvar.glow_players_ct_b);
			line_y += 20;
		}


		{//draw no bug
			x = MenuX + 100;
			y = MenuY + 60;

			Palette(x + 120, save[5], cvar.esp_box_ct_invis_r, cvar.esp_box_ct_invis_g, cvar.esp_box_ct_invis_b);
			Palette(x + 100, save[5], cvar.esp_box_t_invis_r, cvar.esp_box_t_invis_g, cvar.esp_box_t_invis_b);

			Palette(x + 120, save[0], cvar.esp_box_ct_vis_r, cvar.esp_box_ct_vis_g, cvar.esp_box_ct_vis_b);
			Palette(x + 100, save[0], cvar.esp_box_t_vis_r, cvar.esp_box_t_vis_g, cvar.esp_box_t_vis_b);

			Palette(x + 120, save[2], cvar.glow_players_ct_r, cvar.glow_players_ct_g, cvar.glow_players_ct_b);
			Palette(x + 100, save[2], cvar.glow_players_t_r, cvar.glow_players_t_g, cvar.glow_players_t_b);


		/*	Palette(x + 120, save[1], cvar.esp_line_of_sight_r, cvar.esp_line_of_sight_g, cvar.esp_line_of_sight_b);*/


			Palette(x + 120, save[8], cvar.esp_bomb_r, cvar.esp_bomb_g, cvar.esp_bomb_b);
		}
	}

	else if (iCurrentTab == 4)
	{
		int x = MenuX + 10;
		int y = MenuY + 30;
		int box_indent_x = 10;
		int box_indent_y = 15;

		int save[256];

		int line_y = 15;

		{

			DrawBox(x, y, 215, 135);

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 98, y + 2);
			g_Drawing.DrawString(MENU, x + 16, y, 255, 0, 0, 255, FONT_LEFT, "Miscellaneous");


			Checkbox(x + box_indent_x, y + line_y, cvar.use_all_undetected, "Use all undetected");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.hud_clear, "HUD clear");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.autopistol, "AutoPistol");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.autoreload, "AutoReolad");
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 0, 10000, cvar.name_stealer, "Name stealer", true, "ms");
			line_y += 20;

		/*	if (DrawButton(x + box_indent_x + 200, y + line_y, "Disable lag compensation (server)"))
			{
				g_Engine.PlayerInfo_SetValueForKey("cl_lc", "0");
			}*/


			//line_y += 20;
			//g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y, 180, 180, 180, 255, FONT_LEFT, "Menu key:");
		//	KeyBind(x + box_indent_x + 120, y + line_y - 10, cvar.menu_key);
		}

		{
			y = y + 160;
			line_y = 15;

			DrawBox(x, y, 215, 140);

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 55, y + 2);
			g_Drawing.DrawString(MENU, x + 16, y, 255, 0, 0, 255, FONT_LEFT, "Kreedz");

			Checkbox(x + box_indent_x, y + line_y, cvar.knifebot, "Knifebot");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.bunnyhop, "Bunnyhop");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.ddrun, "GroundStrafe");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.autostrafe, "AutoStrafe");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.jumpbug, "Jumpbug");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.fastrun, "Fastrun");
			line_y += 20;
		}

		{
			y = MenuY + 30;
			x = x + 230;
			line_y = 15;

			DrawBox(x, y, 225, 190);

			g_pISurface->DrawSetColor(255, 255, 255, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 60, y + 2);
			g_Drawing.DrawString(MENU, x + 16, y, 255, 0, 0, 255, FONT_LEFT, "Crosshair");

	
			Checkbox(x + box_indent_x, y + line_y, cvar.crosshair_Active, "Active");
			Palette(x + 195, y + line_y, cvar.crosshair_color_r, cvar.crosshair_color_g, cvar.crosshair_color_b);
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 0, 50, cvar.crosshair_Size, "Size", true);
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 0, 50, cvar.crosshair_Gap, "Gap", true);
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 0, 50, cvar.crosshair_Dynamic, "Dynamic", true);
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 0, 10, cvar.crosshair_DynamicScale, "DynamicScale", false, "", true);
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.crosshair_Outline, "Outline");
			line_y += 20;

			Checkbox(x + box_indent_x, y + line_y, cvar.crosshair_Dot, "Dot");
			line_y += 20;
		}


		/*	{//BOX3
				y = y + 180;
				x = x + 200;
				line_y = 15;

				DrawBox(x, y, 250, 250);//Other

				g_pISurface->DrawSetColor(1, 8, 8, 255);
				g_pISurface->DrawFilledRect(x + 14, y - 1, x + 54, y + 2);
				g_Drawing.DrawString(MENU, x + 20, y, 220, 220, 220, 255, FONT_LEFT, "Other");

				Checkbox(x + box_indent_x, y + line_y, cvar.debug, "Debug");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.hide_from_obs, "Hide from OBS");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.bypass_valid_blockers, "Bypass valid blockers");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.bypass_trace_blockers, "Bypass trace blockers");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.esp_fake, "Bypass esp blockers");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 1, cvar.esp_sound_minimum_volume, "Minimum volume sound");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, cvar.spread_overlay_old, "Spread overlay old");
				line_y += 20;

				if (DrawButton(x + box_indent_x, y + line_y, "Patch interpolation cvars"))
				{
					g_Offsets.PatchInterpolation();
				}

				line_y += 30;


				if (DrawButton(x + box_indent_x, y + line_y, "Disable lag compensation (server)"))
				{
					g_Engine.PlayerInfo_SetValueForKey("cl_lc", "0");
				}

				line_y += 20;
			}*/
	}
	else if (iCurrentTab == 5)
	{
		int x = MenuX + 10;
		int y = MenuY + 30;
		int box_indent_x = 10;
		int box_indent_y = 15;

		int save[256];

		int line_y = 15;

		{//BOX1
			DrawBox(x, y, 520, 430);//Player list

			g_pISurface->DrawSetColor(1, 8, 8, 255);
			g_pISurface->DrawFilledRect(x + 14, y - 1, x + 76, y + 2);
			g_Drawing.DrawString(MENU, x + 20, y, 220, 220, 220, 255, FONT_LEFT, "Player list");

			size_t max = g_Engine.GetMaxClients();

			if (max > 16)
				max = 16;

			for (int j = 1; j <= max; j++)
			{
				if (j == g_Local.iIndex)
					continue;

				if (g_PlayerInfoList[j].name)
				{
					if (g_Player[j].iTeam == CT)
						g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y + 4, 0, 0, 180, 255, FONT_LEFT, "%i) %s", j, g_PlayerInfoList[j].name);
					else if (g_Player[j].iTeam == TERRORIST)
						g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y + 4, 180, 0, 0, 255, FONT_LEFT, "%i) %s", j, g_PlayerInfoList[j].name);
					else
						g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y + 4, 180, 180, 180, 255, FONT_LEFT, "%i) %s", j, g_PlayerInfoList[j].name);
				}
				else
					g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y + 4, 180, 180, 180, 255, FONT_LEFT, "%i) ", j);

				if (g_PlayerInfoList[j].ping > 0)
				{
					Checkbox(x + box_indent_x + 160, y + line_y, g_Player[j].bFriend, "F");

					Checkbox(x + box_indent_x + 200, y + line_y, g_Player[j].bPriority, "P");
				}

				line_y += 20;
			}

			x = x + 270;
			line_y = 15;

			for (int j = max + 1; j <= g_Engine.GetMaxClients(); j++)
			{
				if (j == g_Local.iIndex)
					continue;

				if (g_PlayerInfoList[j].name)
				{
					if (g_Player[j].iTeam == CT)
						g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y + 4, 0, 0, 180, 255, FONT_LEFT, "%i) %s", j, g_PlayerInfoList[j].name);
					else if (g_Player[j].iTeam == TERRORIST)
						g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y + 4, 180, 0, 0, 255, FONT_LEFT, "%i) %s", j, g_PlayerInfoList[j].name);
					else
						g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y + 4, 180, 180, 180, 255, FONT_LEFT, "%i) %s", j, g_PlayerInfoList[j].name);
				}
				else
					g_Drawing.DrawString(MENU, x + box_indent_x, y + line_y + 4, 180, 180, 180, 255, FONT_LEFT, "%i) ", j);

				if (g_PlayerInfoList[j].ping > 0)
				{
					Checkbox(x + box_indent_x + 160, y + line_y, g_Player[j].bFriend, "F");

					Checkbox(x + box_indent_x + 200, y + line_y, g_Player[j].bPriority, "P");
				}

				line_y += 20;
			}
		}
	}
}

bool CMenu::DrawButton(int x, int y, char *text)
{
	unsigned int w = 160;
	unsigned int h = 18;

	g_pISurface->DrawSetColor(255, 0, 0, 255); // shadow
	g_pISurface->DrawOutlinedRect(x - 1, y - 1, x + w + 1, y + h + 1);

	g_pISurface->DrawSetColor(5, 5, 20, 255);
	g_pISurface->DrawOutlinedRect(x - 2, y - 2, x + w + 2, y + h + 2);

	bool clicked = false;

	static DWORD dwTemporaryBlockTimer = 0;

	if (GetTickCount() - dwPaletteBlockedTime > 200 && GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && !bCursorInList && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (GetTickCount() - dwTemporaryBlockTimer > 200)
		{
			clicked = true;
			dwTemporaryBlockTimer = GetTickCount();
		}
	}

	if (clicked || CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		g_pISurface->DrawSetColor(255, 0, 0, 255); // цвет когда навели
		g_pISurface->DrawFilledRect(x, y, x + w, y + h);
	}
	else
	{
		g_pISurface->DrawSetColor(5, 5, 20, 255); // цвет
		g_pISurface->DrawFilledRect(x, y, x + w, y + h);
	}

	if (text)
		g_Drawing.DrawString(MENU, x + w / 2, y + (h / 2), 255, 255, 255, 255, FONT_CENTER, text);

	return clicked;
}

void CMenu::Palette(int x, int y, float &r, float &g, float &b)
{
	unsigned int w = 16;
	unsigned int h = 8;

	static DWORD dwTemporaryBlockTimer = 0;
	//Close others and open this
	if (GetTickCount() - dwListBlockedTime > 200 && GetTickCount() - dwPaletteBlockedTime > 200 && !bCursorInList && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (GetTickCount() - dwTemporaryBlockTimer > 200)
		{
			bPaletteOpened[iPaletteIndex] = !bPaletteOpened[iPaletteIndex];

			dwTemporaryBlockTimer = GetTickCount();

			for (unsigned int i = 0; i < LIMIT_PALETTE; i++)
			{
				if (i == iPaletteIndex) continue;

				if (bPaletteOpened[i])
					bPaletteOpened[i] = false;
			}
		}
	}

	g_pISurface->DrawSetColor(r, g, b, 255);
	g_pISurface->DrawFilledRect(x, y, x + w, y + h);

	//	g_pISurface->DrawSetColor(255, 0, 0, 255);
	//	g_pISurface->DrawOutlinedRect(x - 1, y - 1, x + w + 1, y + h + 1);

		//shadow
	g_pISurface->DrawSetColor(0, 0, 0, 255);
	g_pISurface->DrawOutlinedRect(x - 1, y - 1, x + w + 1, y + h + 1);

	static unsigned int panel_w = 230;
	static unsigned int panel_h = 54;
	static unsigned int indent = 8;
	static unsigned int border = 4;

	if (bPaletteOpened[iPaletteIndex])
	{
		g_pISurface->DrawSetColor(5, 5, 20, 255); //(120,168,93,255);
		g_pISurface->DrawFilledRect(x + w + indent - border, y - indent, x + w + panel_w + indent + border, y + panel_h + indent);

		g_pISurface->DrawSetColor(255, 255, 255, 255);
		g_pISurface->DrawOutlinedRect(x + w + indent - border - 1, y - indent - 1, x + w + panel_w + indent + border + 1, y + panel_h + indent + 1);

		//shadow
		//g_pISurface->DrawSetColor(0, 0, 0, 255);
		//g_pISurface->DrawOutlinedRect(x + w + indent - border - 2, y - indent - 2, x + w + panel_w + indent + border + 2, y + panel_h + indent + 2);

		SliderInPallete(x + w + indent, y, 0, 255, r, "", true);
		SliderInPallete(x + w + indent, y + 20, 0, 255, g, "", true);
		SliderInPallete(x + w + indent, y + 40, 0, 255, b, "", true);
	}

	if (bPaletteOpened[iPaletteIndex] && !IsDragging && CursorX >= x + w + indent - border && CursorX <= x + w + panel_w + indent + border && CursorY >= y - indent && CursorY <= y + panel_h + indent)
	{
		bCursorInPalette = true;
		dwCheckboxBlockedTime = GetTickCount();
		dwPaletteBlockedTime = GetTickCount();
		dwSliderBlockedTime = GetTickCount();
	}

	iPaletteIndex++;
}

void CMenu::ListBox(int index, int x, int y, char *name, float &value, char **text, int size, bool default)
{
	unsigned int w = 200;
	unsigned int h = 18;

	if (name)
		g_Drawing.DrawString(MENU, x + 1, y - 10, 0, 0, 0, 255, FONT_LEFT, name);

	static DWORD dwTemporaryBlockTimer = 0;
	//Close others and open this
	if (!bListBoxOpened[index] && GetTickCount() - dwPaletteBlockedTime > 200 && GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (GetTickCount() - dwTemporaryBlockTimer > 200)
		{
			bListBoxOpened[index] = !bListBoxOpened[index];

			dwTemporaryBlockTimer = GetTickCount();

			for (unsigned int i = 0; i < LIMIT_LISTBOX; i++)
			{
				if (i == index) continue;

				if (bListBoxOpened[i])
				{
					bListBoxOpened[i] = false;
				}
			}
		}
	}

	bool effect_cursor = false;
	int effect_cursor_x0, effect_cursor_y0, effect_cursor_x1, effect_cursor_y1;
	int tmp = h;

	if (!bListBoxOpened[index])
	{
		g_pISurface->DrawSetColor(0, 0, 20, 255);//(120,168,93,255);
		g_pISurface->DrawFilledRect(x, y, x + w, y + tmp);

		g_pISurface->DrawSetColor(255, 0, 0, 255);
		g_pISurface->DrawOutlinedRect(x - 1, y - 1, x + w + 1, y + tmp + 1);

		//shadow
		g_pISurface->DrawSetColor(0, 0, 0, 255);
		g_pISurface->DrawOutlinedRect(x - 2, y - 2, x + w + 2, y + tmp + 2);

		bool m_bDrawDefault = true;

		for (unsigned int i = 0; i < size; i++)
		{
			if (text[i] && value == i + 1)
			{
				m_bDrawDefault = false;
				g_Drawing.DrawString(MENU, x + 4, y + (h / 2), 255, 255, 255, 255, FONT_LEFT, text[i]);
			}
		}

		if (m_bDrawDefault && default)
			g_Drawing.DrawString(MENU, x + 4, y + (h / 2), 255, 255, 255, 255, FONT_LEFT, "None");
	}
	else
	{
		for (unsigned int i = 0; i < size; i++)
		{
			if (text[i])
			{
				if (CursorX >= x && CursorX <= x + w && CursorY >= y + tmp - h && CursorY <= y + tmp)
				{
					effect_cursor_x0 = x;
					effect_cursor_x1 = x + w;
					effect_cursor_y0 = y + tmp - h;
					effect_cursor_y1 = y + tmp;
					effect_cursor = true;
				}

				if (GetTickCount() - dwTemporaryBlockTimer > 200 && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y + tmp - h && CursorY <= y + tmp)
				{
					value = i + 1;
					bListBoxOpened[index] = false;
					dwTemporaryBlockTimer = GetTickCount();
				}

				tmp += h;
			}
		}

		if (default)
		{
			if (GetTickCount() - dwTemporaryBlockTimer > 200 && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + tmp)
			{
				value = 0;
				bListBoxOpened[index] = false;
				dwTemporaryBlockTimer = GetTickCount();
			}
		}
		else
			tmp -= h;

		g_pISurface->DrawSetColor(0, 0, 20, 255);//(120,168,93,255);
		g_pISurface->DrawFilledRect(x, y, x + w, y + tmp);

		g_pISurface->DrawSetColor(0, 0, 0, 255);
		g_pISurface->DrawOutlinedRect(x - 1, y - 1, x + w + 1, y + tmp + 1);

		//shadow
		g_pISurface->DrawSetColor(0, 0, 0, 255);
		g_pISurface->DrawOutlinedRect(x - 2, y - 2, x + w + 2, y + tmp + 2);
	}

	if (effect_cursor)
	{
		g_pISurface->DrawSetColor(255, 0, 0, 255);
		g_pISurface->DrawFilledRect(effect_cursor_x0, effect_cursor_y0, effect_cursor_x1, effect_cursor_y1);
	}

	if (bListBoxOpened[index])
	{
		tmp = h;

		for (unsigned int i = 0; i < size; i++)
		{
			if (text[i])
			{
				g_Drawing.DrawString(MENU, x + 4, y + tmp - (h / 2), 255, 255, 255, 255, FONT_LEFT, text[i]);
				tmp += h;
			}
		}

		if (default)
			g_Drawing.DrawString(MENU, x + 4, y + tmp - (h / 2), 220, 220, 220, 255, FONT_LEFT, "None");
		else
			tmp -= h;
	}

	if (bListBoxOpened[index] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + tmp)
	{
		bCursorInList = true;
		dwCheckboxBlockedTime = GetTickCount();
		dwListBlockedTime = GetTickCount();
		dwSliderBlockedTime = GetTickCount();
	}
}

void CMenu::SliderInPallete(int x, int y, float min, float max, float &value, char *text, bool ThisINT, char *amout, bool extra)
{
	unsigned int w = 230;
	unsigned int h = 12;

	g_pISurface->DrawSetColor(5, 5, 20, 255);//r(120,168,93,255);
	g_pISurface->DrawFilledRect(x, y, x + w, y + h);

	g_pISurface->DrawSetColor(255, 0, 0, 255);
	g_pISurface->DrawOutlinedRect(x - 1, y - 1, x + w + 1, y + h + 1);

	//shadow
	g_pISurface->DrawSetColor(5, 5, 20, 255);
	g_pISurface->DrawOutlinedRect(x - 2, y - 2, x + w + 2, y + h + 2);

	if (text)
		g_Drawing.DrawString(MENU, x + 1, y - 10, 255, 255, 255, 255, FONT_LEFT, text);

	if (ThisINT)
		value = (int)value;

	if (value < min)
		value = min;
	else if (value > max)
		value = max;

	float one = (w / max);

	int fill = one * value;

	if (value < 0)
		fill *= -1;

	g_pISurface->DrawSetColor(255, 0, 0, 255);
	g_pISurface->DrawFilledRect(x, y, x + fill, y + h);

	if (!amout)
		amout = "";

	if (!ThisINT)
	{
		if (!extra)
			g_Drawing.DrawString(MENU, x + (w / 2), y + (h / 2), 255, 255, 255, 255, FONT_CENTER, "%.2f%s", value, amout);
		else
			g_Drawing.DrawStringACP(MENU, x + (w / 2), y + (h / 2), 255, 255, 255, 255, FONT_CENTER, "%.2f%s", value, amout);
	}
	else
	{
		if (!extra)
			g_Drawing.DrawString(MENU, x + (w / 2), y + (h / 2), 255, 255, 255, 255, FONT_CENTER, "%.f%s", value, amout);
		else
			g_Drawing.DrawString(MENU, x + (w / 2), y + (h / 2), 255, 255, 255, 255, FONT_CENTER, "%.f%s", value, amout);
	}

	if (GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && !bCursorInList && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (keys[VK_LBUTTON]) {
			value = (CursorX - x) / one;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;
		}
		else if (keys[VK_RBUTTON] && min < 0) {
			value = (CursorX - x) / one;
			value *= -1;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;
		}
	}
}

void CMenu::KeyBind(int x, int y, int &key)
{
	unsigned int w = 60;
	unsigned int h = 18;

	bool clicked = false;

	static DWORD dwTemporaryBlockTimer = 0;

	if (GetTickCount() - dwPaletteBlockedTime > 200 && GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && !bCursorInList && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (GetTickCount() - dwTemporaryBlockTimer > 200)
		{
			clicked = true;
			dwTemporaryBlockTimer = GetTickCount();
		}
	}

	if (key == -2 && GetTickCount() - dwTemporaryBlockTimer > 200)
	{
		for (unsigned int i = 0; i < 255; i++)
		{
			if (g_Menu.keys[i])
			{
				if (i == VK_ESCAPE || i == VK_LBUTTON || i == VK_RBUTTON || i == cvar.menu_key) // i == VK_LBUTTON || i == VK_RBUTTON || 
				{
					key = -1;
					break;
				}
				key = i;
			}
		}
	}

	if (clicked)
	{
		if (key == -1) {
			key = -2;
		}
		else {
			key = -1;
		}
	}

	if (key == -1)
	{
		g_Drawing.DrawString(MENU, x + w / 2, y + (h / 2), 255, 0, 0, 255, FONT_CENTER, "[No key]");
	}
	else if (key == -2)
	{
		g_Drawing.DrawString(MENU, x + w / 2, y + (h / 2), 255, 0, 0, 255, FONT_CENTER, "[Press key]");
	}
	else {
		g_Drawing.DrawString(MENU, x + w / 2, y + (h / 2), 255, 0, 0, 255, FONT_CENTER, "[%s]", VirtualKeyCodeToString(key).c_str());
	}
}

void CMenu::Slider(int x, int y, float min, float max, float &value, char *text, bool ThisINT, char *amout, bool extra)
{
	unsigned int w = 200;
	unsigned int h = 12;

	g_pISurface->DrawSetColor(0, 0, 0, 255);//(120,168,93,255);
	g_pISurface->DrawFilledRect(x, y, x + w, y + h);

	g_pISurface->DrawSetColor(255, 0, 0, 255);
	g_pISurface->DrawOutlinedRect(x - 1, y - 1, x + w + 1, y + h + 1);

	g_pISurface->DrawSetColor(0, 0, 0, 255);
	g_pISurface->DrawOutlinedRect(x - 2, y - 2, x + w + 2, y + h + 2);

	if (text)
		g_Drawing.DrawString(MENU, x + 1, y - 10, 0, 0, 0, 255, FONT_LEFT, text);

	if (ThisINT)
		value = (int)value;

	if (value < min)
		value = min;
	else if (value > max)
		value = max;

	float one = (w / max);

	int fill = one * value;

	if (value < 0)
		fill *= -1;

	g_pISurface->DrawSetColor(255, 0, 0, 255);
	g_pISurface->DrawFilledRect(x, y, x + fill, y + h);

	if (!amout)
		amout = "";

	if (!ThisINT)
	{
		if (!extra)
			g_Drawing.DrawString(MENU, x + (w / 2), y + (h / 2), 255, 255, 255, 255, FONT_CENTER, "%.2f%s", value, amout);
		else
			g_Drawing.DrawStringACP(MENU, x + (w / 2), y + (h / 2), 255, 255, 255, 255, FONT_CENTER, "%.2f%s", value, amout);
	}
	else
	{
		if (!extra)
			g_Drawing.DrawString(MENU, x + (w / 2), y + (h / 2), 255, 255, 255, 255, FONT_CENTER, "%.f%s", value, amout);
		else
			g_Drawing.DrawString(MENU, x + (w / 2), y + (h / 2), 255, 255, 255, 255, FONT_CENTER, "%.f%s", value, amout);
	}

	if (GetTickCount() - dwSliderBlockedTime > 200 && GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && !bCursorInList && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (keys[VK_LBUTTON]) {
			value = (CursorX - x) / one;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;
		}
		else if (keys[VK_RBUTTON] && min < 0) {
			value = (CursorX - x) / one;
			value *= -1;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;
		}
	}
}

/*void CMenu::DrawMenuTabs()
{
	int tab_x = BORDER_SIZE + 1;
	int tab_y = BORDER_SIZE + 1 + 10;
	int tab_h = ((background.height - (BORDER_SIZE * 2)) / MAX_TABS) - 40;
	int tab_w = tab_x + TAB_WEIGHT;

	int y = BORDER_SIZE + 1 + 15;
	int x = BORDER_SIZE + 1 + 12;

	for (unsigned int i = 1; i <= MAX_TABS; i++)
	{

			g_pISurface->DrawSetColor(120,168,93,255);
			g_pISurface->DrawFilledRect(MenuX + tab_x, MenuY + tab_y, MenuX + tab_w, MenuY + tab_y + tab_h);

			g_pISurface->DrawSetColor(91, 99, 100, 255);
			g_pISurface->DrawFilledRect(MenuX + tab_x, MenuY + tab_y - 1, MenuX + tab_w, MenuY + tab_y);
			g_pISurface->DrawSetColor(0, 0, 0, 255);
			g_pISurface->DrawFilledRect(MenuX + tab_x, MenuY + tab_y - 1, MenuX + tab_w - 1, MenuY + tab_y - 2);

			g_pISurface->DrawSetColor(91, 99, 100, 255);
			g_pISurface->DrawFilledRect(MenuX + tab_x, MenuY + tab_y + tab_h - 1, MenuX + tab_w, MenuY + tab_y + tab_h);
			g_pISurface->DrawSetColor(0, 0, 0, 255);
			g_pISurface->DrawFilledRect(MenuX + tab_x, MenuY + tab_y + tab_h + 1, MenuX + tab_w - 1, MenuY + tab_y + tab_h + 2);


		y = y + tab_h + 20;
		tab_y = tab_y + tab_h + 20;
	}
}*/

void CMenu::Drag()
{
	if (bCursorInList || bCursorInPalette)
	{
		IsDragging = false;
		return;
	}

	static int drag_x = 0;
	static int drag_y = 0;

	if (IsDragging && !keys[VK_LBUTTON])
	{
		IsDragging = false;
	}
	else if (IsDragging && keys[VK_LBUTTON])
	{
		MenuX = CursorX - drag_x;
		MenuY = CursorY - drag_y;
	}

	//head move
	if (keys[VK_LBUTTON] && (
		(CursorX >= MenuX && CursorX <= background.width + MenuX && CursorY >= MenuY && CursorY <= MenuY + BORDER_SIZE) ||
		(CursorX >= MenuX && CursorX <= MenuX + BORDER_SIZE && CursorY >= MenuY && CursorY <= background.height + MenuY) ||
		(CursorX >= MenuX + background.width - BORDER_SIZE && CursorX <= MenuX + background.width && CursorY >= MenuY && CursorY <= background.height + MenuY) ||
		(CursorX >= MenuX && CursorX <= background.width + MenuX && CursorY >= MenuY + background.height - BORDER_SIZE && CursorY <= MenuY + background.height)))
	{
		drag_x = CursorX - MenuX;
		drag_y = CursorY - MenuY;
		IsDragging = true;
	}
}


void CMenu::Checkbox(int x, int y, bool &value, char *text)
{
	static unsigned int w = 10;
	static unsigned int h = 10;

	g_pISurface->DrawSetColor(0, 0, 0, 255);
	g_pISurface->DrawOutlinedRect(x, y, x + w, y + h);

	g_pISurface->DrawSetColor(0, 0, 0, 255);
	g_pISurface->DrawOutlinedRect(x - 1, y - 1, x + w + 1, y + h + 1);

	if (value)
	{
		g_pISurface->DrawSetColor(255, 0, 0, 255);
		g_pISurface->DrawFilledRect(x + 1, y + 1, x + w - 1, y + h - 1);
	}

	if (text)
		g_Drawing.DrawString(MENU, x + 16, y + 4, 5, 5, 20, 255, FONT_LEFT, text);

	if (GetTickCount() - dwCheckboxBlockedTime > 200 && !bCursorInPalette && !bCursorInList && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		value = !value;
		dwCheckboxBlockedTime = GetTickCount();
	}
}

void CMenu::DrawBox(int x, int y, int w, int h)
{
	//box
	g_pISurface->DrawSetColor(255, 255, 255, 255);
	g_pISurface->DrawFilledRect(x, y, x + w, y + h);
	//outline
	g_pISurface->DrawSetColor(0, 0, 0, 255);
	g_pISurface->DrawOutlinedRect(x - 1, y - 1, x + w + 1, y + h + 1);
	//shadow
	//g_pISurface->DrawSetColor(0, 0, 0, 255);
	//g_pISurface->DrawOutlinedRect(x - 2, y - 2, x + w + 2, y + h + 2);
}