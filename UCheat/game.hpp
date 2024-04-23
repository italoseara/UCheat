#ifndef GAME_HPP
#define GAME_HPP

#include "unturned.hpp"
#include "unity.hpp"

namespace SDG
{
	class SteamPlayer;
	class SteamChannel;
	class SteamPlayerID;
	class Player;
	class Provider;
	class PlayerEquipment;
	class Asset;
	class ItemGunAsset;
	class Useable;

	class Provider
	{
	private:
		INSTANCE_DEF(Classes::Provider);
	public:
		STATIC_FIELD_DEF(bool, connected, Offsets::Provider::connected);
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
		FIELD_DEF(bool, admin, Offsets::SteamPlayer::admin);
		FIELD_DEF(SteamPlayerID*, info, Offsets::SteamPlayer::info);
	};

	class SteamPlayerID
	{
	public:
		FIELD_DEF(Unity::String*, steam_name, Offsets::SteamPlayerID::steam_name);
		FIELD_DEF(Unity::String*, public_name, Offsets::SteamPlayerID::public_name);
		FIELD_DEF(Unity::String*, private_name, Offsets::SteamPlayerID::private_name);
	};

	class Player
	{
	private:
		INSTANCE_DEF(Classes::Player);
	public:
		STATIC_FIELD_DEF(Player*, player, Offsets::Player::player);
		FIELD_DEF(PlayerEquipment*, equipment, Offsets::Player::equipment);
		FIELD_DEF(SteamChannel*, channel, Offsets::Player::channel);
		GAMEOBJECT_DEF();
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
