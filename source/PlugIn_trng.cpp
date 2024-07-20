// FOR_YOU:
// This is the main source of your plugin.
// It's in this source you'll type all your assembly or c++ code and all 
// the variables and memory zones you need.
#define _USE_MATH_DEFINES
// ************  Top/Header section ************
#include "stdafx.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <malloc.h>
#include "bass.h"		// prototypes for extra sound library: bass.dll
	// Following header files will be updated for every new version of 
	// the tomb_NextGeneration.dll, so it's better you don't change them
	//  because they will be replaced for any new update.

#include "Tomb_NextGeneration.h" // mnemonic constants defined in tomb_NextGeneration.dll
#include "structures.h" // structure of tomb4 program and trng dll
#include "DefTomb4Funct.h" // defines of tomb4 procedure prototypes
#include "functions.h"  // assigments of real tomb4 address to each tomb4 procedures
#include "macros.h"  // definition of macros
	// FOR_YOU:
	// While the followings are the headers you can use 
	// to type your structures, constants and new tomb4 procedures you 
	// discovered. 
	// Following files are only yours and trng will not ever change them:
#include "macros_mine.h"  // here you define your own macros
#include "constants_mine.h" // here you define your mnemonic constants
#include "structures_mine.h" // here you type your structure definitions
#include "Tomb4Discoveries_mine.h" // here type tomb4 procedures you discovered

#include "trng.h" // list of trng functions imported from trng.cpp source. 

#pragma warning( error : 4706 )
#pragma warning(disable: 4996)

#define GRID_SIZE 4
#define RGBA(r, g, b, a) (b | (g << 8) | (r << 16) | (a << 24)) 
#define RAD(a) ((a) * 3.14159265f / 32768)

// ************  Early function declarations ************

void SetParamSubtitles(int number);
void TriggerLaraSparks(long);
void DrawBat();
void UpdateBat();
void PrintBloodShadow(short size, short* box, Tr4ItemInfo* item);
void DrawVfx();

void DoLaraTorch();
void DoFreeTorch(short item_num);
 
// ************  Global Variables Section *************
// FOR_YOU:
// here you type the variables or memory zones you wish use in different
// C++ procedures.
// If you use the assembly you'll have to type ALL your variables in this
// section.
// for example if you wish have a 32 bits signed variable to store big
// numbers you can type:
// TYPE_HERE:
// int MyNumber;
// and then you can use it in asm in this way:
//		mov  eax, [MyNumber]
// or
//      mov eax, dword ptr [MyNumber]
// same speech for wide memory zones.
// If you wish having a memory zone of 13000 bytes you can type:
//  BYTE MyZone[13000];
// and then you can use it in this way:
//     mov  al, MyNumber[ecx]  ; read the ECXth byte and copy it in AL
// or also:
//     mov al, [MyNumber+ecx] ; same result of above instruction

// Variables and memory zone to TYPE_HERE:

HINSTANCE MyDllInstance=NULL;  // the instance handle of your dll

extern char BufferLog[4096]; // temporary global buffer to host error and warning messages

// FOR_YOU: If you mean create code patch (changes in tomb4 code to call your code in the plugin dll,
// you MUST change the MyTomb4PatcherAddress value (here below) 
// You can discover a free tomb4 zone using TrngPatcher program, with the 
// menu command "Tools->Get Assignment of Free Memory range"
// Note: choose an address, in the given range, terminating with hex 
// digits: 0,4,8, or C
// because it's usefull that the address was a multiple by 4
// If you chose an address used from other plugins you'll get an error and
// the game will be aborted
// note: if you don't mean use code patches you can let 0x0 in following line
DWORD MyTomb4PatcherAddress = 0x50DFF0; // <- TYPE_HERE: the new address you chose
								
// this text will contain your plugin name (omitting .dll extension).
// it will be used to give a title to messagebox for error messages or warnings
char TexMyPluginName[80];  
// in this MyData structure you can store all global variables for your plugin
// you have to define them in structures_mine.h source inside structure "StrMyData" or for variable
// to save and restore in savegames, in "StrSavegameGlobalData" or "StrSavegameLocalData" structures
StrMyData MyData;
SubtitleTicket Ticket;
RadioNoise Radio;
MenuYesNo Menu;

BAT_TR3 bats[MAX_BATS];
long next_bat;

// ************  Utilities section  ****************

inline int round(float x)
{
	return (x > 0) ? int(x+0.5f) : int(x-0.5f);
}

// replaces lara_as_* routine entry for given stateNum with func
void ReplaceLaraAsRoutine(short stateNum, void* func)
{
	void** lara_as = (void**)TRNG_LARA_AS;
	lara_as[stateNum] = func;
}

// replaces lara_col_* routine entry for given stateNum with func
void ReplaceLaraColRoutine(short stateNum, void* func)
{
	void** lara_col = (void**)TRNG_LARA_COL;
	lara_col[stateNum] = func;
}

// ************  Patcher Functions section  ***************
// Note: please, don't change or remove the C++ procedure you find here,
// because they are necessary to do work the plugin with the trng dll
// Anyway in many of these functions you can add your code

int Patch_FlareTorch(void)
{
	static BYTE PatchBytesLara[] = { 0x66, 0xB8, 0x00, 0x00, 0xFF, 0x25, 0xF0, 0xDF, 0x50, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90};

	static BYTE PatchBytesTorch[] = { 0x66, 0xB8, 0x01, 0x00, 0xFF, 0x25, 0xF0, 0xDF, 0x50, 0x00 };

	return (ApplyCodePatch(0x41F87A, PatchBytesLara, 15) & ApplyCodePatch(0x41FBB8, PatchBytesTorch, 10));
}

int Patch_UpdateBats(void)
{
	static BYTE VetBytes[]={0x66, 0xB8, 0x2, 0x0, 0xFF, 0x15, 0xF0, 0xDF, 0x50, 0x0};

	return ApplyCodePatch(0x4491C1, VetBytes, 10);
}

int Patch_DrawBats(void)
{
	static BYTE VetBytes[]={0x66, 0xB8, 0x3, 0x0, 0xFF, 0x15, 0xF0, 0xDF, 0x50, 0x0};

	return ApplyCodePatch(0x44E8DC, VetBytes, 10);
}

int Patch_BloodShadows(void)
{
	static BYTE VetBytes[]={0x66, 0xB8, 0x04, 0x00, 0xFF, 0x25, 0xF0, 0xDF, 0x50, 0x00, 0x90, 0x90, 0x90, 0x90};
	return ApplyCodePatch(0x485480, VetBytes, 14);
}

int Patch_DisablePoisonHealthbar(void)
{
	static BYTE VetBytes1[] = { 0x90, 0x90, 0x90, 0x90 };
	static BYTE VetJmpByte[] = { 0xEB, 0x23, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	static BYTE OnlyCritical[] = { 0x60 };
	ApplyCodePatch(0x449022, VetBytes1, 4);
	ApplyCodePatch(0x452356, VetJmpByte, 11);
	ApplyCodePatch(0x45231A, OnlyCritical, 1);

	return 1;
}


// FOR_YOU: In this function you insert the callings of functions used to change tomb4 code
// You can get these functions, in the correct format, using Trng Core -> Asm Souce Editor -> Debugging menu
// program
// note: if there is a conflict the function will return "false"
// and the tomb4 program will abort with an error message
bool CreateMyCodePatches(void)
{
	// the call of the code patch to TYPE_HERE:
	// example: 
	// SET_PATCH(Path_RedirCollision)
	// to call the function Patch_RedirCollision() created with TrngPatcher program (command Assmembly->Create Dynamic Patch Generator)
	if (SetReservedDataZone(0x50DFF0, 128) != APPC_OK) return false;
	SET_PATCH(Patch_FlareTorch);
	SET_PATCH(Patch_UpdateBats);
	SET_PATCH(Patch_DrawBats);
	SET_PATCH(Patch_BloodShadows);
	SET_PATCH(Patch_DisablePoisonHealthbar);
	return true;
}

// ************  Assembly Procedures section  ******************

// FOR_YOU: In the SubPatchArray you'll type all procedure names of your code in the dll you
// wish were called from tomb4 code.
// type them in the order of ax value. So first asm proc in the list, will be called
// with ax=0, while the second in the list will be called with ax=1 ect.

BEGIN_ASM_PROC(Patch_00)
	CALL DoLaraTorch
	MOV EAX, 41F920h
	JMP EAX
END_ASM_PROC


BEGIN_ASM_PROC(Patch_01)
	PUSH EDI
	CALL DoFreeTorch
	ADD ESP, 4h
	MOV EAX, 41FC12h
	JMP EAX
END_ASM_PROC


BEGIN_ASM_PROC(Patch_02)
	CALL UpdateBeetles
	CALL UpdateFish
	CALL UpdateBat
	RETN
END_ASM_PROC


BEGIN_ASM_PROC(Patch_03)
	CALL DrawBeetles
	CALL DrawFish
	CALL DrawBat
	CALL DrawVfx
	RETN
END_ASM_PROC


BEGIN_ASM_PROC(Patch_04)
	MOVSX EAX, WORD PTR [ESP+4h]
	MOV ECX, DWORD PTR [ESP+8h]
	MOV EDX, DWORD PTR [ESP+0Ch]
	PUSH EDX
	PUSH ECX
	PUSH EAX
	CALL PrintBloodShadow
	ADD ESP, 0Ch
	MOV EAX, 48590Eh
	JMP EAX
END_ASM_PROC


void *SubPatchArray[] = {
	
// TYPE_HERE your asm procedure names to call from tomb4 code
	&Patch_00,
	&Patch_01,
	&Patch_02,
	&Patch_03,
	&Patch_04,
	NULL
};


// ************  MainPatcher() asm procedure  *****************

// FOR_YOU: This is your main patcher procedure.
// All your patch in tomb4 code will call this procedure passing to it, in ax register,
// the number of subpatch to call

BEGIN_ASM_PROC(MainPatcher)
	and eax, 0ffffh
	mov eax, dword ptr [SubPatchArray+eax*4];
	jmp eax
END_ASM_PROC



// ************  CallBack functions section  *****************

void lara_as_walk(Tr4ItemInfo* item, Tr4CollInfo* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = 2;
		return;
	}

	if (lara.IsMoving)
		return;

	if (input & CMD_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -728)
			lara.turn_rate = -728;
	}
	else if (input & CMD_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 728)
			lara.turn_rate = 728;
	}

	if (input & CMD_UP)
	{
		if (lara.water_status == 4)
			item->goal_anim_state = 65;
		else if (input & CMD_JUMP)
			item->goal_anim_state = 1;
		else
			item->goal_anim_state = 0;
	}
	else
		item->goal_anim_state = 2;
}

void lara_as_back(Tr4ItemInfo* item, Tr4CollInfo* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = 2;
		return;
	}

	if (!lara.IsMoving)
	{
		if (input & CMD_DOWN)
			item->goal_anim_state = 16;
		else
			item->goal_anim_state = 2;

		if (input & CMD_LEFT)
		{
			lara.turn_rate -= 409;

			if (lara.turn_rate < -728)
				lara.turn_rate = -728;
		}
		else if (input & CMD_RIGHT)
		{
			lara.turn_rate += 409;

			if (lara.turn_rate > 728)
				lara.turn_rate = 728;
		}
	}
}

