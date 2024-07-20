// let below pragma directive at top of this source
#pragma pack(1)
#include <string>

// TYPE_HERE: here you can type your structure definitions like it has been done
// in the "structures.h" file for trng dll
typedef struct
{
	union {
	float sx;
	float dvSX;
	};

	union {
	float sy;
	float dvSY;
	};

	union {
	float sz;
	float dvSZ;
	};

	union {
	float rhw;
	float dvRHW;
	};

	union {
	DWORD color;
	DWORD dcColor;
	};

	union {
	DWORD specular;
	DWORD dcSpecular;
	};

	union {
	float tu;
	float dvTU;
	};

	union {
	float tv;
	float dvTV;
	};

}DXVERTEX;


typedef struct
{
	WORD drawtype;
	WORD tpage;
	WORD flag;
	float u1;
	float v1;
	float u2;
	float v2;
	float u3;
	float v3;
	float u4;
	float v4;
}TEXTURESTRUCT;


typedef struct
{
	float x1, y1, x2, y2;
}TEXCOORDS;


typedef struct
{
	WORD tpage;
	WORD offset;
	WORD width;
	WORD height;
	float x1;	//left
	float y1;	//top
	float x2;	//right
	float y2;	//bottom
}SPRITESTRUCT;


struct ColorRGB
{
	BYTE Blue;
	BYTE Green;
	BYTE Red;
	BYTE Alpha; // ? not sure
};


struct Tr4SampleInfo
{
	short number;
	BYTE volume;
	char radius;
	char chance;
	char pitch;
	WORD flags;
};

typedef struct
{
	short* frame_ptr;
	BYTE frame_rate;
	BYTE frame_size;

	short current_anim_state;

	int velocity;
	int acceleration;
	int Xvelocity;
	int Xacceleration;

	short frame_base;
	short frame_end;
	short jump_anim_num;
	short jump_frame_num;
	short number_changes;
	short change_index;
	short number_commands;
	short command_index;

}Tr4AnimStruct;

typedef struct
{
	short nmeshes;
	short mesh_index;
	long bone_index;
	short* frame_base;
	void (*initialise)(short item_number);
	void (*control)(short item_number);
	void (*floor)(void* item, long x, long y, long z, long* height);
	void (*ceiling)(void* item, long x, long y, long z, long* height);
	void (*draw_routine)(void* item);
	void (*collision)(short item_num, void* laraitem, void* coll);
	short object_mip;
	short anim_index;
	short hit_points;
	short pivot_length;
	short radius;
	short shadow_size;
	WORD bite_offset;
	WORD loaded : 1;
	WORD intelligent : 1;
	WORD non_lot : 1;
	WORD save_position : 1;
	WORD save_hitpoints : 1;
	WORD save_flags : 1;
	WORD save_anim : 1;
	WORD semi_transparent : 1;
	WORD water_creature : 1;
	WORD using_drawanimating_item : 1;
	WORD HitEffect : 2;
	WORD undead : 1;
	WORD save_mesh : 1;
	void (*draw_routine_extra)(void* item);
	DWORD explodable_meshbits;
	DWORD padfuck;
}Tr4ObjectInfo;


struct RadioNoise
{
	bool enabled; // is radio effect enabled in level?
	short sfxSlot; // 'normal' sound slot
	short sfxSlot2; // 'danger' sound slot (closer proximity)
	int radius1; // first proximity
	int radius2; // second proximity
	Tr4SampleInfo *normal;
	Tr4SampleInfo *danger;
	BYTE oldVol1; //'normal' max vol (lerped)
	BYTE oldVol2; // 'danger' max vol (lerped)
	BYTE countdown; // for smooth fadeout when enemies are killed
};


struct Magazine
{
	short magsLeft;
	short shotsMax;
	short shotsLeft;
	WORD reloadAnimNum;
	WORD reloadFrame;
	bool reloadNeeded;
	bool reloaded;
};


