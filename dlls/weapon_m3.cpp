#include "weapon_modular.h"
#include "weapon_definitions.h"

static ModularWeaponInfo kM3Info = {
    "weapon_m3",                  // classname
    "1276",                       // ammoType
    "models/v_m3.mdl",            // viewModel
    "models/p_m3.mdl",            // playerModel
    "models/w_m3.mdl",            // worldModel
    8, 32,                        // maxClip, maxAmmo
    0.85f, 19.0f, 2.5f,           // fireRate, damage, recoil
    Vector(0.05f, 0.07f, 0.05f),  // vecSpread
    1.0f, 0.75f,                  // spreadCrouch, spreadMove
    true, 20,                     // isShotgun, pelletsCount
    BULLET_PLAYER_1276,       // bulletType
    "weapons/m3_1.wav",           // soundFire
    "weapons/dryfirevalue.wav",   // soundEmpty
    1, 1,                         // slot, position
    "carbine", WEAPON_M3,                // animExtension, weaponId
    0, 5, 6,                      // animIdle (0), animReload (1), animDraw (2)
    1, 2,                         // animFireMin (3), animFireMax (4)
    3.08f, "models/shotgunshell.mdl"     // shellModel
};

class CM3 : public CBaseModularWeapon {
public:
    void Spawn() override {
        SetWeaponInfo(kM3Info);
        CBaseModularWeapon::Spawn();
    }

    void Precache() override {
        SetWeaponInfo(kM3Info);
        CBaseModularWeapon::Precache();
    }
};

class CM3Ammo : public CBasePlayerAmmo {
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
            if (pPlayer->GiveAmmo(8, "1276", 32) != -1) {
                EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0, ATTN_NORM);
                return TRUE;
            }
        }
        return FALSE;
    }
};

LINK_ENTITY_TO_CLASS(weapon_m3, CM3);
LINK_ENTITY_TO_CLASS(item_1276, CM3Ammo);