void lara_as_run(Tr4ItemInfo* item, Tr4CollInfo* coll)
{
	//static long jump_ok = 1;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = 8;
		return;
	}

	if (input & CMD_ROLL)
	{
		item->anim_number = 146;
		item->frame_number = anims[146].frame_base + 2;
		item->current_anim_state = 45;
		item->goal_anim_state = 2;
		return;
	}

	/*
	if (input & CMD_DASH && DashTimer)
	{
		item->goal_anim_state = 73;
		return;
	}

	if (input & CMD_DUCK && lara.water_status != 4 &&
		(lara.gun_status == 0|| lara.gun_type == 0 || lara.gun_type == 1 ||
			lara.gun_type == 2 || lara.gun_type == 3 || lara.gun_type == 7))
	{
		item->goal_anim_state = 71;
		return;
	}
	*/

	if (input & CMD_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -1456)
			lara.turn_rate = -1456;

		item->pos.z_rot -= 273;

		if (item->pos.z_rot < -2002)
			item->pos.z_rot = -2002;
	}
	else if (input & CMD_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 1456)
			lara.turn_rate = 1456;

		item->pos.z_rot += 273;

		if (item->pos.z_rot > 2002)
			item->pos.z_rot = 2002;
	}

	/*
	if (item->anim_number == 6)
		jump_ok = 0;
	else if (item->anim_number != 0 || item->frame_number == 4)
		jump_ok = 1;

	if (input & CMD_JUMP && jump_ok && !item->gravity_status)
		item->goal_anim_state = 3;
	*/

	else if (input & CMD_UP)
	{
		if (lara.water_status == 4)
			item->goal_anim_state = 65;
		else if (input & CMD_JUMP)
			item->goal_anim_state = 1;
		else
			item->goal_anim_state = 0;
	}
	else
		item->goal_anim_state = 2;
}

void lara_as_turn_r(Tr4ItemInfo* item, Tr4CollInfo* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = 2;
		return;
	}

	lara.turn_rate += 409;

	if (lara.gun_status != 4 || lara.water_status == 4)
	{
		if (lara.turn_rate > 728)
		{
			if (input & CMD_WALK || lara.water_status == 4)
				lara.turn_rate = 728;
			else
				item->goal_anim_state = 20;
		}
	}
	else
		item->goal_anim_state = 20;

	if (!(input & CMD_UP))
	{
		if (!(input & CMD_RIGHT))
			item->goal_anim_state = 2;

		return;
	}

	if (lara.water_status == 4)
		item->goal_anim_state = 65;
	else if (input & CMD_JUMP)
		item->goal_anim_state = 1;
	else
		item->goal_anim_state = 0;
}

void lara_as_turn_l(Tr4ItemInfo* item, Tr4CollInfo* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = 2;
		return;
	}

	lara.turn_rate -= 409;

	if (lara.gun_status != 4 || lara.water_status == 4)
	{
		if (lara.turn_rate < -728)
		{
			if (input & CMD_WALK || lara.water_status == 4)
				lara.turn_rate = -728;
			else
				item->goal_anim_state = 20;
		}
	}
	else
		item->goal_anim_state = 20;

	if (!(input & CMD_UP))
	{
		if (!(input & CMD_LEFT))
			item->goal_anim_state = 2;

		return;
	}

	if (lara.water_status == 4)
		item->goal_anim_state = 65;
	else if (input & CMD_JUMP)
		item->goal_anim_state = 1;
	else
		item->goal_anim_state = 0;
}

void lara_as_stop(Tr4ItemInfo* item, Tr4CollInfo* coll)
{
	short height, ceiling, fheight, rheight;

	fheight = NO_HEIGHT;
	rheight = NO_HEIGHT;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = 8;
		return;
	}

	if (item->anim_number != 226 && item->anim_number != 228)
		StopSoundEffect(3);

	if (UseInventoryItems(item))
		return;

	if (input & CMD_ROLL && lara.water_status != 4)
	{
		item->anim_number = 146;
		item->frame_number = anims[146].frame_base + 2;
		item->current_anim_state = 45;
		item->goal_anim_state = 2;
		return;
	}

	/*if (input & CMD_DUCK && lara.water_status != 4 && item->current_anim_state == 2 &&
		(lara.gun_status == 0 || lara.gun_type == 0 || lara.gun_type == 1 ||
			lara.gun_type == 2 || lara.gun_type == 3 || lara.gun_type == 7))
	{
		item->goal_anim_state = 71;
		return;
	}*/

	item->goal_anim_state = 2;

	if (input & CMD_LOOK)
		LookUpDown();

	if (input & CMD_UP)
		fheight = LaraFloorFront(item, item->pos.y_rot, 104);
	else if (input & CMD_DOWN)
		rheight = LaraFloorFront(item, item->pos.y_rot + 32768, 104);

	if (input & 0x400)
	{
		height = LaraFloorFront(item, item->pos.y_rot - 16384, 116);
		ceiling = LaraCeilingFront(item, item->pos.y_rot - 16384, 116, 762);

		if (height < 128 && height > -128 && height_type != 2 && ceiling <= 0)
			item->goal_anim_state = 22;
	}
	else if (input & 0x800)
	{
		height = LaraFloorFront(item, item->pos.y_rot + 16384, 116);
		ceiling = LaraCeilingFront(item, item->pos.y_rot + 16384, 116, 762);

		if (height < 128 && height > -128 && height_type != 2 && ceiling <= 0)
			item->goal_anim_state = 21;
	}
	else if (input & CMD_LEFT)
		item->goal_anim_state = 7;
	else if (input & CMD_RIGHT)
		item->goal_anim_state = 6;

	if (lara.water_status == 4)
	{
		/*if (input & CMD_JUMP)
			item->goal_anim_state = 15;*/

		if (input & CMD_UP)
		{
			if (fheight >= 383 || fheight <= -383)
			{
				lara.move_angle = item->pos.y_rot;
				coll->bad_pos = -NO_HEIGHT;
				coll->bad_neg = -384;
				coll->bad_ceiling = 0;
				coll->radius = 102;
				coll->slopes_are_walls = 1;
				GetLaraCollisionInfo(item, coll);

				if (!TestLaraVault(item, coll))
					coll->radius = 100;
			}
			else
				lara_as_wade(item, coll);
		}
		else if (input & CMD_DOWN && rheight < 383 && rheight > -383)
			lara_as_back(item, coll);
	}
	//else if (input & CMD_JUMP)
		//item->goal_anim_state = 15;
	else if (input & CMD_UP)
	{
		ceiling = LaraCeilingFront(item, item->pos.y_rot, 104, 762);
		height = LaraFloorFront(item, item->pos.y_rot, 104);

		if ((height_type == 2 || height_type == 3) && height < 0 || ceiling > 0)
		{
			item->goal_anim_state = 2;
			return;
		}

		if (height >= -256 || fheight >= -256)
		{
			if (input & CMD_JUMP)
				lara_as_run(item, coll);
			else
				lara_as_walk(item, coll);
		}
		else
		{
			lara.move_angle = item->pos.y_rot;
			coll->bad_pos = -NO_HEIGHT;
			coll->bad_neg = -384;
			coll->bad_ceiling = 0;
			coll->radius = 102;
			coll->slopes_are_walls = 1;
			GetLaraCollisionInfo(item, coll);

			if (!TestLaraVault(item, coll))
			{
				coll->radius = 100;
				item->goal_anim_state = 2;
			}
		}
	}
	else if (input & CMD_DOWN)
	{
		//if (input & CMD_WALK)
		{
			if (rheight < 383 && rheight > -383 && height_type != 2)
				lara_as_back(item, coll);
		}
		//else if (rheight > -383)
			//item->goal_anim_state = 5;
	}
}

// TYPE_HERE: your callback function

void cbInitProgram(int NumberLoadedPlugins, char *VetPluginNames[]) 
{
	// save number of plugins (enclosed yours and the tomb_nextgeneration.dll ) and their names
	// these data will be used to locate IDs of any loaded plugins using FindPluginID() function
	Trng.TotPlugins = NumberLoadedPlugins;
	Trng.pVetPluginNames = VetPluginNames;

	// clear all my global variables
	ClearMemory(&MyData, sizeof(StrMyData));
}


void cbInitGame(void)
{
	// here you can initialize your global data for whole adventure
	// this procedure will be called only once, before loading title level

	ReplaceLaraAsRoutine(0, lara_as_walk);
	ReplaceLaraAsRoutine(1, lara_as_run);
	ReplaceLaraAsRoutine(2, lara_as_stop);
	ReplaceLaraAsRoutine(6, lara_as_turn_r);
	ReplaceLaraAsRoutine(7, lara_as_turn_l);
	ReplaceLaraAsRoutine(16, lara_as_back);
}


void ModifyColors(int colIndex, const ColorRGB& primary)
{
	ColorRGB *color = (ColorRGB*) TOMB4_COLOR_TABLE;
	color += colIndex * 0x20;

	for (int i = 0; i < 16; i++)
	{
		color->Red = primary.Red;
		color->Green = primary.Green;
		color->Blue = primary.Blue;
		color += 2;
	}
}

void cbInitLevel(void)
{
	MyData.VignetteSprite = 50;
	MyData.electric = -1;

	if (!(*Trng.pGlobTomb4->pAdr->pLevelNow))
		return;

	MyData.Save.Local.tired = false;
	MyData.Save.Local.runCounter = STAMINA_MAX;
	MyData.subtitleLife = SUBTITLE_LIFE;
	MyData.subtitleY = 945 / 1080.0f;
	MyData.lineSpacing = 54 / 1080.0f;
	// replace FC_DARK_GRAY with pastel green
	ColorRGB c1 = {0x80, 0xFF, 0x80, 0xFF};
	ModifyColors(FC_DARK_GRAY, c1);

	Ticket.ResetTicket();

	Menu.SetupMenu(false, -1, -1, -1);

#ifdef MAGAZINES
	MyData.Save.Global.shotgunMag.alreadyFired = false;
	MyData.Save.Global.shotgunMag.reloadAnimNum = 5;
	MyData.Save.Global.shotgunMag.reloadFrame = 37;
	MyData.Save.Global.shotgunMag.shotsMax = 6;

	//MyData.Save.Global.revolverMag.alreadyFired = false;
	MyData.Save.Global.revolverMag.reloadAnimNum = 4;
	MyData.Save.Global.revolverMag.reloadFrame = 37;
	MyData.Save.Global.revolverMag.shotsMax = 6;
#endif

	Radio.enabled = false;
	Radio.sfxSlot = Radio.sfxSlot2 = -1;
	Radio.radius1 = 24*ONE_STEP;
	Radio.radius2 = 4*ONE_STEP;

	for (int i = 0; i < MyData.BaseCustomizeMine.TotCustomize; i++)
	{
		auto cust = MyData.BaseCustomizeMine.pVetCustomize[i];
		if (cust.CustValue == CUST_RADIO_STATIC)
		{
			short arguments[5] = {-1, -1, -1, -1, -1};
			for (int j = 0; j < cust.NArguments; j++)
				arguments[j] = cust.pVetArg[j];

			if (arguments[0] > 0)
				Radio.enabled = true;
			if (arguments[1] > 0)
				Radio.sfxSlot = arguments[1];

			Radio.sfxSlot2 = Radio.sfxSlot;
			if (arguments[2] > 0)
				Radio.sfxSlot2 = arguments[2];

			if (arguments[3] > 0)
				Radio.radius1 = arguments[3]*ONE_STEP;
			if (arguments[4] > 0)
				Radio.radius2 = arguments[4]*ONE_STEP;

			short *SfxLut = *(short**)TOMB4_SAMPLE_LUT;
			Tr4SampleInfo *samples = *(Tr4SampleInfo**)TOMB4_SAMPLE_INFO;

			Radio.normal = &samples[SfxLut[Radio.sfxSlot]];
			Radio.oldVol1 = Radio.normal->volume;

			Radio.danger = &samples[SfxLut[Radio.sfxSlot2]];
			Radio.oldVol2 = Radio.danger->volume;

			Radio.countdown = 0;
		}
		/*else if (cust.CustValue == CUST_VIGNETTE)
		{
			if (cust.NArguments && cust.pVetArg[0] > 0)
			{
				short nmeshes = -objects[SLOT_DEFAULT_SPRITES].nmeshes;
				if (cust.pVetArg[0] < nmeshes)
					MyData.VignetteSprite = cust.pVetArg[0];
			}
		}*/
	}
}

