#include "..\includes\stdafx.h"
#include "..\includes\GTA\common.h"
#include "..\includes\hooking\Hooking.Patterns.h"
#include "..\includes\GTA\global.h"

//#define _LOG
#ifdef _LOG
#include <fstream>
ofstream logfile;
#endif // _LOG

bool bDelay;

hook::pattern dwGameLoadStatePattern, DxInputNeedsExclusive, EmergencyVehiclesFixPattern, RadarScalingPattern;
hook::pattern MenuPattern, MenuPattern15625, RsSelectDevicePattern, CDarkelDrawMessagesPattern, CDarkelDrawMessagesPattern2, CParticleRenderPattern;
hook::pattern DrawHudHorScalePattern, DrawHudVerScalePattern, CSpecialFXRender2DFXsPattern, CSceneEditDrawPattern, sub61DEB0Pattern;
hook::pattern MenuPattern1, MenuPattern2, MenuPattern3, MenuPattern4, MenuPattern5, MenuPattern6, MenuPattern7, MenuPattern8, MenuPattern9, MenuPattern10, MenuPattern11;
hook::pattern ResolutionPattern0, ResolutionPattern1, ResolutionPattern2, ResolutionPattern3, ResolutionPattern4, ResolutionPattern5;
hook::pattern CRadarPattern, BordersPattern;
uint32_t* dwGameLoadState;
float* fCRadarRadarRange;
uint32_t funcCCameraAvoidTheGeometryJmp;
uint32_t* SetColorAddr;
uint32_t* nFontColor;
uint8_t* bBackgroundOn;
bool* bWantsToDrawHud;
void OverwriteResolution();

void GetPatterns()
{
    dwGameLoadStatePattern = hook::pattern("C7 05 ? ? ? ? 09 00 00 00"); //0x582E78
    EmergencyVehiclesFixPattern = hook::pattern("D9 9B F0 00 00 00 D9 83 EC 00 00 00"); //0x4A42A0
    RadarScalingPattern = hook::pattern("D9 1D ? ? ? ? EB 3C"); //0x4C5D0A
    RsSelectDevicePattern = hook::pattern("C7 40 08 01 00 00 00 B8 01 00 00 00"); //0x581F77
    BordersPattern = hook::pattern("74 ? B9 ? ? ? ? E8"); //0x48E13E //0x4FE5D0

    ResolutionPattern0 = hook::pattern("E8 ? ? ? ? 85 C0 74 0A B8 01 00 00 00 E9 B1 00 00 00"); //0x584A60
    ResolutionPattern1 = hook::pattern("8D 45 04 83 C4 0C 6A 01 50 53"); //0x4890F0 + 0x2
    ResolutionPattern2 = hook::pattern("83 3D ? ? ? ? 00 59 59 74 2B 81 3C 24");
    ResolutionPattern3 = hook::pattern("C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? 80 02 00 00");
    ResolutionPattern4 = hook::pattern("85 DB 75 0D E8 ? ? ? ? 39 05 ? ? ? ? 7C A4"); //0x581E9A
    ResolutionPattern5 = hook::pattern("A3 ? ? ? ? E9 84 00 00 00"); //0x581E37

    MenuPattern1 = hook::pattern("A1 ? ? ? ? 83 EC 10 3D"); //0x48ABE0
    MenuPattern2 = hook::pattern("FF 35 ? ? ? ? E8 ? ? ? ? 8D 84 24 ? ? 00 00 50"); //0x47E0FE
    MenuPattern3 = hook::pattern("6A 00 6A 00 6A 00 E8"); //black text

    char pattern_str[20];
    union {
        uint32_t* Int;
        unsigned char Byte[4];
    } dword_temp;

    dword_temp.Int = *hook::pattern("D8 0D ? ? ? ? D8 0D ? ? ? ? DA 6C 24 0C D9 1C 24 ").get(0).get<uint32_t*>(2);
    sprintf(pattern_str, "%02X %02X %02X %02X %02X %02X", 0xD8, 0x0D, dword_temp.Byte[0], dword_temp.Byte[1], dword_temp.Byte[2], dword_temp.Byte[3]);
    CDarkelDrawMessagesPattern = hook::pattern(pattern_str); //0x5ECD00

    dword_temp.Int = *hook::pattern("D8 0D ? ? ? ? 50 D8 0D ? ? ? ? D9 1C 24 A1 ? ? ? ? 89 44 24 08").get(0).get<uint32_t*>(2);
    sprintf(pattern_str, "%02X %02X %02X %02X %02X %02X", 0xD8, 0x0D, dword_temp.Byte[0], dword_temp.Byte[1], dword_temp.Byte[2], dword_temp.Byte[3]);
    CDarkelDrawMessagesPattern2 = hook::pattern(pattern_str); //0x5ECCF8

    dword_temp.Int = *hook::pattern("D8 0D ? ? ? ? D8 0D ? ? ? ? D8 0D ? ? ? ? DD D9 D9 44 24 1C D8 C1 D9 1C 24").get(0).get<uint32_t*>(2);
    sprintf(pattern_str, "%02X %02X %02X %02X %02X %02X", 0xD8, 0x0D, dword_temp.Byte[0], dword_temp.Byte[1], dword_temp.Byte[2], dword_temp.Byte[3]);
    DrawHudHorScalePattern = hook::pattern(pattern_str); //0x5FDB58

    dword_temp.Int = *hook::pattern("D8 0D ? ? ? ? D8 0D ? ? ? ? D8 0D ? ? ? ? DD DA D9 44 24 1C D8 C2 D9 1C 24").get(0).get<uint32_t*>(2);
    sprintf(pattern_str, "%02X %02X %02X %02X %02X %02X", 0xD8, 0x0D, dword_temp.Byte[0], dword_temp.Byte[1], dword_temp.Byte[2], dword_temp.Byte[3]);
    DrawHudVerScalePattern = hook::pattern(pattern_str); //0x5FDB4C

    dword_temp.Int = *hook::pattern("D8 0D ? ? ? ? DD D9 D9 05 ? ? ? ? D8 C9 D9 1C 24 50").get(0).get<uint32_t*>(2);
    sprintf(pattern_str, "%02X %02X %02X %02X %02X %02X", 0xD8, 0x0D, dword_temp.Byte[0], dword_temp.Byte[1], dword_temp.Byte[2], dword_temp.Byte[3]);
    sub61DEB0Pattern = hook::pattern(pattern_str); //0x592969

    dword_temp.Int = *hook::pattern("D8 0D ? ? ? ? D8 0D ? ? ? ? DD DA D9 00 D8 CA").get(0).get<uint32_t*>(2);
    sprintf(pattern_str, "%02X %02X %02X %02X %02X %02X", 0xD8, 0x0D, dword_temp.Byte[0], dword_temp.Byte[1], dword_temp.Byte[2], dword_temp.Byte[3]);
    CRadarPattern = hook::pattern(pattern_str); //0x5F7148
}

