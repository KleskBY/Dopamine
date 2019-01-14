#include "../../Required.h"
#include "../../keydefs.h"
#include "../../raw_inputs.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <process.h>
#include <Psapi.h>
#define WINVER 0x0500
CSystems g_Systems;
CONST int scrolldown = -120;
CONST int scrollup = 120;
bool asdfg_scrolled;
int goodjump_scrolled;
int frames_without_scroll;
bool scrolling;
bool scroll_forced;
int loop;
bool bJumpScroll;
float time;
int intermission;
using namespace std;
bool bFastRun;





float speedcap = g_Local.flMaxSpeed*1.2f + 50.f;
int pattern;
static int counter;
int poo;
static int frames_to_waste_on_ground;
bool prevent_next_slowdown;
bool fail_bhop = false;
bool perfect_bhop = false;
int bDuckPressed;
bool bDuckScroll;
bool bDuckJustPressed;
int down;
int keynum;
const char *pszCurrentBinding;


float GetGroundOrigin()
{
	vec3_t vOrigin = pmove->origin;
	vec3_t vTracePost = vOrigin;
	vTracePost[2] = -9999.f;
	pmtrace_s tr;

	g_Engine.pEventAPI->EV_SetTraceHull((pmove->flags&FL_DUCKING) ? 1 : 0);
	g_Engine.pEventAPI->EV_PlayerTrace(vOrigin, vTracePost, PM_NORMAL, -1, &tr);
	vec3_t vReturn = vOrigin + (vTracePost - vOrigin) * tr.fraction;

	return vReturn[2];
}


float lastScroll_Time[2];
int scroll_Counter[2];

DWORD dwDuckPressed;
void hookedDuckPressed()
{
	bDuckJustPressed = true;
	bDuckPressed++;

	_asm CALL dwDuckPressed // call original command
}

DWORD dwDuckReleased;
void hookedDuckReleased()
{
	if (bDuckJustPressed)
		bDuckScroll = true;

	if (bDuckPressed)
		bDuckPressed--;

	_asm CALL dwDuckReleased
}

//bool bInGame;

int bJumpPressed;
bool bJumpJustPressed;
DWORD dwJumpPressed;

void hookedJumpPressed()
{
	bJumpJustPressed = true;
	bJumpPressed++;
	_asm CALL dwJumpPressed
}

DWORD dwJumpReleased;
void hookedJumpReleased()
{
	if (bJumpJustPressed)
		bJumpScroll = true;

	if (bJumpPressed)
		bJumpPressed--;

	_asm CALL dwJumpReleased
}
	



bool isGoodJump()
{
	if (pmove->flags & FL_ONGROUND || g_Local.vVelocity[2] >= 0.0 || goodjump_scrolled > 0)
		return false;

	vec3_t          start, end;
	pmtrace_t		tr;
	start[0] = g_Local.vOrigin[0] + g_Local.vVelocity[0] * g_Local.flFrameTime;
	start[1] = g_Local.vOrigin[1] + g_Local.vVelocity[1] * g_Local.flFrameTime;
	start[2] = g_Local.vOrigin[2] + g_Local.flFrameTime*(g_Local.vVelocity[2] - 800.0f*g_Local.flFrameTime*0.5f);

	end[0] = start[0];
	end[1] = start[1];
	end[2] = start[2] - 9999.f;

	float predictedZorigin = start[2] + g_Local.flFrameTime*((g_Local.vVelocity[2] + 1600.0f*g_Local.flFrameTime) - 800.0f*g_Local.flFrameTime*0.5f);

	g_Engine.pEventAPI->EV_SetTraceHull(g_Local.usehull);
	g_Engine.pEventAPI->EV_PlayerTrace(start, end, PM_NORMAL, -1, &tr);
	float height = predictedZorigin - tr.endpos[2];
	if (height <= 2.56f && height >= 0.0 && tr.plane.normal[2] >= 0.7)
	{
		//	g_Engine.pfnClientCmd( "+camin" );
			//g_Engine.Con_Printf( "\t\tGOODJUMP HEIGHT: %.3f Predicted: %.3f\n", g_Local.vOrigin[2] - tr.endpos[2], height );
		return true;
	}
	return false;
}