// called everytime player save the game (but also when lara move from a level to another HUB saving). 
// in this procedure your plugin will save its own data in savegame
// SavingType inform about the kind of saving it is happening (SAVT_... values)
// pAdrZone is a byte pointer where this procedure will save the address from where begin data to save in savegame
// this function will return the size of data to save
// note: It's better you don't change anything of this code. It will save byself of data you stored in MyData.Save
// structure. If you wish adding other variables to save, just you add new fields in Structures of MyData.Save 
DWORD cbSaveMyData(BYTE **pAdrZone, int SavingType)
{
	DWORD SizeData;
	int i;
	static WORD *pVetExtras;
	int TotNWords;
	int TotNewActions;


	if (SavingType & SAVT_COMPLETED) {
		// this call is not to save data but only it is a confirm that the previous saving has been completed
		// now we can free the temporary memory used to save the data in previous call
		if (pVetExtras != NULL) {
			FreeMemory(pVetExtras);
			pVetExtras=NULL;
		}

		return 0;
	}


	TotNWords=0;
	pVetExtras = (WORD *) GetMemory(16);
	// save id of my plugin in first word

	pVetExtras[TotNWords++] = Trng.IdMyPlugin;
	
	if (SavingType & SAVT_LOCAL_DATA) {
		// save local data

		// save Local structure
		AddNGToken(NGTAG_LOCAL_DATA, NO_ARRAY, sizeof(StrSavegameLocalData), &MyData.Save.Local, 
						&pVetExtras, &TotNWords);


		// save all (currently enabled) progressive actions
		// before saving, compact progressive action array to remove intermediate free records
		TotNewActions=0;

		for (i=0;i<MyData.TotProgrActions;i++) {
			if (MyData.VetProgrActions[i].ActionType != AXN_FREE) {

				MyData.VetProgrActions[TotNewActions] = MyData.VetProgrActions[i];

				TotNewActions++;
			}
		}
		// update new valuese after recompatting
		MyData.LastProgrActionIndex =0;
		MyData.TotProgrActions= TotNewActions;

		// store all progressive action records
		AddNGToken(NGTAG_PROGRESSIVE_ACTIONS, MyData.TotProgrActions, sizeof(StrProgressiveAction), 
				&MyData.VetProgrActions[0], &pVetExtras, &TotNWords);

	}

	if (SavingType & SAVT_GLOBAL_DATA) {
		// save global data
		AddNGToken(NGTAG_GLOBAL_DATA, NO_ARRAY, sizeof(StrSavegameGlobalData), &MyData.Save.Global , 
						&pVetExtras, &TotNWords);
	}
	// write final sequence
	AddTokenFinalSequence(&pVetExtras, &TotNWords);

	// return to trng the infos about start of memory where there are our data and their size:
	*pAdrZone = (BYTE *) pVetExtras;
	SizeData = TotNWords * 2;

	return SizeData;
	

}
// called when a savegame will be loaded (but also when lara move from a level to another)
// pAdrZone will point to memory zone with data just loaded from savegame
// SizeData is the size of data pointed by pAdrZone
// note: it's better you don't change anything of this code. It's alread thought to reload all data you saved in MyData.Save 
// structure. There is no need of changes by you
void cbLoadMyData(BYTE *pAdrZone, DWORD SizeData)
{

	WORD *pVetExtras;
	StrParseNGField  ParseField;
	int Indice;
	int i;
	WORD TotActions;

	
	pVetExtras = (WORD*) pAdrZone;

	Indice=0;

	while (ParseNgField(pVetExtras ,Indice, &ParseField)==true) {
		
		// recover different ng token
		switch (ParseField.Type) {
		case NGTAG_LOCAL_DATA:
			// local data
			memcpy(&MyData.Save.Local, ParseField.pData, sizeof(StrSavegameLocalData));
			break;

		case NGTAG_GLOBAL_DATA:
			// global data
			memcpy(&MyData.Save.Global, ParseField.pData, sizeof(StrSavegameGlobalData));
			break;
			
		case NGTAG_PROGRESSIVE_ACTIONS:
			// progressive actions
			i= ParseField.StartDataIndex;
			// read tot actions value
			TotActions = pVetExtras[i++];
			// copy all tot records
			memcpy(&MyData.VetProgrActions[0], &pVetExtras[i], sizeof(StrProgressiveAction) * TotActions);
			MyData.TotProgrActions = TotActions;
			break;
		}
		Indice= ParseField.NextIndex; 
	}

}
// free memory used to store all data about your customize commands loaded in previous level
void FreeMemoryCustomize(void)
{
	int i;

	for (i=0;i<MyData.BaseCustomizeMine.TotCustomize;i++) {
		FreeMemory(MyData.BaseCustomizeMine.pVetCustomize[i].pVetArg);
	}

	if (MyData.BaseCustomizeMine.TotCustomize > 0) {
		FreeMemory(MyData.BaseCustomizeMine.pVetCustomize);
		MyData.BaseCustomizeMine.TotCustomize=0;
	}


	MyData.BaseCustomizeMine.pVetCustomize=NULL;
}

// free memory used to store all data about your parameters commands loaded in previous level
void FreeMemoryParameters(void)
{
	int i;

	for (i=0;i<MyData.BaseParametersMine.TotParameters;i++) {
		FreeMemory(MyData.BaseParametersMine.pVetParameters[i].pVetArg);
	}

	if (MyData.BaseParametersMine.TotParameters > 0) {
		FreeMemory(MyData.BaseParametersMine.pVetParameters);
		MyData.BaseParametersMine.TotParameters=0;
	}

	MyData.BaseParametersMine.pVetParameters=NULL;
}

// this procedure will be called at end of any level
// you can type here code to free resources allocated for level (that quits now)
void FreeLevelResources(void)
{

	// free memory used to store all data about your customize commands loaded in previous level
	FreeMemoryCustomize();
	// free memory used to store all data about your parameters commands loaded in previous level
	FreeMemoryParameters();
	MyData.BaseAssignSlotMine.TotAssign=0;

}
// it will be called before beginning the loading for a new level.
// you can type here code to initialise all variables used for level (to clear old values changed by previous level)
// and to free resources allocated in old level since now we'are going to another new level.

void cbInitLoadNewLevel(void)
{
	int i;

	StrProgressiveAction *pAction;

	// clear all LOCAL variables
	ClearMemory(&MyData.Save.Local,sizeof(StrSavegameLocalData));

	// clear progressive actions
	pAction= &MyData.VetProgrActions[0];

	for (i=0;i<MyData.TotProgrActions;i++) {
		if (pAction->ActionType != AXN_FREE) {
			// here you could analise to free resoruce allocated from this specific action

			pAction->ActionType = AXN_FREE;
		}
	}

	MyData.TotProgrActions=0;
	MyData.LastProgrActionIndex=0;


	/*
	short *SfxLut = *(short**)TOMB4_SAMPLE_LUT;
	short realIndex = SfxLut[Radio.sfxSlot];
	Tr4SampleInfo *samples = *(Tr4SampleInfo**)TOMB4_SAMPLE_INFO;
	samples[realIndex].volume = Radio.oldVol;
	*/

	// here you can initialise other variables of MyData different than Local and progressive actions
	// free resources allocate in previous level
	FreeLevelResources();

}


void InitMenu(int number)
{
	for (int i = 0; i < MyData.BaseParametersMine.TotParameters; i++)
	{
		if (MyData.BaseParametersMine.pVetParameters[i].ParamValue != PARAM_MENU)
			continue;

		if (MyData.BaseParametersMine.pVetParameters[i].pVetArg[0] == number)
		{
			int nArgs = MyData.BaseParametersMine.pVetParameters[i].NArguments;
			short* pArgs = MyData.BaseParametersMine.pVetParameters[i].pVetArg;

			if (nArgs < 3) continue;

			Menu.SetupMenu(true, pArgs[1], pArgs[2], (nArgs >= 4) ? pArgs[3] : -1);
		}
	}
}


// this procedure will be called everytime a flipeffect of yours will be engaged
// you have to elaborate it and then return a TRET_.. value (most common is TRET_PERFORM_ONCE_AND_GO)
int cbFlipEffectMine(WORD FlipIndex, WORD Timer, WORD Extra, WORD ActivationMode)
{
	int RetValue;
	WORD TimerFull;

	RetValue = enumTRET.PERFORM_ONCE_AND_GO;
	// if the flip has no Extra paremeter you can handle a Timer value with values upto 32767
	// in this case you'll use the following TimerFull variable, where (with following code) we set a unique big number 
	// pasting togheter the timer+extra arguments:
	TimerFull = Timer | (Extra << 8);

	switch (FlipIndex) {
		// here type the "case Number:" for each flipeffect number. At end of the code you'll use the "break;" instruction to signal the code ending
		// Note: when you'll add your first "case Number:" then you can remove the following "case -1: and break;" instructions
	case 1:
		Ticket.SetTicket(TimerFull);
		break;

	case 2:
		Ticket.ResetTicket();
		break;

	case 3:
		SetParamSubtitles(TimerFull);
		break;

	case 4:
		MyData.electric = TimerFull * 30;
		break;
	
	case 5:
		InitMenu(TimerFull);
		break;
	
	default:
		SendToLog("WARNING: Flipeffect trigger number %d has not been handled in cbFlipEffectMine() function", FlipIndex);
		break;
	}

	// if there was the one-shot button enabled, return TRET_PERFORM_NEVER_MORE
	if (ActivationMode & enumSCANF.BUTTON_ONE_SHOT) RetValue= enumTRET.PERFORM_NEVER_MORE; 
	return RetValue;
}


// this procedure will be called everytime an action trigger of yours will be engaged 
// you have to elaborate it and then return a TRET_.. value (most common is TRET_PERFORM_ONCE_AND_GO)
int cbActionMine(WORD ActionIndex, int ItemIndex, WORD Extra, WORD ActivationMode)
{
	int RetValue;
	
	RetValue=TRET_PERFORM_ONCE_AND_GO;

	switch (ActionIndex) {
		// type here the code per your action trigger.
		// add "case Number:" and complete the code with "break;" instruction
	case -1:
		// note: remove this "case -1:" and its "break;" it has been added only to avoid warning messages about empty switch
		break;
	default:
		SendToLog("WARNING: action trigger number %d has not been handled in cbActionMine() function", ActionIndex);
		break;
	}
	
	// if there was the one-shot button enabled, return TRET_PERFORM_NEVER_MORE
	if (ActivationMode & enumSCANF.BUTTON_ONE_SHOT) RetValue= enumTRET.PERFORM_NEVER_MORE;
	return RetValue;


}

// this procedure will be called everytime a conditional trigger of yours will be engaged
// you have to elaborate it and then return a CTRET_.. value (most common is CTRET_ONLY_ONCE_ON_TRUE)
int cbConditionMine(WORD ConditionIndex, int ItemIndex, WORD Extra, WORD ActivationMode)
{
	int RetValue;
	
	RetValue=CTRET_ONLY_ONCE_ON_TRUE;

	switch (ConditionIndex){
		// type here the code for your condition trigger, inserting the code in the section
		// beginning with "case NumberOfAction:" and ending with row "break;"
	case -1:
		// note: remove this "case -1:" and its "break;" it has been added only to avoid warning messages about empty switch
		break;
	default:
		SendToLog("WARNING: condition trigger number %d has not been handled in cbConditionMine() function", ConditionIndex);
		break;


	}
	return RetValue;
	  
}

