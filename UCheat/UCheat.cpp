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

        // player info
        auto player_id = steam_player->info();
        auto steam_name = player_id->steam_name()->to_string();
        auto public_name = player_id->public_name()->to_string();
        auto steam_id = player_id->steam_id()->id();

        auto player = steam_player->player();
        auto player_life = player->life();
        auto player_pos = player->game_object()->transform()->local_position();

        log("Player %d: %s (%llu) - %s", i, steam_name.c_str(), steam_id, player_pos.to_string().c_str());
        log("Health: %d, Stamina: %d, Food: %d, Water: %d", player_life->health(), player_life->stamina(), player_life->food(), player_life->water());
        printf("\n");
    }

    // admin stuff
    auto owner = local_player->channel()->owner();
    owner->set_is_admin(1);
    if (owner->is_admin())
        log("Use keys Shift + F1 & F7 for freecam & esp");

    // gun mods
    auto equipment = local_player->equipment();
    auto useable = (SDG::UseableGun*) equipment->useable();
    auto gun = (SDG::ItemGunAsset*) equipment->asset();

    if (gun && useable)
    {
        log("Current Weapon: %s (%i)", gun->name()->to_string().c_str(), gun->id());
        log("Current Ammo: %d", useable->ammo());
        useable->set_ammo(100);
        log("Set current weapon ammo to 100");
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