void GetMemoryAddresses()
{
    RsGlobal = *hook::pattern("8B 0D ? ? ? ? 6A 00 6A 00 52 50").get(0).get<RsGlobalType *>(2); //0x8F4360
    CDraw::pfScreenAspectRatio = *hook::pattern("FF 35 ? ? ? ? EB 0B").get(0).get<float*>(2); //0x5F53C0
    CDraw::pfScreenFieldOfView = *hook::pattern("D8 0D ? ? ? ? 53 83 EC 20 8A 44 24 28").get(0).get<float*>(2); //0x5FBC6C
    CSprite2dDrawRect = (int(__cdecl *)(CRect const &, CRGBA const &)) hook::pattern("8B 44 24 04 53 8B 5C 24 0C 6A 00 53 53 53 53 50").get(0).get<uint32_t>(0); //0x51F970
    bWideScreen = *hook::pattern("80 3D ? ? ? ? 00 DD D9 74 0D").get(0).get<bool*>(2);//0x95CD23
    BordersVar1 = *hook::pattern("A1 ? ? ? ? 53 83 EC 30 85 C0 89 CB").get(0).get<uint32_t*>(1); //0x6FADA0
    BordersVar2 = BordersVar1;
    FindPlayerVehicle = (int(__cdecl *)()) hook::pattern("0F B6 ? ? ? ? ? 6B C0 ? 8B 0C ? ? ? ? ? 85 C9 74 10").get(0).get<uint32_t>(0);//0x4A10C0
    bIsInCutscene = *hook::pattern("80 3D ? ? ? ? ? 0F 84 84 00 00 00 8B 35").get(0).get<bool*>(2); //0x6FAD68
    dwGameLoadState = *dwGameLoadStatePattern.get(0).get<uint32_t*>(2);
    fCRadarRadarRange = *RadarScalingPattern.get(0).get<float*>(2);
    SetColorAddr = hook::pattern("53 83 EC 10 8B 5C 24 18 8A 13").get(0).get<uint32_t>(0);
    nFontColor = *hook::pattern("88 15 ? ? ? ? 8A 43 01 A2 ? ? ? ? 8A 4B 02 88 0D ? ? ? ? 8A 43 03 A2 ? ? ? ? D9 EE").get(0).get<uint32_t*>(2);
    bBackgroundOn = *hook::pattern("C6 05 ? ? ? ? 01 C3").get(2).get<uint8_t*>(2); //0x8F3197
    bWantsToDrawHud = *hook::pattern("80 3D ? ? ? ? 01 8B 5C 24 64 8B 6C 24 54").get(0).get<bool*>(2); //0x4A5877
}

void ShowRadarTrace(float fX, float fY, unsigned int nScale, BYTE r, BYTE g, BYTE b, BYTE a)
{
    if (*bWantsToDrawHud == true && !*bIsInCutscene)
    {
        float	fWidthMult = fWideScreenWidthScaleDown;
        float	fHeightMult = 1.0f / 480.0f;

        CSprite2dDrawRect(CRect(fX - ((nScale + 1.0f) * fWidthMult * RsGlobal->MaximumWidth),
                                fY + ((nScale + 1.0f) * fHeightMult * RsGlobal->MaximumHeight),
                                fX + ((nScale + 1.0f) * fWidthMult * RsGlobal->MaximumWidth),
                                fY - ((nScale + 1.0f) * fHeightMult * RsGlobal->MaximumHeight)),
                                CRGBA(0, 0, 0, a));

        CSprite2dDrawRect(CRect(fX - (nScale * fWidthMult * RsGlobal->MaximumWidth),
                                fY + (nScale * fHeightMult * RsGlobal->MaximumHeight),
                                fX + (nScale * fWidthMult * RsGlobal->MaximumWidth),
                                fY - (nScale * fHeightMult * RsGlobal->MaximumHeight)),
                                CRGBA(r, g, b, a));
    }
}

void SilentPatchCompatibility()
{
    OverwriteResolution();
    auto pattern = hook::pattern("FF 74 24 60 FF 74 24 60 E8");
    injector::MakeCALL(pattern.get(2).get<uint8_t*>(8), ShowRadarTrace, true); //0x4A45FC
    injector::MakeCALL(pattern.get(3).get<uint8_t*>(8), ShowRadarTrace, true); //0x4A47CD
    injector::MakeCALL(pattern.get(4).get<uint8_t*>(8), ShowRadarTrace, true); //0x4A4A0F
    injector::MakeCALL(pattern.get(5).get<uint8_t*>(8), ShowRadarTrace, true); //0x4A4BD9
    //anything else?
}

injector::hook_back<signed int(__cdecl*)(void)> hbRsSelectDevice;
signed int __cdecl RsSelectDeviceHook2()
{
    SilentPatchCompatibility();
    return hbRsSelectDevice.fun();
}