// this procedure vill be called for each Customize=CUST_... command read from script
// having one of yours CUST_ constant
// CustomizeValue will be the value of your CUST_ constant
// NumberOfItems will be the number of following Item (signed 16 bit values) following
// the CUST_ constant in the customize= script command
// pItemArray is the array with all NumberOfItems arguments of customize command
void cbCustomizeMine(WORD CustomizeValue, int NumberOfItems, short *pItemArray)
{
	// here you can replace this default management of anonymous customize commands
	// with your procedure where you can recognize each different CUST_ value and 
	// save its arguments in meaningful names fields, or elaboriting them immediatly
	// when it is possible (warning: in this moment nothing of level it has been yet loaded, excepting the script section)

	// ----- default management (optional)----
	// all customize values will be saved in MyData structure
	DWORD SizeMem;
	StrGenericCustomize *pMyCust;
	int TotCust;

	// ask memory to have another (new) record of StrGenericCustomize structure
	TotCust= MyData.BaseCustomizeMine.TotCustomize;
	TotCust++;
	SizeMem = TotCust * sizeof(StrGenericCustomize);
	MyData.BaseCustomizeMine.pVetCustomize = 
				(StrGenericCustomize *) ResizeMemory(MyData.BaseCustomizeMine.pVetCustomize, SizeMem);

	pMyCust = & MyData.BaseCustomizeMine.pVetCustomize[TotCust-1];
	
	// now require memory for all arguments (NumberOfItems) store in pItemArray

	pMyCust->pVetArg = (short *) GetMemory(2 * NumberOfItems);
	// copy data
	pMyCust->NArguments = NumberOfItems;
	memcpy(pMyCust->pVetArg, pItemArray, 2*NumberOfItems);
	pMyCust->CustValue = CustomizeValue;

	MyData.BaseCustomizeMine.TotCustomize= TotCust;
	// ---- end of default managemnt for generic customize -------------	
}
// callback called everytime in current level section of the script it has been found an AssignSlot command
// with one of your OBJ_ constants
void cbAssignSlotMine(WORD Slot, WORD ObjType)
{
	int i;

	i = MyData.BaseAssignSlotMine.TotAssign;

	if (i >= MAX_ASSIGN_SLOT_MINE) {
		SendToLog("ERROR: too many AssignSlot= commands for current plugin");
		return;
	}

	MyData.BaseAssignSlotMine.VetAssignSlot[i].MioSlot = Slot;
	MyData.BaseAssignSlotMine.VetAssignSlot[i].TipoSlot = ObjType;
	MyData.BaseAssignSlotMine.TotAssign++;

}
// this procedure vill be called for each Parameters=PARAM_... command read from script
// having one of yours PARAM_ constants
// ParameterValue will be the value of your PARAM_ constant
// NumberOfItems will be the number of following Item (signed 16 bit values) following
// the PARAM_ constant in the customize= script command
// pItemArray is the array with all NumberOfItems arguments of Parameter command
void cbParametersMine(WORD ParameterValue, int NumberOfItems, short *pItemArray)
{
	// here you can replace this default management of anonymous parameters commands
	// with your procedure where you can recognize each different Param_ value and 
	// save its arguments in meaningful names fields, or elaboriting them immediatly
	// when it is possible (warning: in this moment nothing of level it has been yet loaded, excepting the script section)

	// ----- default management (optional)----
	// all parameters values will be saved in MyData structure
	DWORD SizeMem;
	StrGenericParameters *pMyParam;
	int TotParam;

	// ask memory to have another (new) record of StrGenericparameters structure
	TotParam= MyData.BaseParametersMine.TotParameters;
	TotParam++;
	SizeMem = TotParam * sizeof(StrGenericParameters);
	MyData.BaseParametersMine.pVetParameters = 
		(StrGenericParameters *) ResizeMemory(MyData.BaseParametersMine.pVetParameters, SizeMem);

	pMyParam = & MyData.BaseParametersMine.pVetParameters[TotParam-1];
	
	// now require memory for all arguments (NumberOfItems) store in pItemArray

	pMyParam->pVetArg = (short *) GetMemory(2 * NumberOfItems);
	// copy data
	pMyParam->ParamValue = ParameterValue;
	pMyParam->NArguments = NumberOfItems;
	memcpy(pMyParam->pVetArg, pItemArray, 2*NumberOfItems);

	MyData.BaseParametersMine.TotParameters= TotParam;
	// ---- end of default managemnt for generic parameters -------------


}


/****** Bat Flock Effect ******/

void ClearBats()
{
	if (MyData.BatSlot < 0)
		return;

	memset(bats, 0, sizeof(BAT_TR3)*MAX_BATS);
}


void TriggerBats(Tr4ItemInfo *item)
{
	long lp;
	BAT_TR3 *bptr;

	short angle = item->pos.y_rot;

	bptr = &bats[0];
	for (lp=0; lp < item->trigger_flags; lp++)
	{
		bptr->pos.x_pos = item->pos.x_pos + (GetRandomControl()&511)-256;
		bptr->pos.y_pos = item->pos.y_pos + 256-(GetRandomControl()&255);
		bptr->pos.z_pos = item->pos.z_pos + (GetRandomControl()&511)-256;
		bptr->pos.y_rot = (angle + (GetRandomControl()&2047) - 1024);
		bptr->speed = (GetRandomControl()&15)+7;
		bptr->turn = ((GetRandomControl()&31)-16)*4;
		bptr->ydisp = (GetRandomControl()&31) - 24;
		bptr->on = 1;
		bptr->life = 144 + (GetRandomControl()&31);
		bptr++;
	}

	item->trigger_flags = 0;
}


void InitialiseBatEmitter(short item_number)
{
	ClearBats();
}


void ControlBatEmitter(short item_number)
{
	Tr4ItemInfo *item = &items[item_number];

	if (TriggerActive((StrItemTr4*)item))
	{
		if (item->trigger_flags == 0)
		{
			KillItem(item_number);
			return;
		}

		TriggerBats(item);
	}
}


void UpdateBat()
{
	long lp;
	BAT_TR3	*bptr;

	bptr = &bats[0];

	for (lp=0;lp<MAX_BATS;lp++)
	{
		if (bptr->on & 1)
		{
			if ((lp & 3) == 0 && (GetRandomControl()&7) == 0)
			{
				PHD_3DPOS	bpos;

				bpos.x_pos = bptr->pos.x_pos;
				bpos.y_pos = bptr->pos.y_pos;
				bpos.z_pos = bptr->pos.z_pos;
  				SoundEffect(211, &bpos, 0);

				if (!(GetRandomControl()&7))
					SoundEffect(209, &bpos, 0);
			}

			bptr->pos.x_pos -= round(bptr->speed * sin(bptr->pos.y_rot * (float)M_PI / 32768.0f));
			bptr->pos.y_pos += (GetRandomControl()&15)-8;
			bptr->pos.z_pos -= round(bptr->speed * cos(bptr->pos.y_rot * (float)M_PI / 32768.0f));
			
			if (bptr->life < 90 + (GetRandomControl()&31))	// After flown straight for a while, change direction erratically.
			{
				bptr->pos.y_pos += bptr->ydisp;
				bptr->pos.y_rot += bptr->turn;
				if ((GetRandomControl()&3) == 0)
				{
					
					bptr->speed += (GetRandomControl()&3);
				}
			}

			bptr->speed+=2;
			if (bptr->speed > 80)
				bptr->speed = 80;

			if (bptr->life)
			{
				bptr->life--;
				if (!bptr->life)
					bptr->on = 0;
			}
		}
		bptr++;
	}
}


void DrawBat()
{
	BAT_TR3* fx;
	short** meshpp;
	long 	 	lp;

	if (MyData.BatSlot < 0)
		return;

	fx = &bats[0];

	for (lp=0;lp<MAX_BATS;lp++,fx++)
	{
		if (fx->on)
		{
			auto bat = &Trng.pGlobTomb4->pAdr->pVetSlot[MyData.BatSlot];
			short** meshes = (short**)Trng.pGlobTomb4->pAdr->VetMeshPointer;
			DWORD *counter = Trng.pGlobTomb4->pAdr->pFrameCounter;
			meshpp = &meshes[bat->IndexFirstMesh + (*counter&3)*2];	// Mesh Pointers

			phd_PushMatrix();
			phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
			phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
			phd_PutPolygons_train(*meshpp, 0);
			phd_PopMatrix();
		}
	}
}


/****** Radio Static and Utility functions ******/

int SimpleDist(const StrPosizione &p1, const StrPosizione &p2)
{
	int xdiff = abs(p1.OrgX - p2.OrgX);
	int ydiff = abs(p1.OrgY - p2.OrgY);
	int zdiff = abs(p1.OrgZ - p2.OrgZ);

	int maxdist = xdiff;
	if (ydiff > maxdist)
		maxdist = ydiff;
	if (zdiff > maxdist)
		maxdist = zdiff;

	return maxdist;
}


int RealDist(const StrPosizione &p1, const StrPosizione &p2)
{
	int xdiff = abs(p1.OrgX - p2.OrgX);
	int ydiff = abs(p1.OrgY - p2.OrgY);
	int zdiff = abs(p1.OrgZ - p2.OrgZ);
	
	return round(sqrt(float(xdiff*xdiff + ydiff*ydiff + zdiff*zdiff)));
}


float InvLerp(float val1, float val2, float x, bool clamp = true)
{
	float t = (x - val1) / (val2 - val1);

	if (clamp)
	{
		if (t < 0.0f)
			t = 0.0f;
		else if (t > 1.0f)
			t = 1.0f;
	}

	return t;
}

float Lerp(float val1, float val2, float t)
{
	if (t <= 0)
		return val1;
	else if (t >= 1)
		return val2;

	return val1 + (val2 - val1) * t;
}

BYTE LerpVol(BYTE val1, BYTE val2, float t)
{
	int val = round(val1 + (val2-val1)*t);
	if (val < 0)
		val = 0;
	if (val > 255)
		val = 255;

	return val&0xFF;
}


int FindClosestEnemy()
{
	StrPosizione lp = {lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos};

	int closestDist = 0x70000000;

	for (int i = 0; i < *Trng.pGlobTomb4->pAdr->pTotItems; i++)
	{
		auto item = &items[i];
		if (!objects[item->object_number].intelligent)
			continue;

		if (!(item->active))
			continue;

		StrPosizione ip = {item->pos.x_pos, item->pos.y_pos, item->pos.z_pos};
		if (SimpleDist(lp, ip) > Radio.radius1)
			continue;

		int dist = RealDist(lp, ip);
		closestDist = (dist < closestDist) ? dist : closestDist;
	}

	return closestDist;
}


void DoStaticEffect()
{
	if (Radio.sfxSlot < 0 || !Radio.enabled)
		return; 

	Tr4ItemInfo* item = lara_item;

	int dist = FindClosestEnemy();
	if (dist > Radio.radius1)
		return;

	float vFactor = InvLerp(Radio.radius1, Radio.radius2, dist);
	
	if (vFactor > 0.0f)
	{
		Radio.countdown = 30;

		Radio.normal->volume = LerpVol(0, Radio.oldVol1, vFactor * 0.7f);
		SoundEffect(Radio.sfxSlot, &item->pos.x_pos, 2);
		if (Radio.sfxSlot != Radio.sfxSlot2)
		{
			Radio.danger->volume = LerpVol(0, Radio.oldVol2, 1.4f * vFactor-0.7f);
			SoundEffect(Radio.sfxSlot2, &item->pos.x_pos, 2);
		}
	}
	else if (Radio.countdown)
	{
		Radio.normal->volume = (Radio.normal->volume > 8) ? Radio.normal->volume - 8 : 0;
		SoundEffect(Radio.sfxSlot, &item->pos.x_pos, 2);
		if (Radio.sfxSlot != Radio.sfxSlot2)
		{
			Radio.danger->volume = (Radio.danger->volume > 8) ? Radio.danger->volume - 8 : 0;
			SoundEffect(Radio.sfxSlot2, &item->pos.x_pos, 2);
		}

		Radio.countdown--;
	}
}


/****** Subtitle Printing ******/ 

void delChar(char *s, char c)
{
	int n = strlen(s);
	int j = 0;
	for (int i = 0; i < n; i++)
	{
		if (s[i] != c)
			s[j++] = s[i];
	}
	s[j] = '\0';
}

