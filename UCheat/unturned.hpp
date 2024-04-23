#ifndef UNTURNED_HPP
#define UNTURNED_HPP

#include "mono.hpp"

#define FIELD_DEF(return_type, field_name, offset)                  \
	return_type field_name()                                        \
	{                                                               \
		return Memory::read<return_type>(THISPTR + offset);         \
	}                                                               \
	void set_##field_name(return_type value)                        \
	{                                                               \
		return Memory::write<return_type>(value, THISPTR + offset); \
	}

#define STATIC_FIELD_DEF(return_type, field_name, offset)              \
	static return_type field_name()                                    \
	{                                                                  \
		return Memory::read<return_type>(instance() + offset);         \
	}                                                                  \
	static void set_##field_name(return_type value)                    \
	{                                                                  \
		return Memory::write<return_type>(value, instance() + offset); \
	}

#define INSTANCE_DEF(class_name)                                                             \
	static inline uintptr_t instance()                                                       \
	{                                                                                        \
		static uintptr_t inst = 0;                                                           \
		if (!inst)                                                                           \
			inst = class_name->get_vtable(Mono::get_root_domain())->get_static_field_data(); \
		return inst;                                                                         \
	}

#define GAMEOBJECT_DEF()                                                                                    \
	Unity::GameObject *game_object()                                                                         \
	{                                                                                                       \
		return (Unity::GameObject*)Memory::read<uintptr_t>(Memory::read<uintptr_t>(THISPTR + 0x10) + 0x30); \
	}

#define GET_OFFSET(class_name, field_name) \
	class_name->find_field(field_name)->offset()

namespace Classes
{
	mono_class_t* Provider;
	mono_class_t* SteamChannel;
	mono_class_t* SteamPlayer;
	mono_class_t* SteamPlayerID;

	mono_class_t* Player;
	mono_class_t* PlayerLife;
	mono_class_t* PlayerEquipment;

	mono_class_t* Asset;
	mono_class_t* ItemGunAsset;
	mono_class_t* Useable;
	mono_class_t* UseableGun;

	void init()
	{
		Provider        = Mono::find_class("Assembly-CSharp", "SDG.Unturned.Provider");
		SteamChannel = Mono::find_class("Assembly-CSharp", "SDG.Unturned.SteamChannel");
		SteamPlayer = Mono::find_class("Assembly-CSharp", "SDG.Unturned.SteamPlayer");
		SteamPlayerID = Mono::find_class("Assembly-CSharp", "SDG.Unturned.SteamPlayerID");

		Player          = Mono::find_class("Assembly-CSharp", "SDG.Unturned.Player");
		PlayerLife      = Mono::find_class("Assembly-CSharp", "SDG.Unturned.PlayerLife");
		PlayerEquipment = Mono::find_class("Assembly-CSharp", "SDG.Unturned.PlayerEquipment");

		Asset           = Mono::find_class("Assembly-CSharp", "SDG.Unturned.Asset");
		ItemGunAsset    = Mono::find_class("Assembly-CSharp", "SDG.Unturned.ItemGunAsset");
		Useable         = Mono::find_class("Assembly-CSharp", "SDG.Unturned.Useable");
		UseableGun      = Mono::find_class("Assembly-CSharp", "SDG.Unturned.UseableGun");
	}
}

namespace Offsets
{
	namespace Provider
	{
		uintptr_t is_connected;
		uintptr_t is_loading_ugc;
		uintptr_t clients;
	}

	namespace SteamChannel
	{
		uintptr_t owner;
	}

	namespace SteamPlayer
	{
		uintptr_t is_admin;
		uintptr_t joined;
		uintptr_t player;
		uintptr_t info;
	}

	namespace SteamPlayerID
	{
		uintptr_t private_name;
		uintptr_t steam_name;
		uintptr_t public_name;
		uintptr_t steam_id;
	}

	namespace Player
	{
		uintptr_t player;
		uintptr_t channel;
		uintptr_t equipment;
		uintptr_t life;
	}

	namespace PlayerLife
	{
		uintptr_t is_dead;
		uintptr_t is_bleeding;
		uintptr_t is_broken;
		uintptr_t health;
		uintptr_t food;
		uintptr_t water;
		uintptr_t virus;
		uintptr_t stamina;
		uintptr_t oxygen;
	}

