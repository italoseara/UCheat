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
	Unity::GameObject *gameObject()                                                                         \
	{                                                                                                       \
		return (Unity::GameObject*)Memory::read<uintptr_t>(Memory::read<uintptr_t>(THISPTR + 0x10) + 0x30); \
	}

#define GET_OFFSET(class_name, field_name) \
	class_name->find_field(field_name)->offset()

#define FIND_CLASS(class_name) \
	Mono::find_class("Assembly-CSharp", class_name)

namespace Classes
{
	mono_class_t* Provider;
	mono_class_t* Player;

	mono_class_t* SteamChannel;
	mono_class_t* SteamPlayer;

	mono_class_t* PlayerEquipment;
	mono_class_t* Asset;
	mono_class_t* ItemGunAsset;
	mono_class_t* Useable;
	mono_class_t* UseableGun;

	void init()
	{
		Provider        = FIND_CLASS("SDG.Unturned.Provider");

		Player          = FIND_CLASS("SDG.Unturned.Player");
		SteamChannel    = FIND_CLASS("SDG.Unturned.SteamChannel");
		SteamPlayer     = FIND_CLASS("SDG.Unturned.SteamPlayer");

		PlayerEquipment = FIND_CLASS("SDG.Unturned.PlayerEquipment");
		Asset           = FIND_CLASS("SDG.Unturned.Asset");
		ItemGunAsset    = FIND_CLASS("SDG.Unturned.ItemGunAsset");
		Useable         = FIND_CLASS("SDG.Unturned.Useable");
		UseableGun      = FIND_CLASS("SDG.Unturned.UseableGun");
	}
}

namespace Offsets
{
	namespace Player
	{
		uintptr_t player;
		uintptr_t channel;
		uintptr_t equipment;
	}

	namespace Provider
	{
		uintptr_t is_connected;
		uintptr_t is_loading_ugc;
		uintptr_t clients;
	}

	namespace SteamPlayer
	{
		uintptr_t is_admin;
		uintptr_t joined;
		uintptr_t player;
	}

	namespace SteamChannel
	{
		uintptr_t owner;
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
		Player::player    = GET_OFFSET(Classes::Player, "_player");
		Player::channel   = GET_OFFSET(Classes::Player, "_channel");
		Player::equipment = GET_OFFSET(Classes::Player, "_equipment");
			
		Provider::is_connected   = GET_OFFSET(Classes::Provider, "_isConnected");
		Provider::is_loading_ugc = GET_OFFSET(Classes::Provider, "isLoadingUGC");
		Provider::clients        = GET_OFFSET(Classes::Provider, "_clients");

		SteamPlayer::is_admin = GET_OFFSET(Classes::SteamPlayer, "_isAdmin");
		SteamPlayer::joined   = GET_OFFSET(Classes::SteamPlayer, "_joined");
		SteamPlayer::player   = GET_OFFSET(Classes::SteamPlayer, "_player");

		SteamChannel::owner = GET_OFFSET(Classes::SteamChannel, "owner");

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