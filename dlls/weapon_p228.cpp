#include "weapon_modular.h"
#include "weapon_definitions.h"

static ModularWeaponInfo kP228Info = {
    "weapon_p228",                // classname
    "357sig",                     // ammoType
    "models/v_p228.mdl",          // viewModel
    "models/p_p228.mdl",          // playerModel
    "models/w_p228.mdl",          // worldModel
    13, 52,                       // maxClip, maxAmmo
    0.15f, 32.0f, 0.9f,           // fireRate, damage, recoil
    Vector(0.035f, 0.035f, 0.035f),// vecSpread
    0.75f, 1.5f,                  // spreadCrouch, spreadMove
    false, 1,                     // isShotgun, pelletsCount
    BULLET_PLAYER_357SIG,         // bulletType
    "weapons/p228_1.wav",         // soundFire
    "weapons/dryfirevalue.wav",   // soundEmpty
    3, 1,                        // slot, position
    ANIMEXT_ONEHANDED, WEAPON_P228,     // animExtension, weaponId
    0, 6, 7,                      // animIdle (0), animReload (4), animDraw (5)
    1, 3,                         // animFireMin (1), animFireMax (3)
    2.7f, "models/pshell.mdl"     // reloadTime, shellModel
};

class CP228 : public CBaseModularWeapon {
public:
    void Spawn() override {
        SetWeaponInfo(kP228Info);
        CBaseModularWeapon::Spawn();
    }

    void Precache() override {
        SetWeaponInfo(kP228Info);
        CBaseModularWeapon::Precache();
    }
};

class CP228Ammo : public CBasePlayerAmmo {
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
            if (pPlayer->GiveAmmo(13, "357sig", 52) != -1) {
                EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0, ATTN_NORM);
                return TRUE;
            }
        }
        return FALSE;
    }
};

LINK_ENTITY_TO_CLASS(weapon_p228, CP228);
LINK_ENTITY_TO_CLASS(item_357sig, CP228Ammo);