	namespace PlayerEquipment
	{
		uintptr_t asset;
		uintptr_t useable;
	}

	namespace Asset
	{
		uintptr_t id;
		uintptr_t name;
	}

	namespace ItemGunAsset
	{
		uintptr_t id;
		uintptr_t recoil_min_x;
		uintptr_t recoil_max_x;
		uintptr_t recoil_min_y;
		uintptr_t recoil_max_y;
		uintptr_t base_spread_angle_radians;
	}

	namespace UseableGun
	{
		uintptr_t ammo;
	}

	void init()
	{	
		Provider::is_connected      = GET_OFFSET(Classes::Provider, "_isConnected");
		Provider::is_loading_ugc = GET_OFFSET(Classes::Provider, "isLoadingUGC");
		Provider::clients        = GET_OFFSET(Classes::Provider, "_clients");

		SteamChannel::owner = GET_OFFSET(Classes::SteamChannel, "owner");

		SteamPlayer::is_admin     = GET_OFFSET(Classes::SteamPlayer, "_isAdmin");
		SteamPlayer::joined    = GET_OFFSET(Classes::SteamPlayer, "_joined");
		SteamPlayer::player    = GET_OFFSET(Classes::SteamPlayer, "_player");
		SteamPlayer::info      = GET_OFFSET(Classes::SteamPlayer, "_playerID");

		SteamPlayerID::steam_name   = GET_OFFSET(Classes::SteamPlayerID, "_playerName");
		SteamPlayerID::public_name  = GET_OFFSET(Classes::SteamPlayerID, "_characterName");
		SteamPlayerID::private_name = GET_OFFSET(Classes::SteamPlayerID, "_nickName");

		Player::player    = GET_OFFSET(Classes::Player, "_player");
		Player::channel   = GET_OFFSET(Classes::Player, "_channel");
		Player::equipment = GET_OFFSET(Classes::Player, "_equipment");
		Player::life      = GET_OFFSET(Classes::Player, "_life");

		PlayerLife::is_dead     = GET_OFFSET(Classes::PlayerLife, "_isDead");
		PlayerLife::is_bleeding = GET_OFFSET(Classes::PlayerLife, "_isBleeding");
		PlayerLife::is_broken   = GET_OFFSET(Classes::PlayerLife, "_isBroken");
		PlayerLife::health      = GET_OFFSET(Classes::PlayerLife, "_health");
		PlayerLife::food        = GET_OFFSET(Classes::PlayerLife, "_food");
		PlayerLife::water       = GET_OFFSET(Classes::PlayerLife, "_water");
		PlayerLife::virus       = GET_OFFSET(Classes::PlayerLife, "_virus");
		PlayerLife::stamina     = GET_OFFSET(Classes::PlayerLife, "_stamina");
		PlayerLife::oxygen      = GET_OFFSET(Classes::PlayerLife, "_oxygen");

		PlayerEquipment::asset   = GET_OFFSET(Classes::PlayerEquipment, "_asset");
		PlayerEquipment::useable = GET_OFFSET(Classes::PlayerEquipment, "_useable");

		Asset::id   = GET_OFFSET(Classes::Asset, "id");
		Asset::name = GET_OFFSET(Classes::Asset, "name");

		ItemGunAsset::recoil_min_x              = GET_OFFSET(Classes::ItemGunAsset, "recoilMin_x");
		ItemGunAsset::recoil_max_x              = GET_OFFSET(Classes::ItemGunAsset, "recoilMax_x");
		ItemGunAsset::recoil_min_y              = GET_OFFSET(Classes::ItemGunAsset, "recoilMin_y");
		ItemGunAsset::recoil_max_y              = GET_OFFSET(Classes::ItemGunAsset, "recoilMax_y");
		ItemGunAsset::base_spread_angle_radians = GET_OFFSET(Classes::ItemGunAsset, "<baseSpreadAngleRadians>k__BackingField");

		UseableGun::ammo = GET_OFFSET(Classes::UseableGun, "ammo");
	}
}

#endif // UNTURNED_HPP