void GetStringDimensions(char *string, short *w, short *h)
{
	char c;
	int len, y;
	short lo, hi;

	len = GetStringLength(string, &hi, &lo);
	y = (lo - hi) + 2;

	while ((c = *string++) != '\0')
	{
		if (c == '\n')
		{
			int l;

			if ((*string == '\n'))
				y += 16;

			else if (*string)
			{
				l = GetStringLength(string, &hi, &lo);

				y += (lo - hi) + 2;

				if (l > len)
					len = l;
			}
		}
	}

	*w = len;
	*h = y;
}


void SubtitleTicket::SetTicket(int stringIndex)
{
	char *txt;
	if (stringIndex < 0)
		txt = nullptr;
	else
		txt = GetString(stringIndex | STRING_NG);

	SetLines(txt);
	if (txt == nullptr)
	{
		active = false;
		life = 0;
		xPercent = 0.0f;
		yPercent = 0.0f;
		return;
	}

	active = true;
	life = MyData.subtitleLife;
	spacing = MyData.lineSpacing;
	xPercent = 0.5f - (maxwidth / 2.0f);
	yPercent = MyData.subtitleY - (newlines * MyData.lineSpacing / 2.0f);
}


void SubtitleTicket::SetLines(char *txt)
{
	char strings[8][256];
	for (int v = 0; v < 8; v++)
	{
		lines[v*2].string.clear();
		lines[v*2].width = 0.0f;
		lines[v*2].green = false;
		lines[v*2].newline = false;

		lines[v*2+1].string.clear();
		lines[v*2+1].width = 0.0f;
		lines[v*2+1].green = false;
		lines[v*2+1].newline = false;

		strcpy(strings[v], "\0");
	}
	
	size = 0;
	newlines = 0;
	maxwidth = 0.0f;
	spacing = 0.0f;
	if (txt == nullptr)
		return;

	// get string dimension for complete line
	char copy[2048];
	strcpy(copy, txt);
	delChar(copy, '_');
	short len, hig;
	GetStringDimensions(copy, &len, &hig);
	maxwidth = float(len) / Trng.pGlobTomb4->ScreenSizeX;

	// split string into lines by '\n' occurences
	strcpy(copy, txt);
	char *token = strtok(copy, "\n");
	int i = 0;
	while (token != NULL && i < 8)
	{
		strcpy(strings[i], token);
		token = strtok(NULL, "\n");
		i++;
	}

	i = 0;
	int j = 0;
	bool nextline = false;

	while (strlen(strings[i]) && i < 8)
	{
		char lncopy[256];
		bool green = false;
		strcpy(lncopy, strings[i]);
		token = strtok(lncopy, "_");
		while (token != NULL && j < 16)
		{
			// find green text formatting, get widths for substrings
			lines[j].newline = nextline;
			lines[j].green = green;
			lines[j].string.assign(token);
			lines[j].width = float(GetStringLength(token, NULL, NULL)) / Trng.pGlobTomb4->ScreenSizeX;
			
			nextline = false;
			token = strtok(NULL, "_");
			if (token)
				green = !green;
			else
			{
				if (i < 7 && strlen(strings[i+1]))
				{
					nextline = true;
					newlines++;
				}
			}

			j++;
		}	
		
		i++;
	}

	size = j;
}


void SubtitleTicket::ResetTicket()
{
	active = false;
	life = 0;
	size = 0;
	newlines = 0;
	maxwidth = 0.0f;
	spacing = 0.0f;
	xPercent = 0.0f;
	yPercent = 0.0f;
	SetLines(nullptr);
}


void DrawSubtitleText()
{

#ifdef MAGAZINES
	WORD handstatus = *Trng.pGlobTomb4->pAdr->pFlagsLaraHands;
	if (handstatus == FLH_HOLDS_ITEM)
	{
		WORD weapon = *Trng.pGlobTomb4->pAdr->pObjInLaraHandsNow;
		if (weapon == HOLD_REVOLVER)
		{
			Magazine &mag = MyData.Save.Global.revolverMag;
			char shots[8];
			char mags[8];
			itoa(mag.shotsLeft, shots, 10);
			itoa(mag.magsLeft, mags, 10);
			strcat(shots, "/");
			strcat(shots, mags);
			PrintText(0.9f * Trng.pGlobTomb4->ScreenSizeX, 0.2f * Trng.pGlobTomb4->ScreenSizeY, shots, 0, FC_WHITE, FTS_ALIGN_RIGHT);
		}
		else if (weapon == HOLD_SHOTGUN)
		{
			Magazine &mag = MyData.Save.Global.shotgunMag;
			char shots[8];
			char mags[8];
			itoa(mag.shotsLeft, shots, 10);
			itoa(mag.magsLeft, mags, 10);
			strcat(shots, "/");
			strcat(shots, mags);
			PrintText(0.9f * Trng.pGlobTomb4->ScreenSizeX, 0.2f * Trng.pGlobTomb4->ScreenSizeY, shots, 0, FC_WHITE, FTS_ALIGN_RIGHT);
		}
	}
#endif

	if (!Ticket.IsActive())
		return;

	int duration = Ticket.GetLife();
	if (!duration)
	{
		Ticket.ResetTicket();
		return;
	}
	else
	{
		int sz = Ticket.GetSize();
		float linespace = Ticket.GetSpacing();
		float xoff = 0.0f;
		float yoff = 0.0f;
		int newlines = 0;
		float xpos = Ticket.GetX();
		float ypos = Ticket.GetY();

		for (int i = 0; i < sz; i++)
		{
			if (Ticket[i].string.empty())
				break;

			if (Ticket[i].newline)
			{
				xoff = 0.0f;
				yoff += linespace;
			}

			int xPx = round((xpos + xoff) * Trng.pGlobTomb4->ScreenSizeX);
			int yPx = round((ypos + yoff) * Trng.pGlobTomb4->ScreenSizeY);

			PrintText(xPx, yPx, const_cast<char*>(Ticket[i].string.c_str()), NULL, Ticket[i].green ? FC_DARK_GRAY : FC_WHITE, FTS_ALIGN_LEFT);

			xoff += Ticket[i].width;
		}
		
		if (duration != 0xDEADDEAD)
			Ticket.DecLife();

		if (duration <= 270)
		{
			if (*Trng.pGlobTomb4->pAdr->pInputGameCommands)
				Ticket.Kill();
		}
	}


}

void SetParamSubtitles(int number)
{
	for (int i = 0; i < MyData.BaseParametersMine.TotParameters; i++)
	{
		if (MyData.BaseParametersMine.pVetParameters[i].pVetArg[0] != number)
			continue;

		if (MyData.BaseParametersMine.pVetParameters[i].ParamValue == PARAM_SUBTITLES)
		{
			int nArgs = MyData.BaseParametersMine.pVetParameters[i].NArguments;
			short *pArgs = MyData.BaseParametersMine.pVetParameters[i].pVetArg;

			if (nArgs < 2)
				return;

			if (pArgs[1] > 0)
				MyData.subtitleLife = pArgs[1];
			else if (!pArgs[1])
				MyData.subtitleLife = 0xDEADDEAD;

			if (nArgs < 3)
				return;

			if (pArgs[2] > 0)
				MyData.subtitleY = pArgs[2] / 1080.0f;

			if (MyData.subtitleY < 0.0f)
				MyData.subtitleY = 0.0f;
			else if (MyData.subtitleY > 1.0f)
				MyData.subtitleY = 1.0f;

			if (nArgs < 4)
				return;

			if (pArgs[3] > 0)
				MyData.lineSpacing = pArgs[3] / 1080.0f;

			if (MyData.lineSpacing < 0.0f)
				MyData.lineSpacing = 0.0f;
			else if (MyData.lineSpacing > 1.0f)
				MyData.lineSpacing = 1.0f;
		}
	}
}

void DrawSquare(int x1, int y1, int x2, int y2, DWORD RGBA)
{
	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;

	DXVERTEX v[4];

	v[0].tu = v[0].tv = 0.0f;
	v[1].tu = v[1].tv = 0.0f;
	v[2].tu = v[2].tv = 0.0f;
	v[3].tu = v[3].tv = 0.0f;

	DWORD col1 = RGBA;

	v[0].sx = float(0 + x1);
	v[0].sy = float(0 + y1);
	v[0].color = col1;
	v[0].specular = col1;

	v[1].sx = float(0 + x2);
	v[1].sy = float(0 + y1);
	v[1].color = col1;
	v[1].specular = col1;

	v[2].sx = float(0 + x2);
	v[2].sy = float(0 + y2);
	v[2].color = col1;
	v[2].specular = col1;

	v[3].sx = float(0 + x1);
	v[3].sy = float(0 + y2);
	v[3].color = col1;
	v[3].specular = col1;

	v[0].sz = f_mznear;
	v[0].rhw = f_moneoznear - 2.0f;

	v[1].sz = v[0].sz;
	v[1].rhw = v[0].rhw;

	v[2].sz = v[0].sz;
	v[2].rhw = v[0].rhw;

	v[3].sz = v[0].sz;
	v[3].rhw = v[0].rhw;

	TEXTURESTRUCT Tex;

	Tex.drawtype = 3;
	Tex.flag = 0;
	Tex.tpage = 0;

	(*AddQuadSorted)(v, 0, 1, 2, 3, &Tex, 0);
}


#ifdef MAGAZINES
bool RevolverFrameCheck(WORD anim, WORD frame)
{
	LARA_ARM *left = (LARA_ARM*)(0x80DF72);
	LARA_ARM *right = (LARA_ARM*)(0x80DF84);

	WORD numAnim = Trng.pGlobTomb4->pAdr->pVetSlot[SLOT_SIXSHOOTER_ANIM].IndexFirstAnim + anim;
	WORD numFrame = Trng.pGlobTomb4->pAdr->pVetAnimations[numAnim].FrameStart + frame;

	if (left->frameNum == numFrame || right->frameNum == numFrame)
		return true;

	return false;
}

void RevolverAnimSet(WORD anim, WORD frame = 0, bool force = false)
{
	LARA_ARM *left = (LARA_ARM*)(0x80DF72);
	LARA_ARM *right = (LARA_ARM*)(0x80DF84);

	WORD numAnim = Trng.pGlobTomb4->pAdr->pVetSlot[SLOT_SIXSHOOTER_ANIM].IndexFirstAnim + anim;
	if (!force)
	{
		if (left->animNum == numAnim || right->animNum == numAnim)
			return;
	}

	WORD numFrame = Trng.pGlobTomb4->pAdr->pVetAnimations[numAnim].FrameStart + frame;
	short *framePtr = (short*)Trng.pGlobTomb4->pAdr->pVetAnimations[numAnim].FrameOffset;

	left->animNum = right->animNum = numAnim;
	left->frameNum = right->frameNum = numFrame;
	left->frameBase = right->frameBase = framePtr;
}


bool ShotgunFrameCheck(WORD anim, WORD frame)
{
	int weaponIndex = *Trng.pGlobTomb4->pAdr->pWeaponHolding;
	StrItemTr4 *weapItem = &Trng.pGlobTomb4->pAdr->pVetItems[weaponIndex];

	WORD numAnim = Trng.pGlobTomb4->pAdr->pVetSlot[weapItem->SlotID].IndexFirstAnim;
	numAnim += anim;

	if (weapItem->AnimationNow == numAnim)
	{
		WORD frameNum = Trng.pGlobTomb4->pAdr->pVetAnimations[numAnim].FrameStart + frame;
		
		if (weapItem->FrameNow == frameNum)
			return true;
	}

	return false;
}


