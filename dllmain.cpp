// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "csgo.hpp"
#include <Windows.h>

using namespace hazedumper::netvars;
using namespace hazedumper::signatures;

struct Vec3
{
    float x, y, z;

    Vec3 operator+(Vec3 d)
    {
        return { x + d.x, y + d.y, z + d.z };
    }

    Vec3 operator-(Vec3 d)
    {
        return { x - d.x, y - d.y, z - d.z };
    }

    Vec3 operator*(float d)
    {
        return { x * d, y * d, z * d };
    }

    void Normalized()
    {
        while (y < 180) { y += 360; }
        while (y > -180) { y -= 360; }
        if (x > 89) { x = 89; }
        if (x < -89) { x = -89; }
    }
};

DWORD WINAPI HackThread(HMODULE hModule)
{
    uintptr_t clientModule = (uintptr_t)GetModuleHandle(L"client.dll");
    uintptr_t engineModule = (uintptr_t)GetModuleHandle(L"engine.dll");

    uintptr_t localPlayer = *(uintptr_t*)(clientModule + dwLocalPlayer);
    Vec3* viewAngels = (Vec3*)(*(uintptr_t*)(engineModule + dwClientState) + dwClientState_ViewAngles);
    int* iShotsFired = (int*)(localPlayer + m_iShotsFired);
    Vec3* aimPunchAngle = (Vec3*)(localPlayer + m_aimPunchAngle);
    Vec3 oPunch{ 0,0,0 };

    while (!GetAsyncKeyState(VK_ESCAPE)) 
    {
        Vec3 punchAngle = *aimPunchAngle * 2;
        if (*iShotsFired > 1)
        {
            Vec3 newAngle = *viewAngels + oPunch - punchAngle;
            newAngle.Normalized();
            *viewAngels = newAngle;
        }
        oPunch = punchAngle;
    }

    FreeLibraryAndExitThread(hModule, 0);
    CloseHandle(hModule);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0);
        if (hThread) CloseHandle(hThread);
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

