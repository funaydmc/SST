#include <pch.h>
#include "main.h"
#include <cstdio>
#include <appdata/il2cpp-init.h>

#include "CheatMenu.h"

#include "cheat/NoCD.h"
#include "cheat/GodMode.h"

#include "cheat/misc/About.h"

void OpenConsole()
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

	auto consoleWindow = GetConsoleWindow();
	SetForegroundWindow(consoleWindow);
	ShowWindow(consoleWindow, SW_RESTORE);
	ShowWindow(consoleWindow, SW_SHOW);
}

void Run(HMODULE* phModule) {
	global::process::hModule = *phModule;
	OpenConsole();
	LOG_INFO("Injected successfully!");

	uintptr_t qwGameAssembly;
	while ((qwGameAssembly = (uintptr_t)GetModuleHandle("GameAssembly.dll")) == 0) {
		LOG_INFO("GameAssembly isn't initialized...\n");
		Sleep(2000);
	}

	LOG_DEBUG("GameAssembly found at: 0x%p", (uintptr_t*)qwGameAssembly);

	constexpr int delayInit = 5000;
	constexpr int delaySecInit = delayInit / 1000;
	LOG_DEBUG("Waiting %d sec for game initialize.", delaySecInit);
	Sleep(delayInit);

	init_il2cpp();

	auto& menu = cheat::CheatMenu::GetInstance();
	menu.AddFeatures({
		&cheat::feature::GodMode::GetInstance(),
		&cheat::feature::NoCD::GetInstance(),
		&cheat::feature::About::GetInstance()
	});
	menu.Init();
}