void ReloadShotgun(WORD anim)
{
	int weaponIndex = *Trng.pGlobTomb4->pAdr->pWeaponHolding;
	StrItemTr4 *weapItem = &Trng.pGlobTomb4->pAdr->pVetItems[weaponIndex];

	WORD numAnim = Trng.pGlobTomb4->pAdr->pVetSlot[weapItem->SlotID].IndexFirstAnim;
	numAnim += anim;

	if (weapItem->AnimationNow == numAnim)
		return;

	weapItem->AnimationNow = numAnim;
	weapItem->FrameNow = Trng.pGlobTomb4->pAdr->pVetAnimations[numAnim].FrameStart;
	weapItem->StateIdCurrent = Trng.pGlobTomb4->pAdr->pVetAnimations[numAnim].StateId;
}


void ControlMagazines()
{
	WORD *input = Trng.pGlobTomb4->pAdr->pInputGameCommands;

	if (*Trng.pGlobTomb4->pAdr->pFlagsLaraHands == FLH_HOLDS_ITEM)
	{
		if (*Trng.pGlobTomb4->pAdr->pObjInLaraHandsNow == HOLD_REVOLVER)
		{
			Magazine &mag = MyData.Save.Global.revolverMag;
			LARA_ARM *left = (LARA_ARM*)(0x80DF72);
			LARA_ARM *right = (LARA_ARM*)(0x80DF84);
			WORD numAnim = Trng.pGlobTomb4->pAdr->pVetSlot[SLOT_SIXSHOOTER_ANIM].IndexFirstAnim + mag.reloadAnimNum;

			int ammo = Trng.pGlobTomb4->pAdr->pInventory->AmmoRevolver;
			int magStatus = ammo % mag.shotsMax;


			if (!magStatus && (*input & CMD_ACTION) || mag.reloadNeeded)
			{
				WORD fireAnim = Trng.pGlobTomb4->pAdr->pVetSlot[SLOT_SIXSHOOTER_ANIM].IndexFirstAnim + 3;
				WORD frameNum = Trng.pGlobTomb4->pAdr->pVetAnimations[fireAnim].FrameStart;
				if (right->frameNum == frameNum && ammo)
				{
					RevolverAnimSet(mag.reloadAnimNum);
					mag.reloadNeeded = true;
					mag.reloaded = false;
				}
			}

			if (mag.reloadNeeded)
			{
				if (!mag.reloaded)
				{
					if (RevolverFrameCheck(mag.reloadAnimNum, mag.reloadFrame))
						mag.reloaded = true;
				}

				WORD reloadAnim = Trng.pGlobTomb4->pAdr->pVetSlot[SLOT_SIXSHOOTER_ANIM].IndexFirstAnim + mag.reloadAnimNum;
				WORD lastFrame = Trng.pGlobTomb4->pAdr->pVetAnimations[reloadAnim].FrameEnd - Trng.pGlobTomb4->pAdr->pVetAnimations[reloadAnim].FrameStart;

				if (RevolverFrameCheck(mag.reloadAnimNum, lastFrame))
				{
					RevolverAnimSet(0, 7, true);
					mag.reloadNeeded = false;
				}
				else
				{
					*input |= CMD_ACTION;
					*input &= ~(CMD_DRAW_WEAPON);
				}
			}

			if (magStatus)
				mag.reloaded = false;

			if (ammo && !magStatus && mag.reloaded)
				magStatus = 6;

			mag.shotsLeft = magStatus;

			if (mag.reloaded)
				ammo--;
			mag.magsLeft = ammo / mag.shotsMax;

			//mag.alreadyFired = true;
		}
		else if (*Trng.pGlobTomb4->pAdr->pObjInLaraHandsNext == HOLD_SHOTGUN)
		{
			Magazine &mag = MyData.Save.Global.shotgunMag;
			int weaponIndex = *Trng.pGlobTomb4->pAdr->pWeaponHolding;
			StrItemTr4 *weapItem = &Trng.pGlobTomb4->pAdr->pVetItems[weaponIndex];

			short ammoDived = Trng.pGlobTomb4->pAdr->pInventory->AmmoShotgunNormals / 6;
			short magStatus = ammoDived % mag.shotsMax;

			if (!magStatus && !mag.reloadNeeded && weapItem->StateIdCurrent == 2 && (*input & CMD_ACTION))
			{
				int frameNum = Trng.pGlobTomb4->pAdr->pVetAnimations[weapItem->AnimationNow].FrameStart + 1;
				if (weapItem->FrameNow == frameNum && ammoDived)
				{
					ReloadShotgun(mag.reloadAnimNum);
					mag.reloadNeeded = true;
				}
			}

			if (ShotgunFrameCheck(mag.reloadAnimNum, mag.reloadFrame))
			{
				mag.reloadNeeded = false;
			}
			
			if (ammoDived && !magStatus && !mag.reloadNeeded)
				magStatus = 6;

			mag.shotsLeft = magStatus;

			if (!mag.reloadNeeded)
				ammoDived--;
			mag.magsLeft = ammoDived / mag.shotsMax;

			//mag.alreadyFired = true;
		}
	}
}
#endif


/****** Visual Effects & Graphics ******/


// Blood Shadow
void ProjectTriPoints(PHD_VECTOR* pos, long& x, long& y, long& z)
{
	long* mptr = PHD_MXPTR;
	x = long((mptr[M00] * pos->x + mptr[M01] * pos->y + mptr[M02] * pos->z + mptr[M03]) >> 14);
	y = long((mptr[M10] * pos->x + mptr[M11] * pos->y + mptr[M12] * pos->z + mptr[M13]) >> 14);
	z = long((mptr[M20] * pos->x + mptr[M21] * pos->y + mptr[M22] * pos->z + mptr[M23]) >> 14);
}


void GridTexture(const SPRITESTRUCT& spriteinput, TEXCOORDS tex[GRID_SIZE][GRID_SIZE], int gridLen)
{
	for (int i = 0; i < gridLen; ++i)
	{
		for (int j = 0; j < gridLen; ++j)
		{
			tex[i][j].x1 = Lerp(spriteinput.x1, spriteinput.x2, i / float(gridLen));
			tex[i][j].x2 = Lerp(spriteinput.x1, spriteinput.x2, (i+1) / float(gridLen));
			tex[i][j].y1 = Lerp(spriteinput.y1, spriteinput.y2, j / float(gridLen));
			tex[i][j].y2 = Lerp(spriteinput.y1, spriteinput.y2, (j+1) / float(gridLen));
		}
	}
}


void PrintBloodShadow(short size, short* box, Tr4ItemInfo* item)
{
	long xSize, zSize, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, opt;
	long sSize = size;
	long xDist, zDist;
	long color = RGBA(0x80, 0x80, 0x80, 0xFF);

	if (item->hit_points <= 0 && !item->active && objects[item->object_number].intelligent)
	{
		// turn red and grow in size
		if (item->TOSSPAD < 90)
			item->TOSSPAD++;

		int red = round(Lerp(0x80, 0x60, item->TOSSPAD / 30.0f));
		int gb = round(Lerp(0x80, 0xFF, item->TOSSPAD / 30.0f));
		color = RGBA(red, gb, gb, 0xFF);
		sSize = round(Lerp(size, size*3/2, item->TOSSPAD / 90.0f));
	}

	xSize = sSize * (box[1] - box[0]) / 160;
	zSize = sSize * (box[5] - box[4]) / 160;
	xDist = xSize / GRID_SIZE;
	zDist = zSize / GRID_SIZE;
	xSize >>= 1;
	zSize >>= 1;

	long yPos = item->floor - 16;

	PHD_VECTOR point[GRID_SIZE+1][GRID_SIZE+1];

	for (int i = 0; i < GRID_SIZE+1; i++)
	{
		for (int j = 0; j < GRID_SIZE+1; j++)
		{
			point[i][j].x = -xSize + xDist*i;
			point[i][j].z = -zSize + zDist*j;

			float s = sin(RAD(-item->pos.y_rot));
			float c = cos(RAD(-item->pos.y_rot));

			long xPos = round(point[i][j].x * c - point[i][j].z * s);
			long zPos = round(point[i][j].z * c + point[i][j].x * s);

			xPos += item->pos.x_pos;
			zPos += item->pos.z_pos;

			short troom = item->room_number;
			void* floor = GetFloor(xPos, yPos, zPos, &troom);
			point[i][j].y = GetHeight(floor, xPos, yPos, zPos);
			if (abs(point[i][j].y - yPos) > 192)
				point[i][j].y = yPos;

			point[i][j].y -= item->floor;
		}
	}

	SPRITESTRUCT* sprite = &spriteinfo[objects[SLOT_DEFAULT_SPRITES].mesh_index + 14];
	TEXCOORDS texGrid[GRID_SIZE][GRID_SIZE];
	GridTexture(*sprite, texGrid, GRID_SIZE);

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x_pos, yPos, item->pos.z_pos);
	phd_RotY(item->pos.y_rot);

	opt = nPolyType;
	nPolyType = 6;

	TEXTURESTRUCT Tex;
	DXVERTEX v[4];

	for (int i = 0; i < GRID_SIZE; ++i)
	{
		for (int j = 0; j < GRID_SIZE; ++j)
		{
			ProjectTriPoints(&point[i][j], x1, y1, z1);

			ProjectTriPoints(&point[i][j+1], x2, y2, z2);

			ProjectTriPoints(&point[i+1][j+1], x3, y3, z3);

			ProjectTriPoints(&point[i+1][j], x4, y4, z4);

			setXYZ4(v, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, clipflags);

			v[0].color = color;
			v[0].specular = 0xFF000000;

			v[1].color = color;
			v[1].specular = 0xFF000000;

			v[2].color = color;
			v[2].specular = 0xFF000000;

			v[3].color = color;
			v[3].specular = 0xFF000000;

			Tex.drawtype = 5;
			Tex.flag = 0;
			Tex.tpage = sprite->tpage;
			Tex.u1 = texGrid[i][j].x1;
			Tex.v1 = texGrid[i][j].y1;
			Tex.u2 = texGrid[i][j].x1;
			Tex.v2 = texGrid[i][j].y2;
			Tex.u3 = texGrid[i][j].x2;
			Tex.v3 = texGrid[i][j].y2;
			Tex.u4 = texGrid[i][j].x2;
			Tex.v4 = texGrid[i][j].y1;
			
			(*AddQuadSorted)(v, 0, 1, 2, 3, &Tex, 0);
		}
	}

	nPolyType = opt;

	phd_PopMatrix();
}

void TriggerTorchFlame(short item_number, long node, long xvel, long yvel, long zvel)
{
	SPARKS* spark = *(SPARKS**)0x43383F;
	SPARKS* sptr = &spark[GetFreeSpark()];

	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x31) + 160;
	sptr->sB = (GetRandomControl() & 0x31) + 160;
	sptr->dR = (GetRandomControl() & 0x1F) + 224;
	sptr->dG = (GetRandomControl() & 0x1F) + 16;
	sptr->dB = 0;
	sptr->FadeToBlack = 5;
	sptr->ColFadeSpeed = 5;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 9;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128 + xvel;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF) + yvel;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128 + zvel;
	sptr->Friction = 5;
	sptr->Flags = 4762;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
	sptr->NodeNumber = (BYTE)node;
	sptr->MaxYvel = 0;
	sptr->FxObj = (BYTE)item_number;
	sptr->Scalar = 1;
	sptr->Size = (GetRandomControl() & 0xF) + 60;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size >> 2;
}


void TriggerTorchGlow(short item_number, long node)
{
	SPARKS* spark = *(SPARKS**)0x43383F;
	SPARKS* sptr = &spark[GetFreeSpark()];

	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 200;
	sptr->sG = (GetRandomControl() & 0x1F) + 32;
	sptr->sB = 0;
	sptr->dR = (GetRandomControl() & 0x1F) + 200;
	sptr->dG = (GetRandomControl() & 0x1F) + 16;
	sptr->dB = 32;
	sptr->FadeToBlack = 0;
	sptr->ColFadeSpeed = 0;
	sptr->TransType = 2;
	sptr->Life = 2;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Friction = 5;
	sptr->Flags = 4762;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->Def = objects[SLOT_DEFAULT_SPRITES].mesh_index + 11;

	sptr->Gravity = 0;
	sptr->NodeNumber = (BYTE)node;
	sptr->MaxYvel = 0;
	sptr->FxObj = (BYTE)item_number;
	sptr->Scalar = 1;
	sptr->Size = (GetRandomControl() & 0x1F) + 200;
	sptr->sSize = sptr->Size;
	sptr->dSize = sptr->Size;
}


