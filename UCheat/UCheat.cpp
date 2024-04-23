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

    if (!SDG::Provider::is_connected())
    {
        log("It seems like you're not in a server yet, run again when you are");
        system("pause");
        return 0;
    }

    auto local_player = SDG::Player::player();
    if (!local_player)
    {
        log("Couldn't find local player");
        system("pause");
        return 0;
    }

    log("Local Player: %p", local_player);
    printf("\n");

    // iterate players
    auto clients = SDG::Provider::clients();
    int count = clients->count();
    log("Connected players: %d", count);
    for (uint32_t i = 0; i < count; i++)
    {
        auto steam_player = clients->get(i);
        auto player = steam_player->player();
        log("Player %d: %p", i, player);
    }
    printf("\n");

    // admin stuff
    auto owner = local_player->channel()->owner();
    owner->set_admin(1); //write
    if (owner->admin()) // read
        log("Use keys Shift + F1 & F7 for freecam & esp");

    // gun mods
    auto equipment = local_player->equipment();
    auto useable = (SDG::UseableGun*) equipment->useable();
    auto gun = (SDG::ItemGunAsset*) equipment->asset();

    if (gun && useable)
    {
        log("Current Weapon: %s (%i)", gun->name()->str().c_str(), gun->id());
        log("Current Ammo: %d", useable->ammo());
        gun->set_recoil_max_x(0);
        gun->set_recoil_max_y(0);
        gun->set_recoil_min_x(0);
        gun->set_recoil_min_y(0);
        gun->set_base_spread_angle_radians(0);
        log("Set current weapon recoil and spread to 0");
    }
    else
    {
        log("Couldn't find current weapon");
    }

    system("pause");
}
