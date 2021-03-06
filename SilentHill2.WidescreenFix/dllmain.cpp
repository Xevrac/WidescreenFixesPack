#include "..\includes\stdafx.h"
#include "..\includes\hooking\Hooking.Patterns.h"
#include <algorithm>
#include <fstream>
HWND hWnd;
bool bDelay;

struct Screen
{
	int Width;
	int Height;
	float fWidth;
	float fHeight;
	float fFieldOfView;
	float fAspectRatio;
	float fHudOffset;
	int Width43;
} Screen;

DWORD WINAPI Init(LPVOID)
{
	auto pattern = hook::pattern("6A 70 68 ? ? ? ? E8 ? ? ? ? 33 DB 53 8B 3D ? ? ? ? FF D7");
	if (!(pattern.size() > 0) && !bDelay)
	{
		bDelay = true;
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Init, NULL, 0, NULL);
		return 0;
	}

	if (bDelay)
	{
		while (!(pattern.size() > 0))
			pattern = hook::pattern("6A 70 68 ? ? ? ? E8 ? ? ? ? 33 DB 53 8B 3D ? ? ? ? FF D7");
	}

	CIniReader iniReader("");
	Screen.Width = iniReader.ReadInteger("MAIN", "ResX", 0);
	Screen.Height = iniReader.ReadInteger("MAIN", "ResY", 0);
	bool bDisableCutsceneBorders = iniReader.ReadInteger("MISC", "DisableCutsceneBorders", 1) != 0;
	bool bSingleCoreAffinity = iniReader.ReadInteger("MISC", "SingleCoreAffinity", 1) != 0;
	bool bDisableSafeMode = iniReader.ReadInteger("MISC", "DisableSafeMode", 1) != 0;
	bool bFastTransitions = iniReader.ReadInteger("MISC", "FastTransitions", 1) != 0;
	bool bCreateLocalFix = iniReader.ReadInteger("MISC", "CreateLocalFix", 1) != 0;
	uint32_t nFPSLimit = iniReader.ReadInteger("MISC", "FPSLimit", 1);

	if (!Screen.Width || !Screen.Height) {
		HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &info);
		Screen.Width = info.rcMonitor.right - info.rcMonitor.left;
		Screen.Height = info.rcMonitor.bottom - info.rcMonitor.top;
	}

	Screen.fWidth = static_cast<float>(Screen.Width);
	Screen.fHeight = static_cast<float>(Screen.Height);
	Screen.fAspectRatio = (Screen.fWidth / Screen.fHeight);
	Screen.fHudOffset = (0.5f / ((4.0f / 3.0f) / (Screen.fAspectRatio)));
	Screen.Width43 = static_cast<uint32_t>(Screen.fHeight * (4.0f / 3.0f));

	auto ResList = *hook::pattern("3B 93 ? ? ? ? 75 ? 8B 44 24 14 3B 83").get(0).get<uint32_t*>(2); //4F60A8
	for (uint32_t i = (uint32_t)ResList; i <= (uint32_t)ResList + 0x2C; i += 4)
	{
		injector::WriteMemory(i, Screen.Width, true);
		i += 4;
		injector::WriteMemory(i, Screen.Height, true);
	}
	
	//2D
	static auto dword_A37080 = *hook::pattern("DB 05 ? ? ? ? 8B 0D ? ? ? ? 85 C9 50").get(0).get<uint32_t*>(2);

	char pattern_str[20];
	union {
		uint32_t* Int;
		unsigned char Byte[4];
	} dword_temp;

	dword_temp.Int = dword_A37080;
	sprintf(pattern_str, "%02X %02X %02X %02X %02X %02X", 0xDB, 0x05, dword_temp.Byte[0], dword_temp.Byte[1], dword_temp.Byte[2], dword_temp.Byte[3]);
	pattern = hook::pattern(pattern_str);
	for (size_t i = 0; i < pattern.size(); ++i) //http://pastebin.com/ZpkGX9My
	{
		if (i == 318 || i == 315 || i == 314 || i == 313 || i == 310 || i == 312 || i == 311 || i == 320 || i == 178 || i == 177 || i == 176 || i == 175 || i == 174 || i == 327 || i == 2 || i == 309
		|| i == 173 || i == 317 || i == 316 || i == 332 || i == 330)
		{
			injector::WriteMemory(pattern.get(i).get<uint32_t>(2), &Screen.Width, true);
		}
	}
	pattern = hook::pattern("A1 ? ? ? ? 8B 0D ? ? ? ? 48 89 44 24 0C 89"); //00478A11
	injector::WriteMemory(pattern.get(0).get<uint32_t>(1), &Screen.Width, true);
	pattern = hook::pattern("8B 15 ? ? ? ? 50 52 6A 00 6A 00 33 C0 "); //00478A7F
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &Screen.Width, true);
	pattern = hook::pattern("8B 0D ? ? ? ? 50 51 56 6A 00 33 C0"); //00478ACE
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &Screen.Width, true);
	pattern = hook::pattern("A1 ? ? ? ? 8B 0D ? ? ? ? 48 51 50 53 50 33 C0"); //00477EBA
	injector::WriteMemory(pattern.get(0).get<uint32_t>(1), &Screen.Width, true);
	pattern = hook::pattern("8B 15 ? ? ? ? 52 C7 44 24 14 00 00 00 00"); //00477313
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &Screen.Width, true);
	pattern = hook::pattern("8B 15 ? ? ? ? D1 EA 85 D2 89 54 24 04"); //00512067 flashlight
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &Screen.Width, true);
	pattern = hook::pattern("D9 05 ? ? ? ? D8 0D ? ? ? ? D9 1D ? ? ? ? D9 05 ? ? ? ? D8 0D ? ? ? ? D9 1D ? ? ? ? D9 05 ? ? ? ? D8 35"); //005106DE flashlight
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &Screen.fWidth, true);
	pattern = hook::pattern("DB 05 ? ? ? ? 85 C0 7D 06 D8 05 ? ? ? ? 8B 0D ? ? ? ? D9 1D ? ? ? ? DB 05 ? ? ? ?"); //00510681 flashlight
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &Screen.Width43, true);
	pattern = hook::pattern("8B 0D ? ? ? ? D1 E9 85 C9 89 4C 24 10 DB 44 24 10"); //004DC103 The light from the lighthouse  (i == 330)
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &Screen.Width, true);

	pattern = hook::pattern("A3 ? ? ? ? 89 44 24 14 A1 ? ? ? ? 8D 4C 24 0C 51");
	struct SetScaleHook
	{
		void operator()(injector::reg_pack& regs)
		{
			*dword_A37080 = Screen.Width43;
		}
	}; injector::MakeInline<SetScaleHook>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(5)); //4F5FFD

	pattern = hook::pattern("89 15 ? ? ? ? C6 05 ? ? ? ? 01 33 C0 C3");
	struct SetScaleHook2
	{
		void operator()(injector::reg_pack& regs)
		{
			*dword_A37080 = Screen.Width43;
		}
	}; injector::MakeInline<SetScaleHook2>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(6)); //4F661E

	pattern = hook::pattern("A1 ? ? ? ? 8B 0D ? ? ? ? BD 01 00 00 00");
	injector::WriteMemory(pattern.get(0).get<uint32_t>(1), &Screen.Width, true); //0x4F6672 + 1
	pattern = hook::pattern("DB 05 ? ? ? ? A1 ? ? ? ? 85 C0 7D 06");
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &Screen.Width, true); //0x479BC2 + 2 stretching

	dword_temp.Int = *hook::pattern("D8 0D ? ? ? ? DE C1 DA 44 24 08 59 C3").get(0).get<uint32_t*>(2);
	sprintf(pattern_str, "%02X %02X %02X %02X %02X %02X", 0xD8, 0x0D, dword_temp.Byte[0], dword_temp.Byte[1], dword_temp.Byte[2], dword_temp.Byte[3]);
	pattern = hook::pattern(pattern_str); //0x630DEC fmul

	//http://pastebin.com/Lqg7hYsW
	int f05Indices[] = { 29,31,33,35,37,39,41,43,45,47,49,51,53,55,57,59,61,63,65,67,75,77,79,81,83,84,85,86,87,88,89,91,93,95,97,99,
		101,103,105,107,109,111,113,120,122,124,126,128,130,132,134,136,138,140,142,144,146,148,150,152,154,156,158,160,162,164,166,168,
		170,172,174,176,178,180,182,184,186,188,190,192,194,196,198,200,202,204,206,208,210,212,214,216,218,220,222,224,226,228,230,232,234,
		236,238,240,242,244,246,248,250,252,254,256,258,260,262,264,266,268,270,272,274,276,278,280,282,284,286,288,290,292,294,296,298,300,302,
		304,306,308,310,312,314,316,318,320,322,324,326,328,330,332,334,336,338,340,342,344,346,348,350,352,354,356,370,372,386,388,390,392,394,396,
		398,400,402,404,406,408,410,412,414,416,418,420,422,424,426,428,430,432,435,437,439,441,443,445,447,449,451,453,455,457,459,461,463,469,471,473,
		475,477,479,481,483,485,487,489,491,493,495,497,499,501,503,505,507,509,511,513,515,517,519,521,523,525,527,529,531,533,535,541,543,549,551,553,555,
		557,559,561,563,565,567,569,571,574,576,578,580,582,584,586,588,590,592,594,596,598,600,602,604,606,608,628,630,632,634,682, 610, 612 ,614, 616, 618, 620, 622, 624, 626 };

	for (size_t i = 0; i < pattern.size(); ++i)
	{
		//logfile << std::dec << i << " " << std::hex << pattern.get(i).get<uint32_t>(2) << std::endl;
		if (!(std::end(f05Indices) == std::find(std::begin(f05Indices), std::end(f05Indices), i)))
		{
			injector::WriteMemory(pattern.get(i).get<uint32_t>(2), &Screen.fHudOffset, true);
		}
	}
	pattern = hook::pattern("D9 05 ? ? ? ? 0F BF 46 0C D8 C9 89 54 24 00");
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &Screen.fHudOffset, true); //49F2C0

	//2D
	pattern = hook::pattern("D8 0D ? ? ? ? D8 44 24 0C D8 44 24 18 D8 25 ? ? ? ? D9 54 24 1C");
	float offset = (((Screen.fWidth - Screen.fHeight * (4.0f / 3.0f)) / 2.0f) / (2.0f / (Screen.fAspectRatio / (4.0f / 3.0f)))) / (Screen.fWidth);
	static float f_05 = -0.5f + offset;
	static float f05 = 0.5f + offset;
	injector::WriteMemory(pattern.get(1).get<uint32_t>(2), &f_05, true); //0x049F5B8
	pattern = hook::pattern("D9 05 ? ? ? ? 0F BE 15 ? ? ? ? D8 C9 A1 ? ? ? ? 85 C0");
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &f05, true);  //0x049F592

	//Text pos
	static float tempvar1, tempvar2, tempvar3, tempvar4, tempvar5, tempvar6, tempvar7, tempvar8, tempvar9;
	static float TextOffset = (Screen.fWidth - Screen.fHeight * (4.0f / 3.0f)) / 2.0f;
	pattern = hook::pattern("89 54 24 04 0F B7 50 ? D8 0D ? ? ? ? 42 D8 C2");
	struct TextPosHook
	{
		void operator()(injector::reg_pack& regs)
		{
			*(uintptr_t*)(regs.esp + 0x4) = regs.edx;
			regs.edx = *(uint16_t*)(regs.eax + 0xA);

			_asm FSTP DWORD PTR[tempvar1]
			_asm FSTP DWORD PTR[tempvar2]
			_asm FSTP DWORD PTR[tempvar3]
			tempvar3 += TextOffset;
			_asm FLD  DWORD PTR[tempvar3]
			_asm FLD  DWORD PTR[tempvar2]
			_asm FLD  DWORD PTR[tempvar1]
		}
	}; injector::MakeInline<TextPosHook>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(8)); //0x4819C7, 0x4819C7+8 | sub_4818D0+75 +

	pattern = hook::pattern("D9 1D ? ? ? ? 8B 08 FF 91");
	static auto flt_807498 = *hook::pattern("D9 15 ? ? ? ? DB 05 ? ? ? ? 7D 06 D8 05").get(0).get<uint32_t*>(2);
	static auto flt_8074A8 = (uint32_t)flt_807498 + 0x10;
	static auto flt_8074B8 = (uint32_t)flt_807498 + 0x20;
	static auto flt_8074C8 = (uint32_t)flt_807498 + 0x30;
	static auto flt_8074D8 = (uint32_t)flt_807498 + 0x40;
	static auto flt_8074E8 = (uint32_t)flt_807498 + 0x50;
	static float temp = 0.0f;
	struct TextPosHook2
	{
		void operator()(injector::reg_pack& regs)
		{
			_asm FSTP DWORD PTR[temp]
			*(float*)flt_807498 += TextOffset;
			*(float*)flt_8074C8 += TextOffset;
			*(float*)flt_8074E8 = temp + TextOffset;
			*(float*)flt_8074A8 += TextOffset;
			*(float*)flt_8074B8 += TextOffset;
			*(float*)flt_8074D8 += TextOffset;
		}
	}; injector::MakeInline<TextPosHook2>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(6)); //481343 | sub_481210+41 | sub_481210+B3 +

	pattern = hook::pattern("89 94 24 A8 00 00 00 DB 44 24 18 89 94 24 D0");
	struct TextPosHook3
	{
		void operator()(injector::reg_pack& regs)
		{
			*(uintptr_t*)(regs.esp + 0xA8) = regs.edx;

			_asm FSTP DWORD PTR[tempvar4]
			tempvar4 += TextOffset;
			_asm FLD  DWORD PTR[tempvar4]

			*(float*)(regs.esp + 0x6C + 4) += TextOffset;
		}
	}; injector::MakeInline<TextPosHook3>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(7)); //0x481E70 | sub_481D20+119+


	pattern = hook::pattern("D9 94 24 90 00 00 00 D9 C1 D9 9C 24 94 00 00 00");
	struct TextPosHook4
	{
		void operator()(injector::reg_pack& regs)
		{
			_asm FSTP DWORD PTR[tempvar5]
			tempvar5 += TextOffset;
			_asm FLD  DWORD PTR[tempvar5]

			float esp90 = 0.0f;
			_asm FST DWORD PTR[esp90]
			*(float*)(regs.esp + 0x90) = esp90;
		}
	}; injector::MakeInline<TextPosHook4>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(7)); //481F72 | sub_481D20+23E+


	pattern = hook::pattern("DB 05 ? ? ? ? 7D 06 D8 05 ? ? ? ? 0F BE 0D ? ? ? ? 8B");
	struct TextPosHook5
	{
		void operator()(injector::reg_pack& regs)
		{
			*(float*)(regs.esp + 0x30) += TextOffset;

			_asm FILD DWORD PTR[Screen.Height] //A37084
		}
	}; injector::MakeInline<TextPosHook5>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(6)); //482054 | sub_481D20+31C+


	pattern = hook::pattern("D9 54 24 58 D9 C9 D9 5C 24 5C D9 5C 24 6C DB 44 24 24");
	struct TextPosHook6
	{
		void operator()(injector::reg_pack& regs)
		{
			_asm FSTP DWORD PTR[tempvar6]
			tempvar6 += TextOffset;
			_asm FLD  DWORD PTR[tempvar6]

			uint32_t esp58 = (regs.esp + 0x58);
			_asm FST DWORD PTR[esp58]
			_asm fxch    st(1)
		}
	}; injector::MakeInline<TextPosHook6>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(6)); //482117 | sub_481D20+3E3+


	pattern = hook::pattern("DB 05 ? ? ? ? 7D ? D8 05 ? ? ? ? D9 5C 24 20 0F BE 15");
	struct TextPosHook7
	{
		void operator()(injector::reg_pack& regs)
		{
			//_asm FSTP DWORD PTR[tempvar7]
			//tempvar7 += TextOffset;
			//_asm FLD  DWORD PTR[tempvar7]
			//*(float*)(regs.esp + 0x2C) += TextOffset;
			//_asm FILD DWORD PTR[A37084]
		}
	}; //injector::MakeInline<TextPosHook7>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(6)); //482249 | sub_482160+D3 


	pattern = hook::pattern("D9 54 24 40 D9 C1 D9 5C 24 44 D9 CA");
	struct TextPosHook8
	{
		void operator()(injector::reg_pack& regs)
		{
			//uint32_t esp40 = (regs.esp + 0x40);
			//_asm FSTP DWORD PTR[tempvar8]
			//tempvar8 += TextOffset;
			//_asm FLD  DWORD PTR[tempvar8]
			//_asm FST DWORD PTR[esp40]
			//_asm fld     st(1)			
		}
	}; //injector::MakeInline<TextPosHook8>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(6)); //482348 | sub_482160+1D6


	pattern = hook::pattern("A1 ? ? ? ? 8B 10 6A 14 8D 4C 24 30 51 6A 03 6A 02 50");
	static auto dword_A36494 = *pattern.get(0).get<uint32_t*>(1);
	struct TextPosHook9
	{
		void operator()(injector::reg_pack& regs)
		{
			//regs.eax = *dword_A36494;
			//*(float*)(regs.esp + 0x7C) += TextOffset;
			//*(float*)(regs.esp + 0x90) += TextOffset;
		}
	}; //injector::MakeInline<TextPosHook9>(pattern.get(0).get<uint32_t>(0), pattern.get(0).get<uint32_t>(5)); //4823D9 | sub_482160+230 | sub_482160+257


	//Cursor
	pattern = hook::pattern("D8 0D ? ? ? ? C7 44 24 3C 0A D7 23 3C");
	injector::WriteMemory<float>(*pattern.get(0).get<uint32_t*>(2), 0.05859375f * (1.0f / (Screen.fAspectRatio / (4.0f / 3.0f))), true);

	//FMV
	pattern = hook::pattern("A1 ? ? ? ? D9 15 ? ? ? ? D9 C2 89 15 ? ? ? ? D9 1D");
	injector::WriteMemory(pattern.get(0).get<uint32_t>(1), &TextOffset, true); //0x0043E4D8
	pattern = hook::pattern("D8 25 ? ? ? ? 8B 0D ? ? ? ? 85 C9 8B 15");
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &TextOffset, true); //0x0043E4C5
	pattern = hook::pattern("8B 15 ? ? ? ? A1 ? ? ? ? 89 15 ? ? ? ? A3");
	injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &TextOffset, true); //0x0043E47F

	if (bDisableCutsceneBorders)
	{
		static float f0 = 0.0f;
		static float f1 = 1.0f;
		pattern = hook::pattern("D8 0D ? ? ? ? DE C1 DA 44 24 ? E8");
		injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &f0, true); //00478A6E
		injector::WriteMemory(pattern.get(1).get<uint32_t>(2), &f1, true); //00478ABD
	}

	if (bSingleCoreAffinity)
	{
		SetProcessAffinityMask(GetCurrentProcess(), 1);
	}

	if (bDisableSafeMode)
	{
		pattern = hook::pattern("74 05 E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 08");
		injector::WriteMemory<uint8_t>(pattern.get(0).get<uint32_t>(0), 0xEB, true);
	}

	if (bFastTransitions)
	{
		pattern = hook::pattern("8B 15 ? ? ? ? 6A 15 52 8B 15 ? ? ? ? 52");
		static uint32_t n640 = 640;
		static uint32_t n480 = 480;
		injector::WriteMemory(pattern.get(0).get<uint32_t>(2), &n640, true);
		injector::WriteMemory(pattern.get(0).get<uint32_t>(11), &n480, true);
	}

	if (bCreateLocalFix)
	{
		char moduleName[MAX_PATH];
		GetModuleFileName(NULL, moduleName, MAX_PATH);
		char* tempPointer = strrchr(moduleName, '\\');
		*(tempPointer + 1) = '\0';
		strcat(moduleName, "local.fix");

		auto FileExists = [](LPCTSTR szPath) -> BOOL 
		{
			DWORD dwAttrib = GetFileAttributes(szPath);
			return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
		};

		if (!FileExists(moduleName))
		{
			std::ofstream local_fix;
			local_fix.open(moduleName);
			local_fix << "HACK DX_CONFIG_DRIVER_AA_NOT_BROKEN 1";
			local_fix.close();
		}
	}

	if (nFPSLimit)
	{
		pattern = hook::pattern("6A 00 6A 1E 50 51");
		injector::WriteMemory<uint8_t>(pattern.get(0).get<uint32_t>(3), nFPSLimit, true); //004F6F53
	}

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
