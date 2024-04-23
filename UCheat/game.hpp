#ifndef GAME_HPP
#define GAME_HPP

#include "unturned.hpp"
#include "unity.hpp"

namespace SDG
{
	class Provider;
	class SteamChannel;
	class SteamPlayer;
	class SteamPlayerID;
	class Player;
	class PlayerLife;
	class PlayerEquipment;
	class Asset;
	class ItemGunAsset;
	class Useable;
	class UseableGun;

	class Provider
	{
	private:
		INSTANCE_DEF(Classes::Provider);
	public:
		STATIC_FIELD_DEF(bool, is_connected, Offsets::Provider::is_connected);
		STATIC_FIELD_DEF(Unity::List<SteamPlayer*>*, clients, Offsets::Provider::clients);
	};

	class SteamChannel
	{
	public:
		FIELD_DEF(SteamPlayer*, owner, Offsets::SteamChannel::owner);
	};

	class SteamPlayer
	{
	public:
		FIELD_DEF(Player*, player, Offsets::SteamPlayer::player);
		FIELD_DEF(float, joined, Offsets::SteamPlayer::joined)
		FIELD_DEF(bool, is_admin, Offsets::SteamPlayer::is_admin);
		FIELD_DEF(SteamPlayerID*, info, Offsets::SteamPlayer::info);
	};

	class SteamPlayerID
	{
	public:
		FIELD_DEF(Unity::String*, steam_name, Offsets::SteamPlayerID::steam_name);
		FIELD_DEF(Unity::String*, public_name, Offsets::SteamPlayerID::public_name);
		FIELD_DEF(Unity::String*, private_name, Offsets::SteamPlayerID::private_name);
		FIELD_DEF(uint64_t, steam_id, Offsets::SteamPlayerID::steam_id);
	};

	class Player
	{
	private:
		INSTANCE_DEF(Classes::Player);
	public:
		STATIC_FIELD_DEF(Player*, player, Offsets::Player::player);
		FIELD_DEF(PlayerEquipment*, equipment, Offsets::Player::equipment);
		FIELD_DEF(SteamChannel*, channel, Offsets::Player::channel);
		FIELD_DEF(PlayerLife*, life, Offsets::Player::life);
		GAMEOBJECT_DEF();
	};

	class PlayerLife
	{
	public:
		FIELD_DEF(bool, is_dead, Offsets::PlayerLife::is_dead);
		FIELD_DEF(bool, is_bleeding, Offsets::PlayerLife::is_bleeding);
		FIELD_DEF(bool, is_broken, Offsets::PlayerLife::is_broken);
		FIELD_DEF(byte, health, Offsets::PlayerLife::health);
		FIELD_DEF(byte, food, Offsets::PlayerLife::food);
		FIELD_DEF(byte, water, Offsets::PlayerLife::water);
		FIELD_DEF(byte, virus, Offsets::PlayerLife::virus);
		FIELD_DEF(byte, stamina, Offsets::PlayerLife::stamina);
		FIELD_DEF(byte, oxygen, Offsets::PlayerLife::oxygen);
	};

	class PlayerEquipment
	{
	public:
		FIELD_DEF(Asset*, asset, Offsets::PlayerEquipment::asset);
		FIELD_DEF(Useable*, useable, Offsets::PlayerEquipment::useable);
	};

    class Asset
    {
    public:
        FIELD_DEF(uint16_t, id, Offsets::Asset::id);
		FIELD_DEF(Unity::String*, name, Offsets::Asset::name);
    };

	class ItemGunAsset : public Asset
	{
	public:
		FIELD_DEF(float, recoil_min_x, Offsets::ItemGunAsset::recoil_min_x);
		FIELD_DEF(float, recoil_max_x, Offsets::ItemGunAsset::recoil_max_x);
		FIELD_DEF(float, recoil_min_y, Offsets::ItemGunAsset::recoil_min_y);
		FIELD_DEF(float, recoil_max_y, Offsets::ItemGunAsset::recoil_max_y);
		FIELD_DEF(float, base_spread_angle_radians, Offsets::ItemGunAsset::base_spread_angle_radians);
	};

	class Useable
	{
	private:
	};

	class UseableGun : public Useable
	{
	public:
		FIELD_DEF(byte, ammo, Offsets::UseableGun::ammo);
	};
}

#endif // GAME_HPP
