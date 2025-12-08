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

		uintptr_t gameSceneNode = *(uintptr_t*)(pCSPlayerPawnPointer + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
		if (!gameSceneNode) {
			continue;
		}

		uintptr_t collision = *(uintptr_t*)(pCSPlayerPawnPointer + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_Collision);
		if (!collision)
		{
			continue;
		}

		Vec3 origin = *(Vec3*)(gameSceneNode + cs2_dumper::schemas::client_dll::CGameSceneNode::m_vecAbsOrigin);
		Vec3 min = *(Vec3*)(collision + cs2_dumper::schemas::client_dll::CCollisionProperty::m_vecMins);
		Vec3 max = *(Vec3*)(collision + cs2_dumper::schemas::client_dll::CCollisionProperty::m_vecMaxs);

		float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
		bool anyPointOnScreen = false;

		for (int i = 0; i < 8; i++)
		{
			Vec3 point;
			point.x = (i & 1) ? min.x : max.x;
			point.y = (i & 2) ? min.y : max.y;
			point.z = (i & 4) ? min.z : max.z;

			point = point + origin;

			Vec2 screenPoint;

			if (point.WorldToScreen(screenPoint, ViewMatrix))
			{
				anyPointOnScreen = true;
				if (minX > screenPoint.x)
					minX = screenPoint.x;
				if (minY > screenPoint.y)
					minY = screenPoint.y;
				if (maxX < screenPoint.x)
					maxX = screenPoint.x;
				if (maxY < screenPoint.y)
					maxY = screenPoint.y;
			}
		}

		if (anyPointOnScreen) {
			auto draw = ImGui::GetBackgroundDrawList();
			draw->AddRect({ minX, minY }, { maxX, maxY }, IM_COL32(255, 255, 255, 255));
		}
    }
}