#include "weapon_modular.h"
#include "weapon_definitions.h"

static ModularWeaponInfo kAK47Info = {
    "weapon_ak47",                // classname
    "762",                        // ammoType
    "models/v_ak47.mdl",          // viewModel
    "models/p_ak47.mdl",          // playerModel
    "models/w_ak47.mdl",          // worldModel
    30, 90,                       // maxClip, maxAmmo
    0.0955f, 36.0f, 1.2f,         // fireRate, damage, recoil
    Vector(0.02f, 0.02f, 0.02f),  // vecSpread
    0.75f, 1.5f,                  // spreadCrouch, spreadMove
    false, 1,                     // isShotgun, pelletsCount
    BULLET_PLAYER_762,           // bulletType
    "weapons/ak47_1.wav",         // soundFire
    "weapons/dryfirevalue.wav",   // soundEmpty
    1, 1,                         // slot, position
    "ak47", WEAPON_AK47,                  // animExtension, weaponId
    0, 1, 2,                      // animIdle (0), animReload (1), animDraw (2)
    3, 5,                         // animFireMin (3), animFireMax (5)
    3.08f, "models/rshell.mdl"           // shellModel
};

class CAK47 : public CBaseModularWeapon {
public:
    void Spawn() override {
        SetWeaponInfo(kAK47Info);
        CBaseModularWeapon::Spawn();
    }

    void Precache() override {
        SetWeaponInfo(kAK47Info);
        CBaseModularWeapon::Precache();
    }
};

class CAK47Ammo : public CBasePlayerAmmo {
public:
    void Spawn() override {
        SET_MODEL(ENT(pev), "models/w_weaponbox.mdl");
        CBasePlayerAmmo::Spawn();
    }

    void Precache() override {
        PRECACHE_MODEL("models/w_weaponbox.mdl");
        PRECACHE_SOUND("items/9mmclip1.wav");
    }

    BOOL AddAmmo(CBaseEntity *pOther) override {
        if (pOther->IsPlayer()) {
            CBasePlayer *pPlayer = (CBasePlayer *)pOther;
            if (pPlayer->GiveAmmo(30, "762", 90) != -1) {
                EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0, ATTN_NORM);
                return TRUE;
            }
        }
        return FALSE;
    }
};

LINK_ENTITY_TO_CLASS(weapon_ak47, CAK47);
LINK_ENTITY_TO_CLASS(item_762, CAK47Ammo);