bool isBadJump()
{
	if (pmove->flags & FL_ONGROUND || g_Local.vVelocity[2] >= 0.0 || isGoodJump())
		return false;

	vec3_t          point;
	pmtrace_t		tr;
	point[0] = g_Local.vOrigin[0] + g_Local.vVelocity[0] * g_Local.flFrameTime;
	point[1] = g_Local.vOrigin[1] + g_Local.vVelocity[1] * g_Local.flFrameTime;
	point[2] = g_Local.vOrigin[2] - 9999.0f;
	float predictedZorigin = g_Local.vOrigin[2] + g_Local.flFrameTime*(g_Local.vVelocity[2] - 800.0f*g_Local.flFrameTime*0.5f);
	g_Engine.pEventAPI->EV_SetTraceHull(g_Local.usehull);
	g_Engine.pEventAPI->EV_PlayerTrace(g_Local.vOrigin, point, PM_NORMAL, -1, &tr);
	float height = predictedZorigin - tr.endpos[2];
	if (height <= 2.56f && height >= 0.0 && tr.plane.normal[2] >= 0.7)
	{
		//g_Engine.pfnClientCmd( "-camin" );
		//g_Engine.Con_Printf( "\t\tBADJUMP HEIGHT: %.3f Predicted: %.3f\n", g_Local.vOrigin[2] - tr.endpos[2], height );
		return true;
	}
	return false;
}

















bool asdfg(int num)
{
	if (pmove->flags & FL_ONGROUND || g_Local.vVelocity[2] >= 0 || asdfg_scrolled)// || goodjump_scrolled!=0 )
		return false;

	vec3_t Origin = g_Local.vOrigin;
	vec3_t Velocity = g_Local.vVelocity;

	while (num > 0)
	{
		Velocity[2] = Velocity[2] - 800.0f*g_Local.flFrameTime;
		Origin[2] = Origin[2] + g_Local.flFrameTime*(Velocity[2] - 800.0f*g_Local.flFrameTime*0.5f);
		//Velocity[2] = Velocity[2] - 800.0f*g_Local.flFrameTime;
		//Origin[2] = Origin[2] + g_Local.flFrameTime*(Velocity[2]-800.0f*g_Local.flFrameTime*0.5f);

		num--;
	}
	vec3_t start, end;
	pmtrace_t tr;
	start = Origin;
	end[0] = start[0];
	end[1] = start[1];
	end[2] = start[2] - 9999.f;

	g_Engine.pEventAPI->EV_SetTraceHull(g_Local.usehull);
	g_Engine.pEventAPI->EV_PlayerTrace(start, end, PM_NORMAL, -1, &tr);

	float height = Origin[2] - tr.endpos[2];
	//g_Engine.Con_Printf( "\t\t asdfg(4) %.3f\n", height, num );
	if (height <= 2.56f && height >= 0.0 && tr.plane.normal[2] >= 0.7)
	{
		//	g_Engine.pfnClientCmd( "+alt1" );
		return true;
	}
	return false;
}



void ComputeMove(int id, float forwardmove, float sidemove)
{
	Vector vecVelocity = g_Player[id].vVelocity;

	float wtf_const = 33.4;

	float flSpeed = vecVelocity.Length2D() * wtf_const;

	cl_entity_s *ent = g_Engine.GetEntityByIndex(id);

	if (flSpeed > 0 && ent)
	{
		QAngle QAngles = ent->angles;

		Vector vecVelocityRotated;
		vecVelocityRotated.VectorRotate(vecVelocity, QAngles);

		forwardmove = vecVelocityRotated[0] * wtf_const;
		sidemove = -1 * vecVelocityRotated[1] * wtf_const;

		if (forwardmove > 250)
			forwardmove = 250;
		
		if (sidemove > 250)
			sidemove = 250;
	}
	else {
		forwardmove = 0;
		sidemove = 0;
	}
}

