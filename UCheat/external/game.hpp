#ifndef GAME_HPP
#define GAME_HPP

#include "unity.hpp"
#include "unturned.hpp"

namespace SDG
{
	class Provider;
	class SteamChannel;
	class SteamPlayer;
	class SteamPlayerID;

	class Player;
	class PlayerLife;
	class PlayerEquipment;

	class ZombieManager;
	class ZombieRegion;
	class Zombie;
	
	class Asset;
	class ItemGunAsset;
	class Useable;
	class UseableGun;

	class Provider
	{
	private:
		INSTANCE_DEF(Classes::Provider);
	public:
		STATIC_GETTER_DEF(bool, is_connected, Offsets::Provider::is_connected);
		STATIC_GETTER_DEF(Unity::List<SteamPlayer*>*, clients, Offsets::Provider::clients);
	};

	class SteamChannel
	{
	public:
		GETTER_DEF(SteamPlayer*, owner, Offsets::SteamChannel::owner);
	};

	class SteamPlayer
	{
	public:
		GETTER_DEF(Player*, player, Offsets::SteamPlayer::player);
		GETTER_DEF(float, joined, Offsets::SteamPlayer::joined)
		GETTER_SETTER_DEF(bool, is_admin, Offsets::SteamPlayer::is_admin);
		GETTER_DEF(SteamPlayerID*, info, Offsets::SteamPlayer::info);
	};

	class SteamPlayerID
	{
	public:
		GETTER_DEF(Unity::String*, steam_name, Offsets::SteamPlayerID::steam_name);
		GETTER_DEF(Unity::String*, public_name, Offsets::SteamPlayerID::public_name);
		GETTER_DEF(Unity::String*, private_name, Offsets::SteamPlayerID::private_name);
		GETTER_DEF(uint64_t, steam_id, Offsets::SteamPlayerID::steam_id);
	};

	class Player
	{
	private:
		INSTANCE_DEF(Classes::Player);
	public:
		STATIC_GETTER_DEF(Player*, player, Offsets::Player::player);
		GETTER_DEF(PlayerEquipment*, equipment, Offsets::Player::equipment);
		GETTER_DEF(SteamChannel*, channel, Offsets::Player::channel);
		GETTER_DEF(PlayerLife*, life, Offsets::Player::life);
		GAMEOBJECT_DEF();
	};

	class PlayerLife
	{
	public:
		GETTER_SETTER_DEF(bool, is_dead, Offsets::PlayerLife::is_dead);
		GETTER_SETTER_DEF(bool, is_bleeding, Offsets::PlayerLife::is_bleeding);
		GETTER_SETTER_DEF(bool, is_broken, Offsets::PlayerLife::is_broken);
		GETTER_SETTER_DEF(cs_byte, health, Offsets::PlayerLife::health);
		GETTER_SETTER_DEF(cs_byte, food, Offsets::PlayerLife::food);
		GETTER_SETTER_DEF(cs_byte, water, Offsets::PlayerLife::water);
		GETTER_SETTER_DEF(cs_byte, virus, Offsets::PlayerLife::virus);
		GETTER_SETTER_DEF(cs_byte, stamina, Offsets::PlayerLife::stamina);
		GETTER_SETTER_DEF(cs_byte, oxygen, Offsets::PlayerLife::oxygen);
	};

	class PlayerEquipment
	{
	public:
		GETTER_DEF(Asset*, asset, Offsets::PlayerEquipment::asset);
		GETTER_DEF(Useable*, useable, Offsets::PlayerEquipment::useable);
	};

	class ZombieManager
	{
	private:
		INSTANCE_DEF(Classes::ZombieManager);
	public:
		STATIC_GETTER_DEF(Unity::List<Zombie*>*, ticking_zombies, Offsets::ZombieManager::ticking_zombies);
		STATIC_GETTER_DEF(Unity::Array<ZombieRegion*>*, regions, Offsets::ZombieManager::regions);
	};

	class ZombieRegion
	{
	public:
		GETTER_DEF(Unity::List<Zombie*>*, zombies, Offsets::ZombieRegion::zombies);
	};

	class Zombie
	{
	private:
		GETTER_DEF(uintptr_t, eyes_ptr, Offsets::Zombie::eyes);
	public:
		GETTER_DEF(uint16_t, id, Offsets::Zombie::id);
		GETTER_SETTER_DEF(uint16_t, health, Offsets::Zombie::health);
		GETTER_SETTER_DEF(uint16_t, max_health, Offsets::Zombie::max_health);
		GETTER_SETTER_DEF(bool, is_dead, Offsets::Zombie::is_dead);
		GAMEOBJECT_DEF();

		Unity::Transform eyes() { return Unity::Transform(eyes_ptr()); }
	};

    class Asset
    {
    public:
		GETTER_DEF(uint16_t, id, Offsets::Asset::id);
		GETTER_SETTER_DEF(Unity::String*, name, Offsets::Asset::name);
    };

	class ItemGunAsset : public Asset
	{
	public:
		GETTER_SETTER_DEF(float, recoil_min_x, Offsets::ItemGunAsset::recoil_min_x);
		GETTER_SETTER_DEF(float, recoil_max_x, Offsets::ItemGunAsset::recoil_max_x);
		GETTER_SETTER_DEF(float, recoil_min_y, Offsets::ItemGunAsset::recoil_min_y);
		GETTER_SETTER_DEF(float, recoil_max_y, Offsets::ItemGunAsset::recoil_max_y);
		GETTER_SETTER_DEF(float, base_spread_angle_radians, Offsets::ItemGunAsset::base_spread_angle_radians);
	};

	class Useable
	{
	private:
	};

	class UseableGun : public Useable
	{
	public:
		GETTER_SETTER_DEF(cs_byte, ammo, Offsets::UseableGun::ammo);
	};
}

#endif // GAME_HPP
