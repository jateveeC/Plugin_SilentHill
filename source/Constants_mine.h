
// TYPE_HERE: here you can type your mnemonic constants, using the #define directive
// like trng did it in the "Tomb_NextGeneration.h" file
// in this source you can also type your declaration of tomb raider function
// like it happens for "functions.h" source

// -------- START PRESET CONSTANTS ---------------------------------
// please don't remove the constants in this "PRESET CONSTANT" zone. 
// They will be used by some preset function in your plugin source

#define MAX_MYPROGR_ACTIONS 100
#define MAX_ASSIGN_SLOT_MINE 200

#define ENDLESS_DURATE 0xFFFF // to use as number of frames to set an infinite durate
#define NGTAG_END_SEQUENCE		   0   // signals the end of ng token sequence
#define NGTAG_PROGRESSIVE_ACTIONS  1
#define NGTAG_LOCAL_DATA           2
#define NGTAG_GLOBAL_DATA          3
#define AXN_FREE  0  // this record is to free an action record. You type this value in ActionType to free a action progress record
// --------- END PRESET CONSTANTS ---------------------------------
// type here the constant name for new progressive action you create
// use progressive number to have always different value for each AXN_ constant/action
#define TOMB4_COLOR_TABLE 0x4BF5D8
#define COL_WHITE_BLK 0xD8
#define COL_WHITE 0x158
#define COL_RED 0x1D8
#define COL_BLUE 0x258
#define COL_GREY 0x2D8
#define COL_GOLD 0x358
#define COL_GREY_DARK 0x3D8
#define COL_YELLOW 0x458
#define COL_RED2 0x4D8
#define TOMB4_SAMPLE_LUT 0x7F7580
#define TOMB4_SAMPLE_INFO 0x7F7584
#define PHD_MXPTR (*(long**)0x7E70C8)
#define phd_PopMatrix() (PHD_MXPTR -= 12)

#define PARAM_SUBTITLES 1
#define PARAM_MENU 2
#define CUST_RADIO_STATIC 1
#define CUST_VIGNETTE 2

#define ONE_STEP 256

#define LINE_SPACING 0.055f

#define	MAX_BATS	64
#define OBJ_BAT 1

#define SECOND 30
#define SUBTITLE_LIFE SECOND*10
#define STAMINA_MAX SECOND*3*15
#define STAMINA_THRESHOLD SECOND*3*2

#define TRNG_LARA_AS 0x10153B90
#define TRNG_LARA_COL 0x10153F90

#define phd_winxmin (*(short*)(0x753C04))
#define phd_winxmax (*(short*)(0x753BEC))
#define phd_winymin (*(short*)(0x753BBC))
#define phd_winymax (*(short*)(0x753C10))
#define phd_centerx (*(long*)(0x753BB4))

#define f_moneoznear (*(float*)(0x753C28))
#define f_mznear (*(float*)(0x753BA4))
#define clipflags (*(short**)(0x753854))

#define spriteinfo (*(SPRITESTRUCT**)(0x533994))
#define objects ((Tr4ObjectInfo*)(0x52B720))
#define items (*(Tr4ItemInfo**)(0x7FE16C))
#define anims (*(Tr4AnimStruct**)(0x533938))
#define input (*(long*)(0x5355D8))
#define thread_ended (*(long*)(0x5339DC))
#define NO_HEIGHT -32512
#define height_type (*(long*)(0x7FE170))

#define lara_item (*(Tr4ItemInfo**)(0x80E01C))
#define lara (*(LARA_INFO*)(0x80DEC0))
#define DashTimer (*(short*)(0x80E01A))

#define nPolyType (*(long*)(0x7E70D0))
#define dbinput (*(long*)(0x535784))
#define SetDebounce (*(long*)(0x7FE040))

#define LINE_POINTS 4
#define GRID_POINTS (4*4)

// MPS_ flags
// you should type here your MPS_ flags for plugin command.
// then you'll test the presence of these flags checking the Trng.MainPluginFlags  value using "&" operator
// please: do not use the value 0x40000000 (MPS_DISABLE) because it has been already set by trng engine