void OverwriteResolution()
{
    CIniReader iniReader("");
    ResX = iniReader.ReadInteger("MAIN", "ResX", -1);
    ResY = iniReader.ReadInteger("MAIN", "ResY", -1);
    bSmallerTextShadows = iniReader.ReadInteger("MAIN", "SmallerTextShadows", 1) != 0;

    if (!ResX || !ResY) {
        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info;
        info.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(monitor, &info);
        ResX = info.rcMonitor.right - info.rcMonitor.left;
        ResY = info.rcMonitor.bottom - info.rcMonitor.top;
    }
    else if (ResX == -1 || ResY == -1)
    {
        return;
    }

    if (!hbRsSelectDevice.fun)
        hbRsSelectDevice.fun = injector::MakeCALL(ResolutionPattern0.get(0).get<uint32_t>(0), RsSelectDeviceHook2).get();

    injector::WriteMemory<uint8_t>(ResolutionPattern1.get(0).get<uint32_t>(2), 0x08, true);

    injector::WriteMemory(ResolutionPattern2.get(0).get<uint32_t>(14), ResX, true); //0x581E5B + 0x3
    injector::WriteMemory(ResolutionPattern2.get(0).get<uint32_t>(24), ResY, true); //0x581E64 + 0x4
    injector::WriteMemory<uint8_t>(ResolutionPattern2.get(0).get<uint32_t>(34), 32, true); //0x581E6E + 0x4

    injector::WriteMemory(ResolutionPattern3.get(0).get<uint32_t>(16), ResX, true); //0x584C4A + 0x6
    injector::WriteMemory(ResolutionPattern3.get(0).get<uint32_t>(26), ResY, true); //0x584C54 + 0x6
    injector::WriteMemory(ResolutionPattern3.get(1).get<uint32_t>(16), ResX, true); //0x584C5E + 0x6
    injector::WriteMemory(ResolutionPattern3.get(1).get<uint32_t>(26), ResY, true); //0x584C68 + 0x6

    injector::WriteMemory<uint8_t>(ResolutionPattern4.get(0).get<uint32_t>(15), 0xEB, true); //jl      short loc_600E60 > jmp      short loc_600E60

    injector::MakeNOP(ResolutionPattern5.get(0).get<uint32_t>(5), 5, true);
    injector::WriteMemory<uint16_t>(ResolutionPattern5.get(0).get<uint32_t>(5), 0x07EB, true);
}

void FixAspectRatio()
{
    auto pattern = hook::pattern("74 08 D9 05 ? ? ? ? EB 06");
    injector::MakeNOP(pattern.get(9).get<uint32_t>(0), 2, true); //0x46B618
    pattern = hook::pattern("80 3D ? ? ? ? 00 DD D9 74 0A FF 35");
    injector::MakeNOP(pattern.get(0).get<uint32_t>(0), 7, true); //0x48D06B
    injector::MakeNOP(pattern.get(0).get<uint32_t>(9), 2, true); //0x48D074

    pattern = hook::pattern("EB 00 5D 5B C3"); //0x584B26
    injector::MakeJMP(pattern.get(0).get<uint32_t>(4), CDraw::CalculateAspectRatio, true);
}

void FixFOV()
{
    auto pattern = hook::pattern("D9 44 24 04 D9 1D ? ? ? ? C3"); //0x4FE7B0
    injector::MakeJMP(pattern.get(0).get<uint32_t>(0), CDraw::SetFOV, true);

    struct EmergencyVehiclesFix
    {
        void operator()(injector::reg_pack& regs)
        {
            *(float*)(regs.ebx + 0xF0) = fEmergencyVehiclesFix;
        }
    }; injector::MakeInline<EmergencyVehiclesFix>(EmergencyVehiclesFixPattern.get(0).get<uint32_t>(0), EmergencyVehiclesFixPattern.get(0).get<uint32_t>(6));

    struct RadarScaling
    {
        void operator()(injector::reg_pack& regs)
        {
            _asm
            {
                fstp    ds : fRadarScaling
            }
            *fCRadarRadarRange = fRadarScaling;
            fRadarScaling -= 120.0f;
        }
    }; injector::MakeInline<RadarScaling>(RadarScalingPattern.get(0).get<uint32_t>(0), RadarScalingPattern.get(0).get<uint32_t>(6));
}

float __stdcall MenuScaleHook(float fScaleFactor)
{
    if (fScaleFactor == 225.0f || fScaleFactor == 415.0f) //logo
        fScaleFactor = ((640.0f * (*CDraw::pfScreenAspectRatio / (4.0f / 3.0f))) / 2.0f) - (320.0f - fScaleFactor);

    if (fScaleFactor == 320.0f || fScaleFactor == 321.0f)
        return fScaleFactor * 0.0015625f * (float)RsGlobal->MaximumWidth;
    else
        return fScaleFactor * fWideScreenWidthScaleDown * (float)RsGlobal->MaximumWidth;
}

void FixMenu()
{
    injector::MakeJMP(MenuPattern1.get(0).get<uint32_t>(0), MenuScaleHook, true);
    static const float fGameRadioOffset = 320.0f - 1.0f;
    injector::WriteMemory(MenuPattern2.get(5).get<uint32_t>(2), &fGameRadioOffset, true); //game radio icon fix
}

void RsSelectDeviceHook()
{
    struct RSDH
    {
        void operator()(injector::reg_pack& regs)
        {
            *(DWORD*)(regs.eax + 8) = 1;
            CDraw::CalculateAspectRatio();
            FixMenu();
        }
    }; injector::MakeInline<RSDH>(RsSelectDevicePattern.get(0).get<uint32_t>(0), RsSelectDevicePattern.get(0).get<uint32_t>(7));
}

void FixCoronas()
{
    auto pattern = hook::pattern("D8 0E D9 1E D9 05 ? ? ? ? D8 35 ? ? ? ? D8 0B D9 1B"); //0x51C46A 
    injector::WriteMemory<uint8_t>(pattern.get(0).get<uint32_t>(1), 0x0B, true);

    auto pfCAutoPreRender = (uint32_t)hook::pattern("81 EC F8 06 00 00 0F BF 45 5C 3D 83 00 00 00").get(0).get<uint32_t>(0);
    auto pfCBrightLightsRegisterOne = (uint32_t)hook::pattern("D9 EE D9 EE 83 EC 20 8B 44 24").get(0).get<uint32_t>(0);
    pattern = hook::range_pattern(pfCAutoPreRender, pfCAutoPreRender + 0x4309, "E8 ? ? ? ?");
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        auto addr = pattern.get(i).get<uint32_t>(0);
        auto dest = injector::GetBranchDestination(addr, true).as_int();
        if (dest == pfCBrightLightsRegisterOne)
            injector::MakeNOP(addr, 5, true); //CBrightLights::RegisterOne
    }
}