void CSystems::KnifeBot(struct usercmd_s *cmd)
{
	if (IsCurWeaponKnife() && CanAttack() && cvar.knifebot)
	{
		int id = NULL;
		float flDist = 8192.f;

		for (int i = 1; i <= g_Engine.GetMaxClients(); i++)
		{
			if (i == g_Local.iIndex)
				continue;

			if (!g_Player[i].bAlive)
				continue;

			if (g_Player[i].bFriend)
				continue;

			if (!g_Player[i].bVisible)
				continue;

			if (g_Player[i].iTeam == g_Local.iTeam)
				continue;

			if (g_Player[i].flDist < flDist || id == NULL)
			{
				flDist = g_Player[i].flDist;
				id = i;
			}
		}

		if(id > 0)
		{
			cl_entity_s *ent = g_Engine.GetEntityByIndex(id);

			if (!ent) 
				return;

			int fDidHit = FALSE;

			Vector vecPlayerOrigin = ent->origin + g_Player[id].vVelocity * g_Player[id].flFrametime;

			if (vecPlayerOrigin.Distance(pmove->origin) < 64)
			{
				// hit
				fDidHit = TRUE;

				float flDamage = 65.0f;

				Vector vecForwardPlayer, vecForward;

				Vector vecSrc = g_Local.vEye;

				QAngle QAimAngle;

				g_Utils.VectorAngles(vecPlayerOrigin - vecSrc, QAimAngle);

				QAimAngle.Normalize();

				g_Engine.pfnAngleVectors(QAimAngle, vecForward, 0, 0);
				g_Engine.pfnAngleVectors(ent->angles, vecForwardPlayer, 0, 0);

				vecForwardPlayer.Normalize();
				vecForward.Normalize();

				float flDot = vecForward.Dot2D(vecForwardPlayer);

				//Triple the damage if we are stabbing them in the back.
				if (flDot > 0.80f)
				{
					flDamage *= 3.0f;
				}

				if (fDidHit) 
				{
					g_Utils.MakeAngle(false, QAimAngle, cmd);

					cmd->buttons |= IN_ATTACK2;
				}
			}
		}
	}
}




void CSystems::SimulateMouse(int cyka)
{
	INPUT    Input = { 0 };													// Create our input.

	Input.type = INPUT_MOUSE;									// Let input know we are using the mouse.
	Input.mi.dwFlags = MOUSEEVENTF_WHEEL;							// We are setting left mouse button down.
	Input.mi.dwExtraInfo = 0;

	Input.mi.time = 0;
	Input.mi.mouseData = cyka;
	SendInput(1, &Input, sizeof(INPUT));								// Send the input.

	ZeroMemory(&Input, sizeof(INPUT));									// Fills a block of memory with zeros.
}