struct TextLine
{
	bool newline; // rendered on new line?
	bool green; // render in green? (false = white)
	float width;
	std::string string;
};


class SubtitleTicket
{
public:

	void DecLife() {life--;}
	int GetLife() {return life;}
	void Kill() {life = 0;}
	bool IsActive() {return active;}
	float GetX() {return xPercent;}
	float GetY() {return yPercent;}
	float GetSpacing() {return spacing;}
	int GetSize() {return size;}
	int GetNewlines() {return newlines;}
	const TextLine& operator[](int i) {return lines[i];}

	void SetLines(char *txt);
	void SetTicket(int stringIndex);
	void ResetTicket();

private:

	bool active;
	int life;
	int size;
	int newlines;
	float maxwidth;
	float spacing;
	float xPercent;
	float yPercent;
	TextLine lines[16];
};


struct MenuYesNo
{
	void SetupMenu(bool isActive, int stringNG, int tgOnYes, int tgOnNo);
	void MenuControl();

private:
	int questionStringNG;
	int tgYes;
	int tgNo;
	bool active;
};


typedef struct
{
	short *frameBase;		//00
	short frameNum;			//04
	short animNum;			//06
	short lock;				//08
	short yRot, xRot, zRot;	//0A
	short flashGun;			//10
}LARA_ARM; //12


typedef struct
{
	long		x;				//00
	long		y;				//04
	long		z;				//08

	short		Xvel;			//0C
	short		Yvel;			//0E
	short		Zvel;			//10
	short		Gravity;		//12
	short		RotAng;			//14
	short		Flags;			//16

	unsigned char	sSize;		//18
	unsigned char	dSize;		//19
	unsigned char	Size;		//1A
	unsigned char	Friction; 	//1B

	unsigned char	Scalar;		//1C
	unsigned char	Def;		//1D
	signed char		RotAdd;		//1E
	signed char		MaxYvel;	//1F

	unsigned char	On;			//20
	unsigned char	sR;			//21
	unsigned char	sG;  		//22
	unsigned char	sB;  		//23

	unsigned char	dR;			//24
	unsigned char	dG;  		//25
	unsigned char	dB;  		//26
	unsigned char	R;			//27

	unsigned char	G;  			//28
	unsigned char	B;  			//29
	unsigned char	ColFadeSpeed;	//2A
	unsigned char	FadeToBlack;	//2B

	unsigned char	sLife;			//2C
	unsigned char 	Life;			//2D
	unsigned char	TransType;		//2E
	unsigned char	extras;			//2F

	signed char		Dynamic;		//30
	unsigned char	FxObj;			//31
	unsigned char	RoomNumber; 	//32
	unsigned char	NodeNumber;		//33

} SPARKS;	//34


typedef struct {
	long x, y, z;
} PHD_VECTOR;


struct Tr4CollInfo
{
	long mid_floor;
	long mid_ceiling;
	long mid_type;
	long front_floor;
	long front_ceiling;
	long front_type;
	long left_floor;
	long left_ceiling;
	long left_type;
	long right_floor;
	long right_ceiling;
	long right_type;
	long left_floor2;
	long left_ceiling2;
	long left_type2;
	long right_floor2;
	long right_ceiling2;
	long right_type2;
	long radius;
	long bad_pos;
	long bad_neg;
	long bad_ceiling;
	PHD_VECTOR shift;
	PHD_VECTOR old;
	short old_anim_state;
	short old_anim_number;
	short old_frame_number;
	short facing;
	short quadrant;
	short coll_type;
	short* trigger;
	char tilt_x;
	char tilt_z;
	char hit_by_baddie;
	char hit_static;
	WORD slopes_are_walls : 2;
	WORD slopes_are_pits : 1;
	WORD lava_is_pit : 1;
	WORD enable_baddie_push : 1;
	WORD enable_spaz : 1;
	WORD hit_ceiling : 1;
};