injector::hook_back<void(__fastcall*)(void*)> hbDrawBorders;
static void __fastcall DrawBordersForWideScreenHook(void* _this)
{
    if (!*bWideScreen == false)
        return hbDrawBorders.fun(_this);
}

void FixBorders()
{
    auto pattern = hook::pattern("74 14 D9 05 ? ? ? ? D8 0D ? ? ? ? D8 0D ? ? ? ? EB 12");
    injector::MakeNOP(pattern.get(0).get<uint32_t>(0), 2, true); //0x46086E
    //pattern = hook::pattern("74 08 D9 05 ? ? ? ? EB 06");
    //injector::MakeNOP(pattern.get(9).get<uint32_t>(0), 2, true); //0x46B618
    pattern = hook::pattern("D9 EE 74 15 80 3D ? ? ? ? 00 75 0C");
    injector::MakeNOP(pattern.get(0).get<uint32_t>(0), 2, true); //0x46EBB9
    pattern = hook::pattern("74 0D FF 35 ? ? ? ? EB 0B");
    injector::MakeNOP(pattern.get(0).get<uint32_t>(0), 2, true); //0x48CF81
    pattern = hook::pattern("80 3D ? ? ? ? 00 0F 84 84 00 00 00 8B 35 ? ? ? ? 89 F0");
    injector::WriteMemory<uint8_t>(pattern.get(0).get<uint32_t>(6), 1, true); //0x48D2D5

    hbDrawBorders.fun = injector::MakeCALL(BordersPattern.get(11).get<uint32_t>(7), DrawBordersForWideScreenHook).get();
    injector::MakeCALL(BordersPattern.get(17).get<uint32_t>(7), DrawBordersForWideScreenHook);
}

void FixHUD()
{
    for (size_t i = 0; i < CDarkelDrawMessagesPattern.size(); i++)
    {
        uint32_t* p15625 = CDarkelDrawMessagesPattern.get(i).get<uint32_t>(2);
        injector::WriteMemory(p15625, &fWideScreenWidthScaleDown, true);
    }

    auto pattern = hook::pattern("D8 0D ? ? ? ? 89 44 24 08 50 D8 0D ? ? ? ? DA 6C 24 0C"); //0x48E363
    injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &fWideScreenWidthScaleDown, true); //sniper scope border @Render2dStuff
    pattern = hook::pattern("D8 0D ? ? ? ? 89 54 24 0C FF 35 ? ? ? ? D8 0D ? ? ? ? 50 DA 44 24 14"); //0x48E3CC
    injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &fWideScreenWidthScaleDown, true);
    pattern = hook::pattern("D8 0D ? ? ? ? D8 0D ? ? ? ? 50 D9 1C 24 E8 ? ? ? ? 59 59"); 
    injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &fWideScreenWidthScaleDown, true); //0x48DC09
    injector::WriteMemory(pattern.get(1).get<uint32_t>(2), &fWideScreenWidthScaleDown, true); //48DCDB

    for (size_t i = 0; i < DrawHudHorScalePattern.size(); i++)
    {
        uint32_t* p15625 = DrawHudHorScalePattern.get(i).get<uint32_t>(2);
        injector::WriteMemory(p15625, &fWideScreenWidthScaleDown, true);
    }

    pattern = hook::pattern("50 D8 0D ? ? ? ? D8 0D ? ? ? ? D9 1C 24 E8"); // radio text
    injector::WriteMemory(pattern.get(24).get<uint32_t>(3), &fWideScreenWidthScaleDown, true);
    injector::WriteMemory(pattern.get(25).get<uint32_t>(3), &fWideScreenWidthScaleDown, true);
    injector::WriteMemory(pattern.get(27).get<uint32_t>(3), &fWideScreenWidthScaleDown, true);//0x595F45 replay

    for (size_t i = 0; i < sub61DEB0Pattern.size(); i++)
    {
        uint32_t* p15625 = sub61DEB0Pattern.get(i).get<uint32_t>(2);
        injector::WriteMemory(p15625, &fWideScreenWidthScaleDown, true);
    }

    //injector::WriteMemory(0x4F6D90 + 0x5DC + 0x2, &fWideScreenWidthScaleDown, true); //CClouds::Render ??

    for (size_t i = 0; i < CRadarPattern.size(); i++)
    {
        uint32_t* p15625 = CRadarPattern.get(i).get<uint32_t>(2);
        injector::WriteMemory(p15625, &fWideScreenWidthScaleDown, true);
    }

    pattern = hook::pattern("D9 05 ? ? ? ? D8 C9 D9 05 ? ? ? ? D8 CA DE C1 D8 25"); // text box width
    injector::WriteMemory(*pattern.get(0).get<uint32_t*>(2), 350.0f, true);
}

void FixCrosshair()
{
    for (size_t i = 0; i < DrawHudVerScalePattern.size(); i++)
    {
        if (i >= 0 && i <= 2) //0x5052A0 + 0x2D0 + 0x2, 0x5052A0 + 0x3AB + 0x2, 0x5052A0 + 0x4BC + 0x2
        {
            uint32_t* pCustomScaleVer = DrawHudVerScalePattern.get(i).get<uint32_t>(2);
            injector::WriteMemory(pCustomScaleVer, &fCrosshairHeightScaleDown, true);
        }
    }

    auto pattern = hook::pattern("D8 0D ? ? ? ? D9 5C 24 14 DB 05"); //0x50550D
    injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &fCrosshairPosFactor, true);
}