void CSystems::BunnyHop(struct usercmd_s *cmd)
{

	if (cvar.bunnyhop == 1)
	{
		if (cmd->buttons&IN_JUMP)
		{
			cmd->buttons &= ~IN_JUMP;

			if (pmove->flags & FL_ONGROUND || pmove->waterlevel >= 2)
			{
				cmd->buttons |= IN_JUMP;
			}
		}
	}
	else if (cvar.bunnyhop == 2)
	{
		if (GetAsyncKeyState(0x20) < 0)
		{
			if (pmove->flags & FL_ONGROUND || pmove->waterlevel >= 2)
			{
			
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
			}
		}

	}

	else if (cvar.bunnyhop == 3)
	{
		if (GetAsyncKeyState(0x02) < 0)
		{
			if (pmove->flags & FL_ONGROUND || pmove->waterlevel >= 2)
			{
				Sleep(0.8);
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
			}
		}

	}
	else if (cvar.bunnyhop == 5)
	{
	/**
		start:
			while (true) {
				if (GetAsyncKeyState(VK_SPACE))
				{
					while (true)
					{
						Sleep(20); //play around with this for other games// 20 is best for cs 1.6
						mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0); //make sure you have mwheeldown binded to +jump in game
						if (GetKeyState(VK_SPACE))
						{
							goto start;
						}
					}
				}
			}
			*/
		

	}

	else if (cvar.bunnyhop == 4)
	{

		static int frames_to_waste_on_ground;
		static int row_perfect, row_good;
		float speedcap = g_Local.flMaxSpeed*1.2f + 50.f;
		if (pmove->flags & FL_ONGROUND)
		{
			if (cvar.groundframes < 0)
				cvar.groundframes = 0;
			cvar.groundframes++;
			if (cvar.groundframes > 5)
			{
				cvar.inBhop = false;
				cvar.ideal_jumps = 2;
			}
			if (cvar.groundframes > 50)
				cvar.groundframes = 50;
		}
		else
		{
			if (cvar.groundframes > 0)
				cvar.groundframes = 0;
		}
		g_Engine.pfnGetScreenInfo(&g_Screen);

		static int counter;
		static int doublekey;
		static int frames_per_strafe;
		if (~pmove->flags&FL_ONGROUND)
		{
			float anglez[3];
			g_Engine.GetViewAngles(anglez);


			g_Engine.SetViewAngles(anglez);
		}

		if (pmove->flags&FL_ONGROUND)
		{
			counter = 0;
			doublekey = 0;
			if (!g_Local.bOnGround)
			{
				cvar.groundframes = 0;
				g_Local.bOnGround = true;
				frames_per_strafe = (int)g_Engine.pfnRandomLong(8, 9);
			}
			cvar.groundframes = ++cvar.groundframes > 50 ? 50 : cvar.groundframes;
		}
		else
		{
			g_Local.bOnGround = false;
		}

		float clientTime = g_Engine.GetClientTime();

		if (bJumpScroll)
		{
			scroll_Counter[0]++;
			lastScroll_Time[0] = clientTime + 0.15;
		}

		if (bDuckScroll)
		{
			scroll_Counter[1]++;
			lastScroll_Time[1] = clientTime + 0.15;
		}



		bDuckScroll = false;
		bJumpScroll = false;
		bDuckJustPressed = false;
		bJumpJustPressed = false;

		if (GetAsyncKeyState(0x20) < 0)
		{
			//SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
			if (asdfg(4) && !isGoodJump() && !isBadJump() && !asdfg_scrolled)
			{
				scrolling = false;
				asdfg_scrolled = true;
				frames_without_scroll = 0;
				scroll_forced = false;
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
				SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
			}
			if (asdfg_scrolled && !scroll_forced)
			{
				if (frames_without_scroll == 3)
				{
					scrolling = false;
					scroll_forced = true;
					mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
					SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
				}
			}

			if (cvar.ideal_jumps > 0 || cvar.inPubstomp)
			{
				if (isBadJump() && !isGoodJump() && g_Local.flXYSpeed >= g_Local.flMaxSpeed*1.2 &&  g_Local.flXYSpeed < speedcap)
				{
					scrolling = false;
					mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
					SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
				}

				if (isGoodJump() && !isBadJump() && (g_Local.flXYSpeed < g_Local.flMaxSpeed*1.2 || g_Local.flXYSpeed>speedcap))
				{
					goodjump_scrolled = 6;
					scrolling = false;
					mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
					SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
				}
				if (cvar.groundframes > 0 && ~g_Local.iOldButtons&IN_JUMP && (g_Local.flXYSpeed < g_Local.flMaxSpeed*1.2 || g_Local.flXYSpeed>speedcap))
				{
					scroll_forced = false;
					asdfg_scrolled = false;
					mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
					SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
					pattern = g_Engine.pfnRandomLong(0, MAX_PATTERNS - 1);
					counter = -1;
					poo = g_Engine.pfnRandomLong(7, 14);

					if (g_Engine.pfnRandomLong(1, 6) <= 2)
						scrollpatterns[pattern][6] = 1;
					else	scrollpatterns[pattern][6] = 0;

					if (g_Engine.pfnRandomLong(1, 33) <= 11)
						scrollpatterns[pattern][0] = 1;
					else	scrollpatterns[pattern][0] = 0;

					//fail_bhop = g_Engine.pfnRandomLong(1,10) <= 4 ? true : false;

					if (g_Engine.pfnRandomLong(1, 10) <= 4)
						frames_to_waste_on_ground = 1;
					else frames_to_waste_on_ground = 2;

					prevent_next_slowdown = g_Engine.pfnRandomLong(1, 6) <= 2 ? true : false;

					scrolling = true;

					if (!cvar.inPubstomp)
						cvar.ideal_jumps--;
				}
			}
			else
			{
				if (isBadJump() && frames_to_waste_on_ground == 2)
				{
					scrolling = false;
					//if( (g_Local.flXYSpeed <= g_Local.flMaxSpeed*1.2 && prevent_next_slowdown) || (g_Local.flXYSpeed < g_Local.flMaxSpeed-15.0f) )
					if ((prevent_next_slowdown) || (g_Local.flXYSpeed < g_Local.flMaxSpeed - 15.0f))
						frames_to_waste_on_ground = 1;
					else
						mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
						SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
				}
				if (isGoodJump() && frames_to_waste_on_ground != 2)
				{
					goodjump_scrolled = 6;
					scrolling = false;
					mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
					SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
				}
				if (~g_Local.iOldButtons&IN_JUMP && ((frames_to_waste_on_ground == 0 && cvar.groundframes > 0) || (cvar.groundframes == frames_to_waste_on_ground || cvar.groundframes > 2)))
				{
					scroll_forced = false;
					asdfg_scrolled = false;
					mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);
					SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
					pattern = g_Engine.pfnRandomLong(0, MAX_PATTERNS - 1);
					counter = -1;
					poo = g_Engine.pfnRandomLong(7, 14);

					if (g_Engine.pfnRandomLong(1, 6) <= 2)
						scrollpatterns[pattern][6] = 1;
					else	scrollpatterns[pattern][6] = 0;

					fail_bhop = g_Engine.pfnRandomLong(1, 10) <= 4 ? true : false;

					if (g_Engine.pfnRandomLong(1, 10) <= 4)
						frames_to_waste_on_ground = 1;
					else frames_to_waste_on_ground = 2;

					prevent_next_slowdown = g_Engine.pfnRandomLong(1, 4) == 2 ? true : false;

					scrolling = true;
				}
			}
		}
	}

	static bool rand_strafe;

	if (cvar.ddrun)
	{


		if ((GetAsyncKeyState(VK_MENU) < 0) && g_Local.bAlive && !cvar.inPubstomp)
		{
			if (pmove->flags&FL_ONGROUND && cvar.groundframes >= cvar.gStrafeGroundDD && !pmove->bInDuck && ~pmove->flags&FL_DUCKING)
			{
				cvar.gStrafeGroundDD = cvar.gStrafeGroundDD <= 1 ? 2 : 1;

				//SimulateMouse(cvar.bScrollJumpUp ? -120 : 120);
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrollup, 0);

				cvar.afterGstrafeFrame = (int)g_Engine.pfnRandomLong(0, 5);
			}
		}
		if (cvar.afterGstrafeFrame > 0)
		{
			cvar.afterGstrafeFrame--;
			if (cvar.afterGstrafeFrame == 0)
				SimulateMouse(cvar.bScrollJumpUp ? -120 : 120);
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrollup, 0);

		}

		if (scrolling)
		{
			if (counter != -1 && scrollpatterns[pattern][counter] == 1)
				//SimulateMouse(cvar.bScrollJumpUp ? 120 : -120);
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolldown, 0);


			counter++;
			if (counter == poo)
			{
				scrolling = false;
				counter = 0;
			}
		}

		if (~pmove->cmd.buttons &IN_JUMP)
			frames_without_scroll++;
		else
			frames_without_scroll = 0;


		//spectating_player_valid = false;

		static int state = 0;
		static int i = 0;
		if (i > 0) {
			if (state == 1)
			{
				state = 2;
				i--;
				return;
			}
			if (state == 2)
			{
				SimulateMouse(cvar.bScrollJumpUp ? -120 : 120);
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrollup, 0);

				state = 1;
				i--;
				return;
			}
		}
		else if (state != 0) { state = 0; i = 0; }
		if (g_Local.bAlive && cvar.inGstrafe && !cvar.inPubstomp)
		{

			if (pmove->flags&FL_ONGROUND && cvar.groundframes >= cvar.Gstrafe && !pmove->bInDuck &&  state == 0 && i == 0) { //gorundframes >= cvar.inGstrafe, add cvar.groundframes
				cvar.Gstrafe = (int)g_Engine.pfnRandomLong(1, 2); //== 1 ? 3 : 2;                                        
				SimulateMouse(cvar.bScrollJumpUp ? -120 : 120);
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrollup, 0);


				state = 1;

				i = (int)g_Engine.pfnRandomLong(4, 6); //4-6 mnogo
			}
		}

	}
}



