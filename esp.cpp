#include "esp.hpp"
#include "../../math.hpp"
#include "../../offsets/offsets.hpp"
#include "../../offsets/client_dll.hpp"
#include "../thirdparty/imgui/imgui.h"
#include <libloaderapi.h>

uintptr_t modules::client = (uintptr_t)GetModuleHandleA("client.dll");
uintptr_t modules::engine2 = (uintptr_t)GetModuleHandleA("engine2.dll");

void ESP::RenderESP()
{
	float(*ViewMatrix)[4][4] = (float(*)[4][4])(modules::client + cs2_dumper::offsets::client_dll::dwViewMatrix);

	auto localPlayerPawn = *(uintptr_t*)(modules::client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);
	if (!localPlayerPawn) {
		return;
	}
	auto localTeam = *(int*)(localPlayerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);

	auto entityList = *(uintptr_t*)(modules::client + cs2_dumper::offsets::client_dll::dwEntityList);
	if (!entityList) {
		return;
	}

	for (int i = 1; i < 64; i++) {
		uintptr_t listEntry = *(uintptr_t*)(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
		if (!listEntry) {
			continue;
		}

		uintptr_t playerController = *(uintptr_t*)(listEntry + 112 * (i & 0x1FF));
		if (!playerController) {
			continue;
		}

		uint32_t playerPawn = *(uint32_t*)(playerController + cs2_dumper::schemas::client_dll::CCSPlayerController::m_hPlayerPawn);
		if (!playerPawn) {
			continue;
		}

		uintptr_t listEntry2 = *(uintptr_t*)(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
		if (!listEntry2) {
			continue;
		}

		uintptr_t pCSPlayerPawnPointer = *(uintptr_t*)(listEntry2 + 112 * (playerPawn & 0x1FF));
		if (!pCSPlayerPawnPointer) {
			continue;
		}

		int health = *(int*)(pCSPlayerPawnPointer + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
		if (!health || health > 100) {
			continue;
		}

		int team = *(int*)(pCSPlayerPawnPointer + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
		if (team == localTeam) {
			continue;
		}

		uintptr_t collision = *(uintptr_t*)(pCSPlayerPawnPointer + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_Collision);
		Vec3 min = *(Vec3*)(collision + 0x40);
		Vec3 max = *(Vec3*)(collision + 0x4C);

		Vec3 feetPos = *(Vec3*)(pCSPlayerPawnPointer + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
		Vec3 headPos = { feetPos.x, feetPos.y, feetPos.z + max.z };
		feetPos = { feetPos.x, feetPos.y, feetPos.z + min.z };

		Vec2 feet, head;

		if (feetPos.WorldToScreen(feet, ViewMatrix) && headPos.WorldToScreen(head, ViewMatrix)) {
			auto draw = ImGui::GetBackgroundDrawList();

			float height = (feet.y - head.y);
			float width = height / 2.0f;
			float x = head.x - (width / 2);
			float y = head.y;

			draw->AddRect({ x, y }, { x + width, y + height }, IM_COL32(255, 255, 255, 255), 0.0f, 0, 1.0f);
		}
    }
}