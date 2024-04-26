// Made by Ítalo Seara (https://github.com/italoseara)

#include "overlay.hpp"
#include "unity.hpp"
#include "unturned.hpp"
#include "game.hpp"

void drawCircle(Unity::Vector3 point)
{
    ImGui::GetBackgroundDrawList()->AddCircleFilled({ point.x, point.y }, 5, IM_COL32(255, 0, 0, 255));
}

void drawLoop()
{
    if (!SDG::Provider::isConnected()) return;

    auto local_player = SDG::Player::getPlayer();
    if (!local_player) return;

    auto playerPos = local_player->getGameObject()->transform().position();
    float fovX = SDG::OptionsSettings::getFov() * PI / 180.0f;

    auto mainCamera = SDG::MainCamera::getInstance();
    auto camTransform = mainCamera->getGameObject()->transform();
    auto camRotation = camTransform.rotation();
    auto camPosition = camTransform.position();
    
    auto zombieRegions = SDG::ZombieManager::getRegions()->toVector();
    for (auto region : zombieRegions)
	{
        auto zombies = region->getZombies()->toVector();
        for (auto zombie : zombies)
		{
            if (zombie->isDead()) continue;
			auto zombiePos = zombie->getGameObject()->transform().position();
            auto zombiePos2D = Unity::Camera::worldToScreen(camPosition, camRotation, fovX, Overlay::width, Overlay::height, zombiePos);
            if (zombiePos2D.z < 0) continue;

			drawCircle(zombiePos2D);
		}

	}
}

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT nCmdShow)
{
    Memory::init();
    Classes::init();
    Offsets::init();

    Overlay::Run(hInstance, nCmdShow, drawLoop);
    return 0;
}