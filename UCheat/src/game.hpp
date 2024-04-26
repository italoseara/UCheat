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
		STATIC_GETTER_BOOL_DEF(Connected, Offsets::Provider::_isConnected);
		STATIC_GETTER_DEF(Unity::List<SteamPlayer*>*, Clients, Offsets::Provider::_clients);
	};

	class SteamChannel
	{
	public:
		GETTER_DEF(SteamPlayer*, Owner, Offsets::SteamChannel::owner);
	};

	class SteamPlayer
	{
	public:
		GETTER_DEF(Player*, Player, Offsets::SteamPlayer::_player);
		GETTER_DEF(float, Joined, Offsets::SteamPlayer::_joined)
			GETTER_SETTER_BOOL_DEF(Admin, Offsets::SteamPlayer::_isAdmin);
		GETTER_DEF(SteamPlayerID*, Info, Offsets::SteamPlayer::_playerID);
	};

	class SteamPlayerID
	{
	public:
		GETTER_DEF(Unity::String*, SteamName, Offsets::SteamPlayerID::_playerName);
		GETTER_DEF(Unity::String*, PublicName, Offsets::SteamPlayerID::_characterName);
		GETTER_DEF(Unity::String*, PrivateName, Offsets::SteamPlayerID::_nickName);
		GETTER_DEF(uint64_t, SteamId, Offsets::SteamPlayerID::_steamID);
	};

	class Player
	{
	private:
		INSTANCE_DEF(Classes::Player);
	public:
		STATIC_GETTER_DEF(Player*, Player, Offsets::Player::_player);
		GETTER_DEF(PlayerEquipment*, Equipment, Offsets::Player::_equipment);
		GETTER_DEF(SteamChannel*, Channel, Offsets::Player::_channel);
		GETTER_DEF(PlayerLife*, Life, Offsets::Player::_life);
		GAMEOBJECT_DEF();
	};

	class PlayerLife
	{
	public:
		GETTER_SETTER_BOOL_DEF(Dead, Offsets::PlayerLife::_isDead);
		GETTER_SETTER_BOOL_DEF(Bleeding, Offsets::PlayerLife::_isBleeding);
		GETTER_SETTER_BOOL_DEF(Broken, Offsets::PlayerLife::_isBroken);
		GETTER_SETTER_DEF(cs_byte, Health, Offsets::PlayerLife::_health);
		GETTER_SETTER_DEF(cs_byte, Food, Offsets::PlayerLife::_food);
		GETTER_SETTER_DEF(cs_byte, Water, Offsets::PlayerLife::_water);
		GETTER_SETTER_DEF(cs_byte, Virus, Offsets::PlayerLife::_virus);
		GETTER_SETTER_DEF(cs_byte, Stamina, Offsets::PlayerLife::_stamina);
		GETTER_SETTER_DEF(cs_byte, Oxygen, Offsets::PlayerLife::_oxygen);
	};

	class PlayerEquipment
	{
	public:
		GETTER_DEF(Asset*, Asset, Offsets::PlayerEquipment::_asset);
		GETTER_DEF(Useable*, Useable, Offsets::PlayerEquipment::_useable);
	};

	class ZombieManager
	{
	private:
		INSTANCE_DEF(Classes::ZombieManager);
	public:
		STATIC_GETTER_DEF(Unity::List<Zombie*>*, TickingZombies, Offsets::ZombieManager::_tickingZombies);
		STATIC_GETTER_DEF(Unity::Array<ZombieRegion*>*, Regions, Offsets::ZombieManager::_regions);
	};

	class ZombieRegion
	{
	public:
		GETTER_DEF(Unity::List<Zombie*>*, Zombies, Offsets::ZombieRegion::_zombies);
	};

	class Zombie
	{
	public:
		GETTER_DEF(uint16_t, Id, Offsets::Zombie::id);
		GETTER_SETTER_DEF(uint16_t, Health, Offsets::Zombie::health);
		GETTER_SETTER_DEF(uint16_t, MaxHealth, Offsets::Zombie::maxHealth);
		GETTER_SETTER_BOOL_DEF(Dead, Offsets::Zombie::isDead);
		GAMEOBJECT_DEF();
	};

	class Asset
	{
	public:
		GETTER_DEF(uint16_t, Id, Offsets::Asset::id);
		GETTER_SETTER_DEF(Unity::String*, Name, Offsets::Asset::name);
	};

	class ItemGunAsset : public Asset
	{
	public:
		GETTER_SETTER_DEF(float, RecoilMinX, Offsets::ItemGunAsset::recoilMin_x);
		GETTER_SETTER_DEF(float, RecoilMaxX, Offsets::ItemGunAsset::recoilMax_x);
		GETTER_SETTER_DEF(float, RecoilMinY, Offsets::ItemGunAsset::recoilMin_y);
		GETTER_SETTER_DEF(float, RecoilMaxY, Offsets::ItemGunAsset::recoilMax_y);
		GETTER_SETTER_DEF(float, BaseSpreadAngle, Offsets::ItemGunAsset::baseSpreadAngleRadians);
	};

	class Useable
	{
	private:
	};

	class UseableGun : public Useable
	{
	public:
		GETTER_SETTER_DEF(cs_byte, Ammo, Offsets::UseableGun::ammo);
	};

	class MainCamera
	{
	private:
		INSTANCE_DEF(Classes::MainCamera);
	public:
		STATIC_GETTER_DEF(Unity::Camera*, Instance, Offsets::MainCamera::_instance);
	};

	class OptionsSettings
	{
	private:
		INSTANCE_DEF(Classes::OptionsSettings);
	public:
		static float getFov() {
			return 60 + Memory::read<float>(instance() + Offsets::OptionsSettings::_fov) * 40;
		};
	};
}

#endif // GAME_HPP