void DoFlareSparks(long x, long y, long z, long xvel, long yvel, long zvel)
{
	SPARKS* spark = *(SPARKS**)0x43383F;
	SPARKS* sptr = &spark[GetFreeSpark()];

	long dx, dz, rnd;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	rnd = GetRandomControl();
	sptr->On = 1;
	sptr->dR = -1;
	sptr->dG = (rnd & 0x7F) + 64;
	sptr->dB = -64 - sptr->dG;
	sptr->sR = 0x40;
	sptr->sG = 0x40;
	sptr->sB = 0x40;
	sptr->Life = 10;
	sptr->sLife = 10;
	sptr->ColFadeSpeed = 3;
	sptr->FadeToBlack = 5;
	sptr->x = (rnd & 7) + x - 3;
	sptr->y = ((rnd >> 3) & 7) + y - 3;
	sptr->z = ((rnd >> 6) & 7) + z - 3;

	short randX = (((rnd >> 2) & 0xFF) - 128) * 3 / 2;
	short randY = (((rnd >> 4) & 0xFF) - 128) * 3 / 2;
	short randZ = (((rnd >> 6) & 0xFF) - 128) * 3 / 2;
	sptr->Xvel = short(randX + xvel);
	sptr->Yvel = short(randY + yvel);
	sptr->Zvel = short(randZ + zvel);
	sptr->TransType = 2;
	sptr->Friction = 64;
	sptr->Scalar = 1;
	sptr->Flags = 2;
	sptr->sSize = ((rnd >> 9) & 3) + 4;
	sptr->Size = sptr->sSize;
	sptr->dSize = ((rnd >> 12) & 1) + 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = 64;
}


void DoFlareSmoke(long x, long y, long z, long xvel, long yvel, long zvel)
{
	SPARKS* spark = *(SPARKS**)0x43383F;
	SPARKS* sptr = &spark[GetFreeSpark()];

	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;

	sptr->dR = 96 + (GetRandomControl() & 0x1F);
	sptr->dG = 80;
	sptr->dB = 80;

	sptr->x = x;
	sptr->y = y;
	sptr->z = z;

	sptr->ColFadeSpeed = 12 + (GetRandomControl() & 3);
	sptr->FadeToBlack = 24;
	sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 32;

	sptr->TransType = 2;

	sptr->Xvel = ((GetRandomControl() & 127) - 64) + xvel;
	sptr->Zvel = ((GetRandomControl() & 127) - 64) + zvel;
	sptr->Friction = 6;

	sptr->Yvel = -(GetRandomControl() & 15) - 24 + yvel;

	sptr->Flags = 2 | 8 | 16 | 512;

	sptr->RotAng = GetRandomControl() & 4095;
	if (GetRandomControl() & 1)
		sptr->RotAdd = -(GetRandomControl() & 7) - 8;
	else
		sptr->RotAdd = (GetRandomControl() & 7) + 8;

	sptr->Scalar = 2;
	sptr->Gravity = -(GetRandomControl() & 15) - 16;
	sptr->MaxYvel = -(GetRandomControl() & 7) - 8;
	BYTE size = (GetRandomControl() & 15) + 64;
	sptr->Size = sptr->sSize = size >> 2;
	sptr->dSize = size;
}


void DoLaraTorch()
{
	StrPosizione pos = { -12, 40, 144 };
	GetLaraJointPos(&pos, 14);
	TriggerDynamic(pos.OrgX, pos.OrgY, pos.OrgZ, 12 - (GetRandomControl() & 1),
		(GetRandomControl() & 0x1F) + 224, (GetRandomControl() & 0xF) + 32, (GetRandomControl() & 0x1F));
	SoundEffect(150, &pos, 0);

	StrPosizione pos2 = { -32, 80, 400 };
	GetLaraJointPos(&pos2, 14);

	int xVel = (pos2.OrgX - pos.OrgX);
	int yVel = (pos2.OrgY - pos.OrgY);
	int zVel = (pos2.OrgZ - pos.OrgZ);

	DoFlareSmoke(pos.OrgX, pos.OrgY, pos.OrgZ, xVel, yVel, zVel);
	DoFlareSparks(pos.OrgX, pos.OrgY, pos.OrgZ, xVel * 3, yVel * 3, zVel * 3);
	DoFlareSparks(pos.OrgX, pos.OrgY, pos.OrgZ, xVel * 3, yVel * 3, zVel * 3);
	DoFlareSparks(pos.OrgX, pos.OrgY, pos.OrgZ, xVel * 3, yVel * 3, zVel * 3);

	TriggerTorchFlame(*Trng.pGlobTomb4->pAdr->pLaraIndex, 0, xVel * 2, yVel * 2, zVel * 2);
	TriggerTorchGlow(*Trng.pGlobTomb4->pAdr->pLaraIndex, 0);
	TriggerTorchGlow(*Trng.pGlobTomb4->pAdr->pLaraIndex, 0);
}


void DoFreeTorch(short item_num)
{
	auto item = &items[item_num];

	TriggerDynamic(item->pos.x_pos, item->pos.y_pos - 64, item->pos.z_pos, 12 - (GetRandomControl() & 1),
		(GetRandomControl() & 0x1F) + 224, (GetRandomControl() & 0xF) + 32, (GetRandomControl() & 0x1F));
	SoundEffect(150, &item->pos, 0);

	StrMovePosition pos = { 0, 0, 208 };
	GetJointAbsPosition((StrItemTr4*)item, &pos, 0);
	StrMovePosition pos2 = { 0, 0, 464 };
	GetJointAbsPosition((StrItemTr4*)item, &pos2, 0);

	int xVel = (pos2.RelX - pos.RelX);
	int yVel = (pos2.RelY - pos.RelY);
	int zVel = (pos2.RelZ - pos.RelZ);

	DoFlareSmoke(pos.RelX, pos.RelY, pos.RelZ, xVel, yVel, zVel);
	DoFlareSparks(pos.RelX, pos.RelY, pos.RelZ, xVel * 3, yVel * 3, zVel * 3);
	DoFlareSparks(pos.RelX, pos.RelY, pos.RelZ, xVel * 3, yVel * 3, zVel * 3);
	DoFlareSparks(pos.RelX, pos.RelY, pos.RelZ, xVel * 3, yVel * 3, zVel * 3);

	TriggerTorchFlame(item_num, 1, xVel * 2, yVel * 2, zVel * 2);
	TriggerTorchGlow(item_num, 1);
	TriggerTorchGlow(item_num, 1);
}


void TriggerLaraSparks(long smoke)
{
	SPARKS* spark = *(SPARKS**)0x43383F; // get remapped SPARKS array address from GetFreeSparks()
	SPARKS* sptr;
	StrPosizione pos;
	pos.OrgX = pos.OrgY = pos.OrgZ = 0;
	GetLaraJointPos(&pos, GetRandomControl() % 15);
	int dx = GetRandomControl();
	sptr = &spark[GetFreeSpark()];

	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 63) + 192;
	sptr->sG = sptr->sR;
	sptr->sB = sptr->sR;

	sptr->dR = 0;
	sptr->dG = sptr->sR >> 1;
	sptr->dB = sptr->sR;

	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 4;
	sptr->sLife = sptr->Life = 12;
	sptr->TransType = 2;

	sptr->x = pos.OrgX;
	sptr->y = pos.OrgY;
	sptr->z = pos.OrgZ;
	sptr->Xvel = ((GetRandomControl() & 511) - 256) << 1;
	sptr->Yvel = ((GetRandomControl() & 511) - 256) << 1;
	sptr->Zvel = ((GetRandomControl() & 511) - 256) << 1;
	sptr->Friction = 3 | (3 << 4);
	sptr->Scalar = 1;
	sptr->Gravity = sptr->MaxYvel = 0;
	sptr->Flags = 0;

	if (smoke)
	{
		sptr = &spark[GetFreeSpark()];

		sptr->On = 1;
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;

		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;

		sptr->ColFadeSpeed = 8 + (GetRandomControl() & 3);
		sptr->FadeToBlack = 16;
		sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 28;

		sptr->TransType = 2;

		sptr->x = pos.OrgX;
		sptr->y = pos.OrgY;
		sptr->z = pos.OrgZ;

		sptr->Xvel = ((GetRandomControl() & 255) - 128);
		sptr->Zvel = ((GetRandomControl() & 255) - 128);
		sptr->Friction = 3;

		sptr->Yvel = -(GetRandomControl() & 15) - 16;

		sptr->Flags = 2 | 8 | 16 | 512;

		sptr->RotAng = GetRandomControl() & 4095;
		if (GetRandomControl() & 1)
			sptr->RotAdd = -(GetRandomControl() & 7) - 8;
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 8;

		sptr->Scalar = 2;
		sptr->Gravity = -(GetRandomControl() & 15) - 8;
		sptr->MaxYvel = -(GetRandomControl() & 7) - 8;
		BYTE size = (GetRandomControl() & 31) + 128;
		sptr->Size = sptr->sSize = size >> 2;
		sptr->dSize = size;
	}
}


void ElectricLight(short index)
{
	long wibble = *(long*)(0x4BF238);
	auto item = &items[index];

	if (TriggerActive((StrItemTr4*)item))
	{
		short shade = 0;
		short ocb = item->trigger_flags;

		if (ocb > 0)	// Normal electric light.
		{
			if (item->item_flags[0] < 16)
			{
				shade = (GetRandomControl() & 63) << 2;
				item->item_flags[0]++;
			}
			else if (item->item_flags[0] < 96)
			{
				if ((wibble & 63) == 0 || (GetRandomControl() & 7) == 0)
				{
					shade = 192 - (GetRandomControl() & 63);
				}
				else
				{
					shade = GetRandomControl() & 63;
				}
				item->item_flags[0]++;
			}
			else if (item->item_flags[0]< 160)
			{
				shade = 96 - (GetRandomControl() & 31);

				if ((GetRandomControl() & 31) == 0 && item->item_flags[0] > 128)
					item->item_flags[0] = 160;
				else
					item->item_flags[0]++;
			}
			else
			{
				shade = 255 - (GetRandomControl() & 31);
			}
		}
		else	// Broken electric light.
		{
			if (item->item_flags[0] <= 0)
			{
				item->item_flags[0] = (GetRandomControl() & 3) + 4;
				item->item_flags[1] = (GetRandomControl() & 127) + 128;
				item->item_flags[2] = GetRandomControl() & 1;
			}

			item->item_flags[0]--;

			if (item->item_flags[2])
				return;

			item->item_flags[0]--;

			shade = item->item_flags[0] - (GetRandomControl() & 127);
			if (shade > 64)
				SoundEffect(364, &item->pos, 8 | ((shade >> 3) << 8));
		}

		BYTE r, g, b;
		r = (ocb & 31) << 3;
		g = ((ocb >> 5) & 31) << 3;
		b = ((ocb >> 10) & 31) << 3;
		r = (r * shade) >> 8;
		g = (g * shade) >> 8;
		b = (b * shade) >> 8;

		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 24, r, g, b);
	}
	else
		item->item_flags[0] = 0;
}


void DrawElectricSparks()
{
	if (MyData.electric > -1)
	{
		if (MyData.electric > 1)
			MyData.electric--;
		else if (MyData.electric == 1)
			MyData.electric = -1;

		TriggerLaraSparks(0);
		TriggerLaraSparks(0);
		TriggerLaraSparks(1);

		StrPosizione pos = {0, 0, 0};
		GetLaraJointPos(&pos, 0);
		TriggerDynamic(pos.OrgX,pos.OrgY,pos.OrgZ,8,0,GetRandomControl()&127,(GetRandomControl()&63)+128);
	}
}