typedef struct {
	long x_pos;
	long y_pos;
	long z_pos;
	short x_rot;
	short y_rot;
	short z_rot;
} PHD_3DPOS;


struct Tr4ItemInfo
{
	int floor;
	DWORD touch_bits;
	DWORD mesh_bits;
	short object_number;
	short current_anim_state;
	short goal_anim_state;
	short required_anim_state;
	short anim_number;
	short frame_number;
	short room_number;
	short next_item;
	short next_active;
	short speed;
	short fallspeed;
	short hit_points;
	WORD box_number;
	short timer;
	short flags;
	short shade;
	short trigger_flags;
	short carried_item;
	short after_death;
	WORD fired_weapon;
	short item_flags[4];
	void* data;
	PHD_3DPOS pos;
	BYTE lightdata[5528];
	DWORD active : 1;
	DWORD status : 2;
	DWORD gravity_status : 1;
	DWORD hit_status : 1;
	DWORD collidable : 1;
	DWORD looked_at : 1;
	DWORD dynamic_light : 1;
	DWORD poisoned : 1;
	DWORD ai_bits : 5;
	DWORD really_active : 1;
	DWORD InDrawRoom : 1;
	DWORD meshswap_meshbits;
	short draw_room;
	short TOSSPAD;

}; // 15F6


struct LARA_INFO
{
	short item_number;
	short gun_status;
	short gun_type;
	short request_gun_type;
	short last_gun_type;
	short calc_fallspeed;
	short water_status;
	short climb_status;
	short pose_count;
	short hit_frame;
	short hit_direction;
	short air;
	short dive_count;
	short death_count;
	short current_active;
	short current_xvel;
	short current_yvel;
	short current_zvel;
	short spaz_effect_count;
	short flare_age;
	short vehicle;
	short weapon_item;
	short back_gun;
	short flare_frame;
	short poisoned;
	short dpoisoned;
	unsigned char electric;
	unsigned char wet[15];
	WORD flare_control_left : 1;
	WORD Unused1 : 1;
	WORD look : 1;
	WORD burn : 1;
	WORD keep_ducked : 1;
	WORD IsMoving : 1;
	WORD CanMonkeySwing : 1;
	WORD Unused2 : 1;
	WORD OnBeetleFloor : 1;
	WORD BurnGreen : 1;
	WORD IsDucked : 1;
	WORD has_fired : 1;
	WORD Busy : 1;
	WORD LitTorch : 1;
	WORD IsClimbing : 1;
	WORD Fired : 1;
	long water_surface_dist;
	PHD_VECTOR last_pos;
	void* spaz_effect;
	long mesh_effects;
	short* mesh_ptrs[15];
	Tr4ItemInfo* target;
	short target_angles[2];
	short turn_rate;
	short move_angle;
	short head_y_rot;
	short head_x_rot;
	short head_z_rot;
	short torso_y_rot;
	short torso_x_rot;
	short torso_z_rot;
	LARA_ARM left_arm;
	LARA_ARM right_arm;
	WORD holster;
	void* creature;
	long CornerX;
	long CornerZ;
	char RopeSegment;
	char RopeDirection;
	short RopeArcFront;
	short RopeArcBack;
	short RopeLastX;
	short RopeMaxXForward;
	short RopeMaxXBackward;
	long RopeDFrame;
	long RopeFrame;
	WORD RopeFrameRate;
	WORD RopeY;
	long RopePtr;
	void* GeneralPtr;
	long RopeOffset;
	DWORD RopeDownVel;
	char RopeFlag;
	char MoveCount;
	long RopeCount;
	char pistols_type_carried;
	char uzis_type_carried;
	char shotgun_type_carried;
	char crossbow_type_carried;
	char grenade_type_carried;
	char sixshooter_type_carried;
	char lasersight;
	char binoculars;
	char crowbar;
	char mechanical_scarab;
	unsigned char small_water_skin;
	unsigned char big_water_skin;
	char examine1;
	char examine2;
	char examine3;
	char puzzleitems[12];
	WORD puzzleitemscombo;
	WORD keyitems;
	WORD keyitemscombo;
	WORD pickupitems;
	WORD pickupitemscombo;
	short questitems;
	short num_small_medipack;
	short num_large_medipack;
	short num_flares;
	short num_pistols_ammo;
	short num_uzi_ammo;
	short num_revolver_ammo;
	short num_shotgun_ammo1;
	short num_shotgun_ammo2;
	short num_grenade_ammo1;
	short num_grenade_ammo2;
	short num_grenade_ammo3;
	short num_crossbow_ammo1;
	short num_crossbow_ammo2;
	short num_crossbow_ammo3;
	char beetle_uses;
	char blindTimer;
	char location;
	char highest_location;
	char locationPad;
};