injector::hook_back<void(__cdecl*)(float, float, wchar_t*)> hbPrintString2;
void __cdecl PrintStringHook2(float PosX, float PosY, wchar_t* c)
{
    if (*bBackgroundOn == 1)
        return hbPrintString2.fun(PosX, PosY, c);

    unsigned char originalColorA = (*nFontColor >> 24) & 0xFF;
    unsigned char originalColorB = (*nFontColor >> 16) & 0xFF;
    unsigned char originalColorG = (*nFontColor >> 8) & 0xFF;
    unsigned char originalColorR = *nFontColor & 0xFF;

    if (!originalColorR && !originalColorG && !originalColorB)
        return;

    CRGBA rgba;
    if ((originalColorR == 0x01 && originalColorG == 0x01 && originalColorB == 0x01) || (originalColorR == 0x14 && originalColorG == 0x14 && originalColorB == 0x14))
    {
        rgba.colorInt = (uint32_t)0xFFFFFFFFF; rgba.alpha = originalColorA / 10;
    }
    else
    {
        rgba.colorInt = (uint32_t)0x00000000; rgba.alpha = originalColorA;
    }

    injector::cstd<void(CRGBA*)>::call(SetColorAddr, &rgba);

    hbPrintString2.fun(PosX + 1.0f, PosY, c);
    hbPrintString2.fun(PosX - 1.0f, PosY, c);
    hbPrintString2.fun(PosX, PosY + 1.0f, c);
    hbPrintString2.fun(PosX, PosY - 1.0f, c);
    hbPrintString2.fun(PosX + 1.0f, PosY + 1.0f, c);
    hbPrintString2.fun(PosX - 1.0f, PosY + 1.0f, c);
    hbPrintString2.fun(PosX + 1.0f, PosY - 1.0f, c);
    hbPrintString2.fun(PosX - 1.0f, PosY - 1.0f, c);

    if (ReplaceTextShadowWithOutline > 1)
    {
        hbPrintString2.fun(PosX + 2.0f, PosY, c);
        hbPrintString2.fun(PosX - 2.0f, PosY, c);
        hbPrintString2.fun(PosX, PosY + 2.0f, c);
        hbPrintString2.fun(PosX, PosY - 2.0f, c);
        hbPrintString2.fun(PosX + 2.0f, PosY + 2.0f, c);
        hbPrintString2.fun(PosX - 2.0f, PosY + 2.0f, c);
        hbPrintString2.fun(PosX + 2.0f, PosY - 2.0f, c);
        hbPrintString2.fun(PosX - 2.0f, PosY - 2.0f, c);
    }
    rgba.red = originalColorR;
    rgba.green = originalColorG;
    rgba.blue = originalColorB;
    rgba.alpha = originalColorA;
    injector::cstd<void(CRGBA*)>::call(SetColorAddr, &rgba);
    hbPrintString2.fun(PosX, PosY, c);
}

