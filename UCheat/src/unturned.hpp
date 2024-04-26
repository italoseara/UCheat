// Credits: https://github.com/Coopyy/Unturned-External-Base

#ifndef UNTURNED_HPP
#define UNTURNED_HPP

#include "mono.hpp"

#define PI 3.14159265358979323846f

#define GETTER_DEF(returnType, fieldName, offset)          \
	returnType get##fieldName()                            \
	{                                                      \
		return Memory::read<returnType>(THISPTR + offset); \
	}

#define GETTER_BOOL_DEF(fieldName, offset)                  \
	bool is##fieldName()                                    \
	{                                                       \
		return Memory::read<bool>(THISPTR + offset);        \
	}

#define SETTER_DEF(returnType, fieldName, offset)                  \
	void set##fieldName(returnType value)                          \
	{                                                              \
		return Memory::write<returnType>(value, THISPTR + offset); \
	}

#define GETTER_SETTER_DEF(returnType, fieldName, offset) \
	GETTER_DEF(returnType, fieldName, offset)            \
	SETTER_DEF(returnType, fieldName, offset)

#define GETTER_SETTER_BOOL_DEF(fieldName, offset) \
	GETTER_BOOL_DEF(fieldName, offset)            \
	SETTER_DEF(bool, fieldName, offset)

#define STATIC_GETTER_DEF(returnType, fieldName, offset)             \
	static returnType get##fieldName()                               \
	{                                                                \
		return Memory::read<returnType>(instance() + offset);        \
	}

#define STATIC_GETTER_BOOL_DEF(fieldName, offset)                    \
	static bool is##fieldName()                                      \
	{                                                                \
		return Memory::read<bool>(instance() + offset);             \
	}

#define STATIC_SETTER_DEF(returnType, fieldName, offset)              \
	static void set##fieldName(returnType value)                      \
	{                                                                 \
		return Memory::write<returnType>(value, instance() + offset); \
	}

#define STATIC_GETTER_SETTER_DEF(returnType, fieldName, offset) \
	STATIC_GETTER_DEF(returnType, fieldName, offset)            \
	STATIC_SETTER_DEF(returnType, fieldName, offset)

#define STATIC_GETTER_SETTER_BOOL_DEF(fieldName, offset) \
	STATIC_GETTER_BOOL_DEF(fieldName, offset)            \
	STATIC_SETTER_DEF(bool, fieldName, offset)

#define INSTANCE_DEF(className)                                                             \
	static inline uintptr_t instance()                                                      \
	{                                                                                       \
		static uintptr_t inst = 0;                                                          \
		if (!inst)                                                                          \
			inst = className->getVTable(Mono::getRootDomain())->getStaticFieldData();       \
		return inst;                                                                        \
	}

#define GAMEOBJECT_DEF()                                                                                    \
	Unity::GameObject *getGameObject()                                                                      \
	{                                                                                                       \
		return (Unity::GameObject*)Memory::read<uintptr_t>(Memory::read<uintptr_t>(THISPTR + 0x10) + 0x30); \
	}

#define GET_OFFSET(className, fieldName) \
	className->findField(fieldName)->offset()

namespace Classes
{
	MonoClass* Provider;
	MonoClass* SteamChannel;
	MonoClass* SteamPlayer;
	MonoClass* SteamPlayerID;

	MonoClass* Player;
	MonoClass* PlayerLife;
	MonoClass* PlayerEquipment;

	MonoClass* ZombieManager;
	MonoClass* ZombieRegion;
	MonoClass* Zombie;

	MonoClass* Asset;
	MonoClass* ItemGunAsset;
	MonoClass* Useable;
	MonoClass* UseableGun;

	MonoClass* MainCamera;

	MonoClass* OptionsSettings;