typedef struct {
	PHD_3DPOS pos;
	short turn;
	unsigned char speed;
	char ydisp;
	unsigned char on;  	// bit 0 = on/off.
	unsigned char life;
} BAT_TR3;


// --------------- PRESET STRUCTURE ZONE -----------------------------
// Please, don't remove structures and fields you find in this "PRESET ZONE". They will be used by some
// preset functions of your plugin sources
// However, you can add new fields and structures in following structures, of course
typedef struct StrSavegameGlobalData {
	// FOR_YOU:
	// define here your variables that you wish were saved (and then restored) to/from savegame in GLOBAL section 
	//           (only one for all levels)
	// note: the size of this structure should be always even (if you add BYTE variable, remember to compensate that 
	//       with another BYTE vairable or placefolder)
	Magazine shotgunMag;
	Magazine revolverMag;
	bool disableCtrl;
	bool disableDraw;
	
}SavegameGlobalDataFields;

typedef struct StrSavegameLocalData {
	// FOR_YOU:
	// define here your variables that you wish were saved (and then restored) to/from savegame in LOCAL section (one for each different level)
	// note: the size of this structure should be always even (if you add BYTE variable, compensate it with another BYTE vairable or placefolder)
	short runCounter;
	bool tired;
}SavegameLocalDataFields;


typedef struct StrSavegameData {
	StrSavegameGlobalData Global;
	StrSavegameLocalData  Local;
}SavegameDataFields;


typedef struct StrBaseAssignSlotMine {
	int TotAssign;
	StrRecordAssSlot VetAssignSlot[MAX_ASSIGN_SLOT_MINE];
}BaseAssignSlotMineFields;


enum MatrixIndex
{
	M00,M01,M02,M03,
	M10,M11,M12,M13,
	M20,M21,M22,M23
};


typedef struct StrMyData {
	StrSavegameData Save;  // variable that it will be saved and restored to/from savegame
	// FOR_YOU:
	// define here all your global variables, i.e. those variables that will be seen from all procedures and keep 
	// their value between all cycles of the game.
	// after you defined a variabile like:
	// int  Alfa;
	// then you'll be able to access to them using syntax like:
	// MyData.Alfa = 3;
	// if (MyData.Alfa == 5) .
	int TotProgrActions;
	int LastProgrActionIndex; // used in case of overloading
	short BatSlot;
	short VignetteSprite;
	int electric;

	int subtitleLife;
	float subtitleY;
	float lineSpacing;

	StrProgressiveAction VetProgrActions[MAX_MYPROGR_ACTIONS];
	StrBaseGenericCustomize BaseCustomizeMine;  // stored all your customize script commands for current level
	StrBaseGenericParameters BaseParametersMine; // store of all your parameters= script commands of current level
	StrBaseAssignSlotMine BaseAssignSlotMine; // stored all AssignSlot= command script commands of current level

}MyDataFields;
// ----------------- END PRESET STRUCTURE ZONE ---------------------------------------

// let below pragma directive at bottom of this source
#pragma pack(2)