void CSystems::AutoStrafe(struct usercmd_s *cmd, float frametime)
{
	float yaw;
	vec3_t viewangles; g_Engine.GetViewAngles(viewangles);
	yaw = viewangles.y;

	static float old_yaw;
	static float angle_diff;
	static float angle_diff_old;
	static float right_peak;
	static float left_peak;

	static int right_fall;
	static int left_fall;

	angle_diff = old_yaw - yaw;
	if (angle_diff > 300.0)
		angle_diff -= 360.0;
	if (angle_diff < -300.0)
		angle_diff += 360.0;

	static bool bJumped;
	if (cvar.autostrafe) {
		if (!bJumped && ~pmove->flags&FL_ONGROUND)
		{
			g_Engine.pfnClientCmd("-forward");
			//renameCommand( "-forward", "-f" );
			//renameCommand( "-moveright", "-m" );
			bJumped = true;
		}
		if (bJumped && pmove->flags&FL_ONGROUND)
		{
			//renameCommand( "-f", "-forward" );
			//renameCommand( "-m", "-moveright" );
			SimulateKey(DIKEYBOARD_NUMPAD4, 0);
			SimulateKey(DIKEYBOARD_NUMPAD6, 0);

			right_peak = 0.0;
			left_peak = 0.0;
			right_fall = 0;
			left_fall = 0;
		}
		if (~pmove->flags & FL_ONGROUND)
		{
			if (angle_diff > 0.0)
			{
				left_fall = 0;
				left_peak = 0.0;
				if (angle_diff > right_peak)
					right_peak = angle_diff;

				if (angle_diff < right_peak)
					right_fall++;

				if (right_fall > 1)
				{
					//	renameCommand( "-m", "-moveright" );
					SimulateKey(DIKEYBOARD_NUMPAD4, 1); // PRESS Aw
					SimulateKey(DIKEYBOARD_NUMPAD6, 0); // RELEASE D
				}
				else
				{
					//	renameCommand( "-moveright", "-m" );
					SimulateKey(DIKEYBOARD_NUMPAD4, 0); // RELEASE A
					SimulateKey(DIKEYBOARD_NUMPAD6, 1); // PRESS D
				}
			}
			else if (angle_diff < 0.0)
			{
				right_fall = 0;
				right_peak = 0.0;
				if (angle_diff < left_peak)
					left_peak = angle_diff;

				if (angle_diff > left_peak)
					left_fall++;
				//renameCommand( "-m", "-moveright" );
				if (left_fall > 1)
				{
					SimulateKey(DIKEYBOARD_NUMPAD4, 0); // RELEASE A
					SimulateKey(DIKEYBOARD_NUMPAD6, 1); // PRESS D
				}
				else
				{
					SimulateKey(DIKEYBOARD_NUMPAD4, 1); // PRESS A
					SimulateKey(DIKEYBOARD_NUMPAD6, 0); // RELEASE D
				}
			}
		}
	}
}
void StrafeEmulator(void)
{
	
}