void ApplyIniOptions()
{
    CIniReader iniReader("");
    fHudWidthScale = iniReader.ReadFloat("MAIN", "HudWidthScale", 0.0f); fHudWidthScale == 0.0f ? fHudWidthScale = 0.8f : fHudWidthScale;
    fHudHeightScale = iniReader.ReadFloat("MAIN", "HudHeightScale", 0.0f); fHudHeightScale == 0.0f ? fHudHeightScale = 0.8f : fHudHeightScale;

    if (fHudWidthScale && fHudHeightScale)
    {
        for (size_t i = 0; i < DrawHudHorScalePattern.size(); i++)
        {
            if (i > 2 && i != 5 && i != 6 && i != 7 && i != 8 && i != 60 && i != 65 && i != 81) //0 1 2 - crosshair /65 - subs
            {
                uint32_t* pCustomScaleHor = DrawHudHorScalePattern.get(i).get<uint32_t>(2);
                injector::WriteMemory(pCustomScaleHor, &fCustomWideScreenWidthScaleDown, true);
            }
        }

        for (size_t i = 0; i < DrawHudVerScalePattern.size(); i++)
        {
            if (i > 2 && i != 5 && i != 6 && i != 7 && i != 8 && i != 57 && i != 60 && i != 73) //0 1 2 - crosshair / 60 - subs
            {
                uint32_t* pCustomScaleVer = DrawHudVerScalePattern.get(i).get<uint32_t>(2);
                injector::WriteMemory(pCustomScaleVer, &fCustomWideScreenHeightScaleDown, true);
            }
        }

        auto pattern = hook::pattern("50 D8 0D ? ? ? ? D8 0D ? ? ? ? D9 1C 24 E8"); //0x57E954 radio text
        injector::WriteMemory(pattern.get(24).get<uint32_t>(3), &fWideScreenWidthScaleDown, true);
        //injector::WriteMemory(pattern.get(25).get<uint32_t>(3), &fWideScreenWidthScaleDown, true);
        injector::WriteMemory(pattern.get(27).get<uint32_t>(3), &fWideScreenWidthScaleDown, true);//0x595F45 replay

        pattern = hook::pattern("50 D8 0D ? ? ? ? D8 0D ? ? ? ? D9 1C 24 DB 05 ? ? ? ? 50"); //0x57E93E radio text
        injector::WriteMemory(pattern.get(16).get<uint32_t>(3), &fCustomWideScreenHeightScaleDown, true);
        injector::WriteMemory(pattern.get(18).get<uint32_t>(3), &fCustomWideScreenHeightScaleDown, true);//0x620C2F replay

        for (size_t i = 0; i < CDarkelDrawMessagesPattern.size(); i++)
        {
            uint32_t* p15625 = CDarkelDrawMessagesPattern.get(i).get<uint32_t>(2);
            injector::WriteMemory(p15625, &fCustomWideScreenWidthScaleDown, true);
        }

        for (size_t i = 0; i < CDarkelDrawMessagesPattern2.size(); i++)
        {
            uint32_t* p2232143 = CDarkelDrawMessagesPattern2.get(i).get<uint32_t>(2);
            injector::WriteMemory(p2232143, &fCustomWideScreenHeightScaleDown, true);
        }
    }

    bIVRadarScaling = iniReader.ReadInteger("MAIN", "IVRadarScaling", 0) != 0;
    fRadarWidthScale = iniReader.ReadFloat("MAIN", "RadarWidthScale", 0.0f); fRadarWidthScale == 0.0f ? fRadarWidthScale = 0.9f : fRadarWidthScale;
    if (fRadarWidthScale && !bIVRadarScaling)
    {
        uint32_t* p15625 = CRadarPattern.get(0).get<uint32_t>(2);
        injector::WriteMemory(p15625, &fCustomRadarWidthScale, true);

        injector::WriteMemory(DrawHudHorScalePattern.get(60).get<uint32_t>(2), &fCustomRadarWidthScale, true);

        /*auto pattern = hook::pattern("D8 0D ? ? ? ? DD DA D9 05 ? ? ? ? D8 CA"); //0x4C4200 + 0xF4 + 0x2 + 0x6
        injector::WriteMemory(pattern.get(2).get<uint32_t>(2), &fPlayerMarkerPos, true);

        pattern = hook::pattern("D8 0D ? ? ? ? DD DA D9 C0 D8 CA"); //0x4C5170 + 0x307 + 0x2 + 0x6
        injector::WriteMemory(pattern.get(1).get<uint32_t>(2), &fPlayerMarkerPos, true);*/
    }

    fSubtitlesScale = iniReader.ReadFloat("MAIN", "SubtitlesScale", 0.0f); fSubtitlesScale == 0.0f ? fSubtitlesScale = 0.8f : fSubtitlesScale;
    if (fSubtitlesScale)
    {
        auto pattern = hook::pattern("D8 0D ? ? ? ? D9 1C 24 DB 05 ? ? ? ? 50 D8 0D ? ? ? ? D8 0D ? ? ? ? D9 1C 24"); //0x5FDC7C
        injector::WriteMemory<float>(*pattern.get(12).get<uint32_t*>(2), 1.2f * fSubtitlesScale, true);
        pattern = hook::pattern("D8 0D ? ? ? ? D9 1C 24 E8 ? ? ? ? 59 59"); //0x5FDC80
        injector::WriteMemory<float>(*pattern.get(12).get<uint32_t*>(2), 0.47999999f * fSubtitlesScale, true);
    }

    bRestoreCutsceneFOV = iniReader.ReadInteger("MAIN", "RestoreCutsceneFOV", 1) != 0;
    fCarSpeedDependantFOV = iniReader.ReadFloat("MAIN", "CarSpeedDependantFOV", 0.0f);
    bDontTouchFOV = iniReader.ReadInteger("MAIN", "DontTouchFOV", 0) != 0;

    szForceAspectRatio = iniReader.ReadString("MAIN", "ForceAspectRatio", "auto");
    if (strncmp(szForceAspectRatio, "auto", 4) != 0)
    {
        AspectRatioWidth = std::stoi(szForceAspectRatio);
        AspectRatioHeight = std::stoi(strchr(szForceAspectRatio, ':') + 1);
        fCustomAspectRatioHor = static_cast<float>(AspectRatioWidth);
        fCustomAspectRatioVer = static_cast<float>(AspectRatioHeight);
    }

    bFOVControl = iniReader.ReadString("MAIN", "FOVControl", "1") != 0;
    auto pattern = hook::pattern("DE D9 DE D9 EB E9"); //0x513547 1.0
    if (bFOVControl && pattern.size() > 0)
    {
        FOVControl = pattern.get(0).get<uint32_t>(6);
        injector::WriteMemory<float>(FOVControl, 1.0f, true);
    }

    nHideAABug = iniReader.ReadInteger("MAIN", "HideAABug", 0);
    if (nHideAABug)
    {
        auto pattern = hook::pattern("E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? C3 00 00 00 00"); //0x48E0DC
        injector::MakeJMP(pattern.get(3).get<uint32_t>(15), Hide1pxAABug, true);
    }

    bSmartCutsceneBorders = iniReader.ReadInteger("MAIN", "SmartCutsceneBorders", 1) != 0;
    if (bSmartCutsceneBorders)
    {
        injector::MakeCALL(BordersPattern.get(11).get<uint32_t>(7), CCamera::DrawBordersForWideScreen); //0x4A61EE 0x54A223
        injector::MakeCALL(BordersPattern.get(17).get<uint32_t>(7), CCamera::DrawBordersForWideScreen);
    }

    ReplaceTextShadowWithOutline = iniReader.ReadInteger("MAIN", "ReplaceTextShadowWithOutline", 0);
    if (ReplaceTextShadowWithOutline)
    {
        auto pattern = hook::pattern("E8 ? ? ? ? 83 C4 0C 89 E9 FF 35 ? ? ? ? E8 ? ? ? ? 89 E9"); 
        auto dest = injector::GetBranchDestination(pattern.get(1).get<uint32_t>(0), true); //PrintString 500F50

        pattern = hook::pattern("66 C7 05 ? ? ? ? 00 00 59 C3 00 00 00 00 00 00");
        auto newPrintString = pattern.get(0).get<uint32_t>(11); //0x500C24

        injector::WriteMemory((uint32_t)newPrintString, 0x55575653, true); //push    ebx,esi,edi,ebp
        injector::WriteMemory((uint32_t)newPrintString + 4, 0xE938EC83, true); //sub     esp, 38h jmp
        injector::MakeRelativeOffset((uint32_t)newPrintString + 4 + 4, (uint32_t)dest + 7, 4, true); //500F57

        injector::MakeJMP(dest, newPrintString, true);
        hbPrintString2.fun = injector::MakeJMP(dest, PrintStringHook2).get();

        //Changing black color for necessary strings
        for (size_t i = 0; i < MenuPattern3.size(); i++)
        {
            if (i == 9 || i == 15 || i == 20 || i == 33 /*|| i == 36*/ ) // 9 - MAIN MENU strings, 15,21 - controls page key on hover, 33 - ammo, 36 - wanted stars;
            {
                auto pClr = MenuPattern3.get(i);
                injector::WriteMemory<uint8_t>(pClr.get<uint32_t>(1), 0x01, true);
                injector::WriteMemory<uint8_t>(pClr.get<uint32_t>(3), 0x01, true);
                injector::WriteMemory<uint8_t>(pClr.get<uint32_t>(5), 0x01, true);
            }
        }
        pattern = hook::pattern("52 8D 8C 24 34 01 00 00 50 E8 ? ? ? ? 8D 84 24 28 01 00 00 50");//48A03B
        injector::WriteMemory<uint8_t>(pattern.get(0).get<uint32_t>(0), 0x51, true); //edx -> ecx to randomize color(controls page key on hover)

        pattern = hook::pattern("68 FF 00 00 00 6A 00 6A 00 6A 00 E8 ? ? ? ? 8D 84 24 5C 04 00 00 50 E8 ? ? ? ? 59");//48A03B
        injector::WriteMemory(pattern.get(0).get<uint32_t>(1), 0x00000000, true); //subtitles shadow alpha

        pattern = hook::pattern("6A 28 6A 28 6A 28");//50A0C2
        injector::WriteMemory<uint8_t>(pattern.get(0).get<uint32_t>(1), 0x00, true);
        injector::WriteMemory<uint8_t>(pattern.get(0).get<uint32_t>(3), 0x00, true);
        injector::WriteMemory<uint8_t>(pattern.get(0).get<uint32_t>(5), 0x00, true); //mission text shadow color


        pattern = hook::pattern("6A 32 6A 64 6A 64 6A 64");//0x47ACB7
        injector::WriteMemory<uint8_t>(pattern.get(1).get<uint32_t>(1), 0x00, true); // cursor shadow alpha
        //pattern = hook::pattern("E8 ? ? ? ? E8 ? ? ? ? D9 05 ? ? ? ? D8 4C 24 04 D9 3C 24");
        //injector::WriteMemory(pattern.get(0).get<uint32_t>(1), 0xFFFF885F, true); //0x50948D text box background disable

        //0x508363 pager?
        //0x592BD8 with backgr?
    }

    pattern = hook::pattern("A1 ? ? ? ? 3B C3"); //0x5B7D75
    if (pattern.size() > 0)
    {
        szForceAspectRatio = iniReader.ReadString("MAIN", "ForceMultisamplingLevel", "");
        if (strncmp(szForceAspectRatio, "max", 3) != 0)
        {
            SelectedMultisamplingLevels = iniReader.ReadInteger("MAIN", "ForceMultisamplingLevel", 0);
            injector::WriteMemory(pattern.get(0).get<uint32_t>(1), &SelectedMultisamplingLevels, true);
        }
        else
        {
            auto pattern2 = hook::pattern("89 1D ? ? ? ? 89 1D ? ? ? ? BE 02 00 00 00 EB 05"); //0x5B728F
            injector::WriteMemory(pattern.get(0).get<uint32_t>(1), *pattern2.get(0).get<uint32_t*>(2), true);
        }
    }

    if (iniReader.ReadInteger("MAIN", "FixVehicleLights", 1))
    {
        pattern = hook::pattern("D9 C2 DE CB D9 C2 DE CB D9 05");
        injector::WriteMemory<float>(*pattern.get(0).get<uint32_t*>(10), 2.0f, true); //car lights stretch 0x4F949C
    }

    if (bIVRadarScaling)
    {
        fCustomRadarPosXIV = 40.0f + 31.0f;
        auto pattern = hook::pattern("D8 05 ? ? ? ? DE C1 D9 19 8B 15 ? ? ? ? 89 14 24");
        injector::WriteMemory<float>(*pattern.get(0).get<uint32_t*>(2), fCustomRadarPosXIV, true); //0x4A5075
        fCustomRadarWidthIV = 94.0f - 5.5f;
        pattern = hook::pattern("D8 0D ? ? ? ? DD DA D9 00 D8 CA");
        injector::WriteMemory<float>(*pattern.get(0).get<uint32_t*>(2), fCustomRadarWidthIV, true); //0x4A505B

        fCustomRadarPosYIV = 116.0f - 7.5f;
        pattern = hook::pattern("D9 05 ? ? ? ? 89 14 24 D8 CB DA 2C 24 DE C1 DE E1");
        injector::WriteMemory<float>(*pattern.get(0).get<uint32_t*>(2), fCustomRadarPosYIV, true); //0x4A50B0
        fCustomRadarHeightIV = 76.0f + 5.0f;
        pattern = hook::pattern("D9 05 ? ? ? ? D8 C9 DD DA D9 40 04 D8 CA");
        injector::WriteMemory<float>(*pattern.get(0).get<uint32_t*>(2), fCustomRadarHeightIV, true); //0x4A5093

        fCustomRadarRingPosXIV = 34.0f + 31.0f;
        fCustomRadarRingPosXIV2 = fCustomRadarRingPosXIV + 6.0f;
        pattern = hook::pattern("FF 35 ? ? ? ? DD D8 E8 ? ? ? ? B9 ? ? ? ? 50");
        injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &fCustomRadarRingPosXIV, true); //0x50845F
        pattern = hook::pattern("D8 05 ? ? ? ? D8 05 ? ? ? ? D9 1C 24 D9 C0 D8 25 ? ? ? ? 50");
        injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &fCustomRadarRingPosXIV2, true); //0x508481

        fCustomRadarRingWidthIV = 94.0f - 5.5f;
        pattern = hook::pattern("D8 0D ? ? ? ? D8 05 ? ? ? ? D8 05");
        injector::WriteMemory<float>(*pattern.get(0).get<uint32_t*>(2), fCustomRadarRingWidthIV, true); //0x5FDC70

        fCustomRadarRingPosYIV = 116.0f - 7.5f;
        pattern = hook::pattern("D9 05 ? ? ? ? D8 CA DA 6C 24 34 DD DA D8 C1");
        injector::WriteMemory<float>(*pattern.get(0).get<uint32_t*>(2), fCustomRadarRingPosYIV); //0x508433

        fCustomRadarRingHeightIV = 76.0f + 5.0f;
        pattern = hook::pattern("D9 05 ? ? ? ? D8 C9 D9 05 ? ? ? ? D8 CA DA 6C 24 34");
        injector::WriteMemory<float>(*pattern.get(0).get<uint32_t*>(2), fCustomRadarRingHeightIV, true); //0x5FDC68
    }

    /*bSmallerTextShadows = iniReader.ReadInteger("MAIN", "SmallerTextShadows", 1) != 0;
    if (bSmallerTextShadows || ReplaceTextShadowWithOutline)
    {

    }*/
}

