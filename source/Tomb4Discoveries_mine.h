
// FOR_YOU: here you can type your discoveries about tomb4 procedures
// if you mean use the C language to call them, you can create the
// prototypes of them like it has been done in the "DefTomb4Funct.h" file
// and "functions.h" file.
// While if you wish use only assembly code, you can type here
// the #define ADR_NOME_PROC 0x404040  to remember and use in the asm
// code all addresses of the tomb4 procedures

// TYPE_HERE: prototype (c++ language), mnemonic constants for 
// addresses (assembly)
typedef int(__cdecl *TYPE_GetLaraJointPos) (StrPosizione *pPos, int joint);
TYPE_GetLaraJointPos GetLaraJointPos = (TYPE_GetLaraJointPos)0x41D890;

typedef long(__cdecl *TYPE_GetFreeSpark) (void);
TYPE_GetFreeSpark GetFreeSpark = (TYPE_GetFreeSpark)0x433830;

typedef void(__cdecl *TYPE_TriggerFireFlame) (long x, long y, long z, long body_part, long type);
TYPE_TriggerFireFlame TriggerFireFlame = (TYPE_TriggerFireFlame)0x434BB0;

typedef void(__cdecl *TYPE_phd_GetVectorAngles) (int distX, int distY, int distZ, short* dest);
TYPE_phd_GetVectorAngles phd_GetVectorAngles = (TYPE_phd_GetVectorAngles)0x48E710;

typedef void (__cdecl *TYPE_TriggerBlood)(int x, int y, int z, int direction, int speed);
TYPE_TriggerBlood TriggerBlood = (TYPE_TriggerBlood) 0x4388E0;

typedef void (__cdecl *TYPE_UpdateBeetles)(void);
typedef void (__cdecl *TYPE_UpdateFish)(void);
TYPE_UpdateBeetles UpdateBeetles = (TYPE_UpdateBeetles) 0x40E2F0;
TYPE_UpdateFish UpdateFish = (TYPE_UpdateFish) 0x403850;

typedef void (__cdecl *TYPE_DrawFish)(void);
typedef void (__cdecl *TYPE_DrawBeetles)(void);
TYPE_DrawFish DrawFish = (TYPE_DrawFish) 0x403BF0;
TYPE_DrawBeetles DrawBeetles = (TYPE_DrawBeetles) 0x40E550;

typedef void(__cdecl *TYPE_phd_PushUnitMatrix) (void);
TYPE_phd_PushUnitMatrix phd_PushUnitMatrix = (TYPE_phd_PushUnitMatrix) 0x48DF90;

typedef void(__cdecl *TYPE_phd_TranslateRel) (int x, int y, int z);
TYPE_phd_TranslateRel phd_TranslateRel = (TYPE_phd_TranslateRel) 0x48DFD0;

typedef void (__cdecl *TYPE_setXY4) (DXVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long z, short* clip);
TYPE_setXY4 setXY4 = (TYPE_setXY4) 0x483420;

typedef void (__cdecl *TYPE_setXYZ4) (DXVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, long x4, long y4, long z4, short* clip);
TYPE_setXYZ4 setXYZ4 = (TYPE_setXYZ4) 0x483770;

typedef void (__cdecl *TYPE_AddQuadSorted) (DXVERTEX* v, short s1, short s2, short s3, short s4, TEXTURESTRUCT* t, long l1);
TYPE_AddQuadSorted* AddQuadSorted = (TYPE_AddQuadSorted*) 0x5339A4;

typedef int(__cdecl* TYPE_DrawPhaseGame)(void);
TYPE_DrawPhaseGame DrawPhaseGame = (TYPE_DrawPhaseGame) 0x44E250;

typedef void(__cdecl* TYPE_UpdatePulseColour) (void);
TYPE_UpdatePulseColour UpdatePulseColour = (TYPE_UpdatePulseColour) 0x462AE0;

typedef int(__cdecl* TYPE_TestLaraVault) (Tr4ItemInfo* pLara, Tr4CollInfo* pCollision);
TYPE_TestLaraVault TestLaraVault = (TYPE_TestLaraVault) 0x4222B0;

typedef void(__cdecl* TYPE_GetLaraCollisionInfo) (Tr4ItemInfo* pLara, Tr4CollInfo* pCollision);
TYPE_GetLaraCollisionInfo GetLaraCollisionInfo = (TYPE_GetLaraCollisionInfo) 0x422180;

typedef long(__cdecl* TYPE_UseInventoryItems) (Tr4ItemInfo* item);
TYPE_UseInventoryItems UseInventoryItems = (TYPE_UseInventoryItems) 0x424C70;

typedef void(__cdecl* TYPE_LookUpDown) (void);
TYPE_LookUpDown LookUpDown = (TYPE_LookUpDown)0x4288F0;

typedef short(__cdecl* TYPE_LaraFloorFront) (Tr4ItemInfo* item, short ang, long dist);
TYPE_LaraFloorFront LaraFloorFront = (TYPE_LaraFloorFront)0x421450;

typedef short(__cdecl* TYPE_LaraCeilingFront) (Tr4ItemInfo* item, short ang, long dist, long h);
TYPE_LaraCeilingFront LaraCeilingFront = (TYPE_LaraCeilingFront)0x4208B0;

typedef void(__cdecl* TYPE_lara_as_wade) (Tr4ItemInfo* item, Tr4CollInfo* coll);
TYPE_lara_as_wade lara_as_wade = (TYPE_lara_as_wade)0x427390;