void FastRunFunction(usercmd_s *cmd)
{
	if (g_Local.flXYspeed && g_Local.flMaxSpeed == 0 && g_Local.iFlags&FL_ONGROUND && IsCurWeaponKnife)
	{
		if ((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVELEFT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVERIGHT))
		{
			if (bFastRun) { bFastRun = false; cmd->sidemove -= 89.6f; cmd->forwardmove -= 89.6f; }
			else { bFastRun = true;  cmd->sidemove += 89.6f; cmd->forwardmove += 89.6f; }
		}
		else if ((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVERIGHT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVELEFT))
		{
			if (bFastRun) { bFastRun = false; cmd->sidemove -= 89.6f; cmd->forwardmove += 89.6f; }
			else { bFastRun = true;  cmd->sidemove += 89.6f; cmd->forwardmove -= 89.6f; }
		}
		else if (cmd->buttons&IN_FORWARD || cmd->buttons&IN_BACK)
		{
			if (bFastRun) { bFastRun = false; cmd->sidemove -= 126.6f; }
			else { bFastRun = true;  cmd->sidemove += 126.6f; }
		}
		else if (cmd->buttons&IN_MOVELEFT || cmd->buttons&IN_MOVERIGHT)
		{
			if (bFastRun) { bFastRun = false; cmd->forwardmove -= 126.6f; }
			else { bFastRun = true;  cmd->forwardmove += 126.6f; }
		}
	}

	bFastRun = cvar.fastrun;
}













inline float PlayerHeight(int usehull)
{
	Vector vTemp = g_Local.vOrigin;
	vTemp[2] -= 8192;
	pmtrace_t *trace = g_Engine.PM_TraceLine(g_Local.vOrigin, vTemp, 1, usehull, -1);
	vec3_t vDistance = (vTemp - g_Local.vOrigin) * trace->fraction;
	float ret = -vDistance[2];
	vTemp = trace->endpos;

	pmtrace_t pTrace;
	g_Engine.pEventAPI->EV_SetTraceHull(usehull);
	g_Engine.pEventAPI->EV_PlayerTrace(g_Local.vOrigin, vTemp, PM_GLASS_IGNORE | PM_STUDIO_BOX, g_Local.iIndex, &pTrace);

	if (pTrace.fraction < 1.0f) {
		vDistance = (vTemp - g_Local.vOrigin) * pTrace.fraction;
		ret = -vDistance[2];
		int ind = g_Engine.pEventAPI->EV_IndexFromTrace(&pTrace);
		if (ind > 0 && ind < 33) {
			float dst = g_Local.vOrigin.z - (g_Local.iUseHull == 0 ? 32 : 18) - g_Player[ind].vOrigin.z - ret;
			if (dst < 30)
				ret -= 14.0f;
		}
	}
	return ret;
}