injector::hook_back<void(__cdecl*)(CRect&, CRGBA const&, CRGBA const&, CRGBA const&, CRGBA const&, unsigned int)> hbSetVertices;
static void __cdecl SetVerticesHook(CRect& a1, CRGBA const& a2, CRGBA const& a3, CRGBA const& a4, CRGBA const& a5, unsigned int a6)
{
    if (static_cast<int>(a1.m_fRight) == RsGlobal->MaximumWidth && static_cast<int>(a1.m_fBottom) == RsGlobal->MaximumHeight)
    {
        float fMiddleScrCoord = (float)RsGlobal->MaximumWidth / 2.0f;
        
        a1.m_fTop = 0.0f;
        a1.m_fLeft = fMiddleScrCoord - ((((float)RsGlobal->MaximumHeight * ((float)FrontendAspectRatioWidth / (float)FrontendAspectRatioHeight))) / 2.0f);
        a1.m_fBottom = (float)RsGlobal->MaximumHeight;
        a1.m_fRight = fMiddleScrCoord + ((((float)RsGlobal->MaximumHeight * ((float)FrontendAspectRatioWidth / (float)FrontendAspectRatioHeight))) / 2.0f);

        CSprite2dDrawRect(CRect(-5.0f, a1.m_fBottom, a1.m_fLeft, -5.0f), CRGBA(0, 0, 0, a2.alpha));
        CSprite2dDrawRect(CRect((float)RsGlobal->MaximumWidth, a1.m_fBottom, a1.m_fRight, -5.0f), CRGBA(0, 0, 0, a2.alpha));
    }

    return hbSetVertices.fun(a1, a2, a3, a4, a5, a6);
}