	void init()
	{
		// Steam
		Provider = Mono::findClass("Assembly-CSharp", "SDG.Unturned.Provider");
		SteamChannel = Mono::findClass("Assembly-CSharp", "SDG.Unturned.SteamChannel");
		SteamPlayer = Mono::findClass("Assembly-CSharp", "SDG.Unturned.SteamPlayer");
		SteamPlayerID = Mono::findClass("Assembly-CSharp", "SDG.Unturned.SteamPlayerID");

		// Player
		Player = Mono::findClass("Assembly-CSharp", "SDG.Unturned.Player");
		PlayerLife = Mono::findClass("Assembly-CSharp", "SDG.Unturned.PlayerLife");
		PlayerEquipment = Mono::findClass("Assembly-CSharp", "SDG.Unturned.PlayerEquipment");

		// Zombie
		ZombieManager = Mono::findClass("Assembly-CSharp", "SDG.Unturned.ZombieManager");
		ZombieRegion = Mono::findClass("Assembly-CSharp", "SDG.Unturned.ZombieRegion");
		Zombie = Mono::findClass("Assembly-CSharp", "SDG.Unturned.Zombie");

		// Assets
		Asset = Mono::findClass("Assembly-CSharp", "SDG.Unturned.Asset");
		ItemGunAsset = Mono::findClass("Assembly-CSharp", "SDG.Unturned.ItemGunAsset");
		Useable = Mono::findClass("Assembly-CSharp", "SDG.Unturned.Useable");
		UseableGun = Mono::findClass("Assembly-CSharp", "SDG.Unturned.UseableGun");

		// Camera
		MainCamera = Mono::findClass("Assembly-CSharp", "SDG.Unturned.MainCamera");

		// Options
		OptionsSettings = Mono::findClass("Assembly-CSharp", "SDG.Unturned.OptionsSettings");
	}
}

namespace Offsets
{
	namespace Provider
	{
		uintptr_t _isConnected;
		uintptr_t isLoadingUGC;
		uintptr_t _clients;
	}

	namespace SteamChannel
	{
		uintptr_t owner;
	}

	namespace SteamPlayer
	{
		uintptr_t _isAdmin;
		uintptr_t _joined;
		uintptr_t _player;
		uintptr_t _playerID;
	}

	namespace SteamPlayerID
	{
		uintptr_t _nickName;
		uintptr_t _playerName;
		uintptr_t _characterName;
		uintptr_t _steamID;
	}

	namespace Player
	{
		uintptr_t _player;
		uintptr_t _channel;
		uintptr_t _equipment;
		uintptr_t _life;
	}

	namespace PlayerLife
	{
		uintptr_t _isDead;
		uintptr_t _isBleeding;
		uintptr_t _isBroken;
		uintptr_t _health;
		uintptr_t _food;
		uintptr_t _water;
		uintptr_t _virus;
		uintptr_t _stamina;
		uintptr_t _oxygen;
	}

	namespace PlayerEquipment
	{
		uintptr_t _asset;
		uintptr_t _useable;
	}

	namespace ZombieManager
	{
		uintptr_t _tickingZombies;
		uintptr_t _regions;
	}

	namespace ZombieRegion
	{
		uintptr_t _zombies;
	}

	namespace Zombie
	{
		uintptr_t id;
		uintptr_t health;
		uintptr_t maxHealth;
		uintptr_t isDead;
	}

	namespace Asset
	{
		uintptr_t id;
		uintptr_t name;
	}

	namespace ItemGunAsset
	{
		uintptr_t id;
		uintptr_t recoilMin_x;
		uintptr_t recoilMax_x;
		uintptr_t recoilMin_y;
		uintptr_t recoilMax_y;
		uintptr_t baseSpreadAngleRadians;
	}

	namespace UseableGun
	{
		uintptr_t ammo;
	}

	namespace MainCamera
	{
		uintptr_t _instance;
	}

	namespace OptionsSettings
	{
		uintptr_t _fov;
	}