#if 0
void DrawVignette()
{
	if (MyData.VignetteSprite < 0)
		return;

	clipflags[0] = 0;
	clipflags[1] = 0;
	clipflags[2] = 0;
	clipflags[3] = 0;

	DXVERTEX v[4];

	v[0].tu = v[0].tv = 0.0f;
	v[1].tu = v[1].tv = 0.0f;
	v[2].tu = v[2].tv = 0.0f;
	v[3].tu = v[3].tv = 0.0f;

	BYTE alpha = 0x6;
	DWORD col1 = RGBA(alpha, alpha, alpha, 0xFF);

	v[0].sx    = float(phd_winxmin);
	v[0].sy    = float(phd_winymin);
	v[0].color = col1;
	v[0].specular = col1;

	v[1].sx    = float(phd_winxmax+1);
	v[1].sy    = float(phd_winymin);
	v[1].color = col1;
	v[1].specular = col1;

	v[2].sx    = float(phd_winxmax+1);
	v[2].sy    = float(phd_winymax+1);
	v[2].color = col1;
	v[2].specular = col1;

	v[3].sx    = float(phd_winxmin);
	v[3].sy    = float(phd_winymax+1);
	v[3].color = col1;
	v[3].specular = col1;

	v[0].sz    = f_mznear-8.0f;
	v[0].rhw   = f_moneoznear-6.0f;

	v[1].sz    = v[0].sz;
	v[1].rhw   = v[0].rhw;
	
	v[2].sz    = v[0].sz;
	v[2].rhw   = v[0].rhw;
	
	v[3].sz    = v[0].sz;
	v[3].rhw   = v[0].rhw;

	int framemod = ((*Trng.pGlobTomb4->pAdr->pFrameCounter) / 2) % 15;

	SPRITESTRUCT* sprite = (spriteinfo + objects[SLOT_DEFAULT_SPRITES].mesh_index + MyData.VignetteSprite + framemod);

	TEXTURESTRUCT tex;
	tex.drawtype = 2;

	tex.tpage = sprite->tpage;
	tex.u1 = sprite->x1;
	tex.v1 = sprite->y1;
	tex.u2 = sprite->x2;
	tex.v2 = sprite->y1;
	tex.u3 = sprite->x2;
	tex.v3 = sprite->y2;
	tex.u4 = sprite->x1;
	tex.v4 = sprite->y2;

	(*AddQuadSorted)(v,0,1,2,3,&tex,0);
}
#endif


void DrawVfx()
{
	DrawSubtitleText();
	//DrawVignette();
}


/****** Other Callbacks ******/


int cbCycleEnd()
{
	return RET_CYCLE_CONTINUE;
}

// this procedure will be called every game cycle (at begin of cycle)
void cbCycleBegin(void)
{
	Menu.MenuControl();

	DoStaticEffect();

#ifdef STAMINA
	if (MyData.Save.Local.runCounter < STAMINA_MAX)
	{
		if (lara->StateIdCurrent != 1)
		{
			
			MyData.Save.Local.runCounter += 2;
			if (lara->StateIdCurrent == 2)
				MyData.Save.Local.runCounter += 2;

			if (MyData.Save.Local.runCounter > STAMINA_MAX)
				MyData.Save.Local.runCounter = STAMINA_MAX;
		}
	}

	if (MyData.Save.Local.runCounter >= STAMINA_THRESHOLD)
		MyData.Save.Local.tired = false;
#endif

#ifdef MAGAZINES
	ControlMagazines();
#endif
}

// this function will be called for each your (common) progressive action to be peformed
void PerformMyProgrAction(StrProgressiveAction *pAction)
{


	switch (pAction->ActionType) {
// replace the "case -1:" with your first "case AXN_...:" progressive action to manage)		
	case -1:
		break;

	}

}
// callback called from trng for each frame in game cycle to perform your (common) progressive action
void cbProgrActionMine(void)
{
	int i;
	StrProgressiveAction *pAction;

	pAction = &MyData.VetProgrActions[0];
	for (i=0;i<MyData.TotProgrActions;i++) {
		if (pAction->ActionType != AXN_FREE) {
			PerformMyProgrAction(pAction);
		}
		pAction++;
	}
}

// Not yet linked! To link it add to RequireMyCallBacks() function the row:
//  	GET_CALLBACK(CB_CYCLE_END, 0, 0, cbCycleEnd);
// this procedure will be called everygame cycle, at end.
// you have to return a RET_CYCLE_ value
#ifdef MAGAZINES
int cbCycleEnd(void)
{
	if (MyData.Save.Global.shotgunMag.reloadNeeded)
	{
		int weaponIndex = *Trng.pGlobTomb4->pAdr->pWeaponHolding;
		StrItemTr4 *weapItem = &Trng.pGlobTomb4->pAdr->pVetItems[weaponIndex];
		weapItem->StateIdNext = 5;
	}
	return RET_CYCLE_CONTINUE;	
}
#endif

// inside this function you'll type call to functions to intialise your new objects or customize that olds.
// this callback will be called at start of loading new level and a bit after having started to load level data
void cbInitObjects(void) 
{
	auto electric = &Trng.pGlobTomb4->pAdr->pVetSlot[SLOT_BLINKING_LIGHT];
	electric->pProcDraw = NULL;
	electric->pProcControl = ElectricLight;

	next_bat = 0;

	MyData.BatSlot = -1;

	for (int i = 0; i < MyData.BaseAssignSlotMine.TotAssign; ++i)
	{
		if (MyData.BaseAssignSlotMine.VetAssignSlot[i].TipoSlot == OBJ_BAT)
		{
			MyData.BatSlot = MyData.BaseAssignSlotMine.VetAssignSlot[i].MioSlot;
		}
	}

	if (MyData.BatSlot > 1)
	{
		auto BatEmitter = &Trng.pGlobTomb4->pAdr->pVetSlot[MyData.BatSlot];

		if (!BatEmitter->Flags & 0x1)
			return;

		BatEmitter->pProcInitialise = InitialiseBatEmitter;
		BatEmitter->pProcControl = ControlBatEmitter;
		BatEmitter->pProcDraw = NULL;
		BatEmitter->pProcCollision = NULL;
		BatEmitter->pProcFloor = NULL;
		BatEmitter->pProcCeiling = NULL;
		BatEmitter->Flags |= 0x20;
	}
}


void MenuYesNo::SetupMenu(bool isActive, int stringNG, int tgOnYes, int tgOnNo)
{
	active = isActive;
	questionStringNG = stringNG;
	tgYes = tgOnYes;
	tgNo = tgOnNo;
}


void MenuYesNo::MenuControl()
{
	if (active)
	{
		bool selection = true;
		do
		{
			S_InitialisePolyList();

			//UpdatePulseColour();

			PrintString(phd_centerx, (phd_winymax * 3 / 4), 2, GetString(questionStringNG | STRING_NG), 0x8000);
			//PrintString(phd_centerx, (phd_winymax * 7 / 8), 2, "\x19\x1B", 0x8000);
			PrintString((phd_winxmax * 7 / 16), (phd_winymax * 7 / 8), selection ? 3 : 2, "Yes", 0x8000);
			PrintString((phd_winxmax * 9 / 16), (phd_winymax * 7 / 8), selection ? 2 : 3, "No", 0x8000);

			SetDebounce = 1;
			S_UpdateInput();

			if ((selection && (dbinput & CMD_RIGHT)) || (!selection && (dbinput & CMD_LEFT)))
			{
				SoundEffect(111, NULL, 2);
				selection = !selection;
			}

			if ((dbinput & CMD_ACTION) || (dbinput & 0x100000))
			{
				SoundEffect(109, NULL, 2);

				if (selection && tgYes > 0)
					PerformTriggerGroup(tgYes);
				if (!selection && tgNo > 0)
					PerformTriggerGroup(tgNo);

				active = false;
				break;
			}

			DrawPhaseGame();

		} while (!thread_ended);
	}
}


// FOR_YOU:
// in this function RequireMyCallBacks() you'll type
// a list of:
//		GET_CALLBACK(CB_..., ,)
// one for each callback you need
bool RequireMyCallBacks(void)
{
// ************  RequireMyCallBacks() function  *****************
	// protype of GET_CALLBACK:
	// GET_CALLBACK(CallBackCB, CBT_Flags, Index, MyProcToCall)
	// default callbacks required always 
	GET_CALLBACK(CB_INIT_PROGRAM, 0, 0, cbInitProgram)
	GET_CALLBACK(CB_INIT_GAME, 0, 0, cbInitGame)
	GET_CALLBACK(CB_INIT_LEVEL, 0,0, cbInitLevel)
	GET_CALLBACK(CB_SAVING_GAME, 0, 0, cbSaveMyData)
	GET_CALLBACK(CB_LOADING_GAME, 0, 0, cbLoadMyData)
	GET_CALLBACK(CB_INIT_LOAD_NEW_LEVEL, 0,0, cbInitLoadNewLevel);
	GET_CALLBACK(CB_FLIPEFFECT_MINE, 0, 0, cbFlipEffectMine);
	//GET_CALLBACK(CB_ACTION_MINE, 0,0, cbActionMine);
	//GET_CALLBACK(CB_CONDITION_MINE,0,0,cbConditionMine);
	GET_CALLBACK(CB_CUSTOMIZE_MINE, 0,0, cbCustomizeMine);
	GET_CALLBACK(CB_PARAMETER_MINE, 0, 0, cbParametersMine);
	GET_CALLBACK(CB_ASSIGN_SLOT_MINE, 0,0, cbAssignSlotMine);
	GET_CALLBACK(CB_CYCLE_BEGIN, 0, 0, cbCycleBegin);
	//GET_CALLBACK(CB_PROGR_ACTION_MINE, 0, 0, cbProgrActionMine);
	GET_CALLBACK(CB_INIT_OBJECTS, 0, 0, cbInitObjects);
	GET_CALLBACK(CB_LARA_DRAW, CBT_AFTER, 0, DrawElectricSparks);

#ifdef MAGAZINES
	GET_CALLBACK(CB_ANIMATE_LARA, CBT_AFTER, 0, ControlMagazines);
	GET_CALLBACK(CB_CYCLE_END, 0, 0, cbCycleEnd);
#endif

	return true;
}
// FOR_YOU:
// This function will be the first code to be executed of your plugin
// It happens when trng will load your plugin.
// In this moment no directX graphic is yet active, so you can show
// message boxes (TryMessageBox) to advise your players or level designer 
// about something
// Note: if you wish abort all (because there is an error or something
// is missing) you have to exit from this function returning: false
bool InitializeAll(void)
{
// ************  InitializeAll() function  ****************
	//  perform all your patches
	CALL_CHECK(CreateMyCodePatches)

	// call the function that requires all callback you need
	CALL_CHECK(RequireMyCallBacks)

	// TYPE_HERE: code to allocate global resource to use in the whole game

	return true;
}

// FOR_YOU: Tyis function will be called when tomb4 game is to be closed.
// you should type in this function further codes to free the global
// resource you had allocated in the InitializeAll() function 
void ReleaseAll(void)
{
// ************  ReleaseAll() function  ******************
	FreeLevelResources();
}


BOOL APIENTRY DllMain( HINSTANCE hInstanceDll, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			MyDllInstance = hInstanceDll;
			GetTrngInfo();
			// control per check control value about size and alignment with globtomb4 structure
			if (CheckControlGlobTomb4() == false) return FALSE;

			if  (InitializeAll()==false) {
				return FALSE;
			}
			return TRUE;
			

		case DLL_PROCESS_DETACH:
			ReleaseAll();
			break;
    }
    return TRUE;
}