void Fix2DSprites()
{
    CIniReader iniReader("");
    szForceAspectRatio = iniReader.ReadString("MAIN", "FrontendAspectRatio", "auto");
    if (strncmp(szForceAspectRatio, "auto", 4) != 0)
    {
        FrontendAspectRatioWidth = std::stoi(szForceAspectRatio);
        FrontendAspectRatioHeight = std::stoi(strchr(szForceAspectRatio, ':') + 1);
    }
    else
    {
        FrontendAspectRatioWidth = 16;
        FrontendAspectRatioHeight = 9;
    }

    auto pattern = hook::pattern("E8 ? ? ? ? 89 D9 83 C4 18 E8 ? ? ? ? 6A 04"); //0x51ED62
    hbSetVertices.fun = injector::MakeCALL(pattern.get(1).get<uint32_t>(0), SetVerticesHook).get();
    injector::MakeCALL(pattern.get(2).get<uint32_t>(0), SetVerticesHook); //0x51EE0E
}

DWORD WINAPI Init(LPVOID)
{
    #ifdef _LOG
    logfile.open("GTAIII.WidescreenFix.log");
    #endif // _LOG

    auto pattern = hook::pattern("6A 02 6A 00 6A 00 68 01 20 00 00");
    if (!(pattern.size() > 0) && !bDelay)
    {
        bDelay = true;
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Init, NULL, 0, NULL);
        return 0;
    }

    if (bDelay)
    {
        while (!(pattern.size() > 0))
            pattern = hook::pattern("6A 02 6A 00 6A 00 68 01 20 00 00");
    }

    //Immediate changes
    GetPatterns();
    OverwriteResolution();
    GetMemoryAddresses();
    FixAspectRatio();
    FixFOV();
    RsSelectDeviceHook();
    FixMenu();
    FixCoronas();
    FixBorders();
    FixHUD();
    FixCrosshair();
    ApplyIniOptions();
    Fix2DSprites();

    //Delayed changes
    struct LoadState
    {
        void operator()(injector::reg_pack& regs) 
        {  
            *dwGameLoadState = 9;
            fCrosshairPosFactor = ((0.52999997f - 0.5f) / ((*CDraw::pfScreenAspectRatio) / (16.0f / 9.0f))) + 0.5f;
            fCrosshairHeightScaleDown = fWideScreenWidthScaleDown * *CDraw::pfScreenAspectRatio;

            fWideScreenHeightScaleDown = 1.0f / 480.0f;
            fCustomWideScreenWidthScaleDown = fWideScreenWidthScaleDown * fHudWidthScale;
            fCustomWideScreenHeightScaleDown = fWideScreenHeightScaleDown * fHudHeightScale;

            fCustomRadarWidthScale = fWideScreenWidthScaleDown * fRadarWidthScale;
            fPlayerMarkerPos = 94.0f * fRadarWidthScale;
            if (bIVRadarScaling)
                fPlayerMarkerPos = (94.0f - 5.5f) * fRadarWidthScale;

            //SilentPatchCompatibility(); not needed atm

            //WIDESCREEN to BORDERS text
            auto pattern = hook::pattern("B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? C2 04 00");
            auto GetTextCall = pattern.get(0).get<uint32_t>(10);
            auto GetText = injector::GetBranchDestination(GetTextCall, true).as_int();
            auto pfGetText = (wchar_t *(__thiscall *)(int, char *))GetText;
            auto TheText = *pattern.get(0).get<uint32_t*>(1);

            wchar_t* ptr = pfGetText((int)TheText, "FED_WIS");
            wcscpy(ptr, L"BORDERS");
        }
    }; injector::MakeInline<LoadState>(dwGameLoadStatePattern.get(0).get<uint32_t>(0), dwGameLoadStatePattern.get(0).get<uint32_t>(10));
    return 0;
}


BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        Init(NULL);
    }
    return TRUE;
}