	void init()
	{
		Provider::_isConnected = GET_OFFSET(Classes::Provider, "_isConnected");
		Provider::isLoadingUGC = GET_OFFSET(Classes::Provider, "isLoadingUGC");
		Provider::_clients = GET_OFFSET(Classes::Provider, "_clients");

		SteamChannel::owner = GET_OFFSET(Classes::SteamChannel, "owner");

		SteamPlayer::_isAdmin = GET_OFFSET(Classes::SteamPlayer, "_isAdmin");
		SteamPlayer::_joined = GET_OFFSET(Classes::SteamPlayer, "_joined");
		SteamPlayer::_player = GET_OFFSET(Classes::SteamPlayer, "_player");
		SteamPlayer::_playerID = GET_OFFSET(Classes::SteamPlayer, "_playerID");

		SteamPlayerID::_playerName = GET_OFFSET(Classes::SteamPlayerID, "_playerName");
		SteamPlayerID::_characterName = GET_OFFSET(Classes::SteamPlayerID, "_characterName");
		SteamPlayerID::_nickName = GET_OFFSET(Classes::SteamPlayerID, "_nickName");
		SteamPlayerID::_steamID = GET_OFFSET(Classes::SteamPlayerID, "_steamID");

		Player::_player = GET_OFFSET(Classes::Player, "_player");
		Player::_channel = GET_OFFSET(Classes::Player, "_channel");
		Player::_equipment = GET_OFFSET(Classes::Player, "_equipment");
		Player::_life = GET_OFFSET(Classes::Player, "_life");

		PlayerLife::_isDead = GET_OFFSET(Classes::PlayerLife, "_isDead");
		PlayerLife::_isBleeding = GET_OFFSET(Classes::PlayerLife, "_isBleeding");
		PlayerLife::_isBroken = GET_OFFSET(Classes::PlayerLife, "_isBroken");
		PlayerLife::_health = GET_OFFSET(Classes::PlayerLife, "_health");
		PlayerLife::_food = GET_OFFSET(Classes::PlayerLife, "_food");
		PlayerLife::_water = GET_OFFSET(Classes::PlayerLife, "_water");
		PlayerLife::_virus = GET_OFFSET(Classes::PlayerLife, "_virus");
		PlayerLife::_stamina = GET_OFFSET(Classes::PlayerLife, "_stamina");
		PlayerLife::_oxygen = GET_OFFSET(Classes::PlayerLife, "_oxygen");

		PlayerEquipment::_asset = GET_OFFSET(Classes::PlayerEquipment, "_asset");
		PlayerEquipment::_useable = GET_OFFSET(Classes::PlayerEquipment, "_useable");

		ZombieManager::_tickingZombies = GET_OFFSET(Classes::ZombieManager, "_tickingZombies");
		ZombieManager::_regions = GET_OFFSET(Classes::ZombieManager, "_regions");

		ZombieRegion::_zombies = GET_OFFSET(Classes::ZombieRegion, "_zombies");

		Zombie::id = GET_OFFSET(Classes::Zombie, "id");
		Zombie::health = GET_OFFSET(Classes::Zombie, "health");
		Zombie::maxHealth = GET_OFFSET(Classes::Zombie, "maxHealth");
		Zombie::isDead = GET_OFFSET(Classes::Zombie, "isDead");

		Asset::id = GET_OFFSET(Classes::Asset, "id");
		Asset::name = GET_OFFSET(Classes::Asset, "name");

		ItemGunAsset::recoilMin_x = GET_OFFSET(Classes::ItemGunAsset, "recoilMin_x");
		ItemGunAsset::recoilMax_x = GET_OFFSET(Classes::ItemGunAsset, "recoilMax_x");
		ItemGunAsset::recoilMin_y = GET_OFFSET(Classes::ItemGunAsset, "recoilMin_y");
		ItemGunAsset::recoilMax_y = GET_OFFSET(Classes::ItemGunAsset, "recoilMax_y");
		ItemGunAsset::baseSpreadAngleRadians = GET_OFFSET(Classes::ItemGunAsset, "<baseSpreadAngleRadians>k__BackingField");

		UseableGun::ammo = GET_OFFSET(Classes::UseableGun, "ammo");

		MainCamera::_instance = GET_OFFSET(Classes::MainCamera, "_instance");

		OptionsSettings::_fov = GET_OFFSET(Classes::OptionsSettings, "_fov");
	}
}

#endif // UNTURNED_HPP