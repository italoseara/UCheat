// https://github.com/Coopyy/Unturned-External-Base
// Rewritten by Ítalo Seara (https://github.com/italoseara)

#include <iostream>
#include "game.hpp"

using namespace std;

int main()
{
    Memory::init();
    Classes::init();
    Offsets::init();

    if (!SDG::Provider::isConnected())
    {
        log("It seems like you're not in a server yet, run again when you are");
        system("pause");
        return 0;
    }

    auto local_player = SDG::Player::getPlayer();
    if (!local_player)
    {
        log("Couldn't find local player");
        system("pause");
        return 0;
    }

    log("Local Player: %p", local_player);
    printf("\n");

    // iterate players
    auto clients = SDG::Provider::getClients();
    int count = clients->count();
    log("Connected players: %d", count);
    for (uint32_t i = 0; i < count; i++)
    {
        auto steam_player = clients->get(i);

        // player info
        auto player_id = steam_player->getInfo();
        auto steam_name = player_id->getSteamName()->toString();
        auto public_name = player_id->getPublicName()->toString();
        auto steam_id = player_id->getSteamId();

        auto player = steam_player->getPlayer();
        auto player_life = player->getLife();
        auto player_pos = player->getGameObject()->transform().localPosition();

        log("Player %d: %s (SteamID: %llu) - %f %f %f", i, steam_name.c_str(), steam_id, player_pos.x, player_pos.y, player_pos.z);
        log("Health: %d, Stamina: %d, Food: %d, Water: %d", player_life->getHealth(), player_life->getStamina(), player_life->getFood(), player_life->getWater());
        printf("\n");
    }

    // iterate zombies
    auto player_pos = local_player->getGameObject()->transform().localPosition();
    auto regions = SDG::ZombieManager::getRegions()->toVector();

    for (auto region : regions)
    {
        auto zombies = region->getZombies()->toVector();
        for (auto zombie : zombies)
        {
            // zombie info
            auto id = zombie->getId();
            auto is_alive = zombie->isDead() ? "No" : "Yes";
            auto health = zombie->getHealth();
            auto max_health = zombie->getMaxHealth();
            auto pos = zombie->getGameObject()->transform().localPosition();

            log("Zombie %d - isAlive: %s - Health: %d/%d - Pos: %f %f %f",
                id, is_alive, health, max_health, pos.x, pos.y, pos.z);
        }
    }

    // admin stuff
    auto owner = local_player->getChannel()->getOwner();
    owner->setAdmin(1);
    if (owner->isAdmin())
        log("Use keys Shift + F1 & F7 for freecam & esp");

    // gun mods
    auto equipment = local_player->getEquipment();
    auto useable = (SDG::UseableGun*)equipment->getUseable();
    auto gun = (SDG::ItemGunAsset*)equipment->getAsset();

    if (gun && useable)
    {
        log("Current Weapon: %s (%i)", gun->getName()->toString().c_str(), gun->getId());
        log("Current Ammo: %d", useable->getAmmo());
        useable->setAmmo(100);
        log("Set current weapon ammo to 100");
        gun->setRecoilMaxX(0);
        gun->setRecoilMaxY(0);
        gun->setRecoilMinX(0);
        gun->setRecoilMinY(0);
        gun->setBaseSpreadAngle(0);
        log("Set current weapon recoil and spread to 0");
    }
    else
    {
        log("Couldn't find current weapon");
    }

    system("pause");
}