#include "weapon_modular.h"
#include "weapon_definitions.h"

static ModularWeaponInfo kEliteInfo = {
    "weapon_elite",               // classname
    "918",                        // ammoType
    "models/v_elite.mdl",         // viewModel
    "models/p_elite.mdl",         // playerModel
    "models/w_elite.mdl",         // worldModel
    30, 120,                      // maxClip, maxAmmo
    0.12f, 20.0f, 1.1f,           // fireRate, damage, recoil
    Vector(0.04f, 0.04f, 0.04f),  // vecSpread
    0.75f, 1.5f,                  // spreadCrouch, spreadMove
    false, 1,                     // isShotgun, pelletsCount
    BULLET_PLAYER_918,            // bulletType
    "weapons/elite_fire.wav",     // soundFire
    "weapons/dryfirevalue.wav",   // soundEmpty
    1, 1,                         // slot, position
    "dualpistols", WEAPON_ELITE,  // animExtension, weaponId
    0, 14, 15,                    // animIdle (0), animReload (14), animDraw (15)
    1, 13,                        // animFireMin (1), animFireMax (13)
    4.5f, "models/pshell.mdl"     // reloadTime, shellModel
};

class CElites : public CBaseModularWeapon {
public:
    void Spawn() override {
        SetWeaponInfo(kEliteInfo);
        CBaseModularWeapon::Spawn();
    }

    void Precache() override {
        SetWeaponInfo(kEliteInfo);
        CBaseModularWeapon::Precache();
    }
};

class CElitesAmmo : public CBasePlayerAmmo {
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
            if (pPlayer->GiveAmmo(30, "918e", 120) != -1) {
                EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0, ATTN_NORM);
                return TRUE;
            }
        }
        return FALSE;
    }
};

LINK_ENTITY_TO_CLASS(weapon_elites, CElites);
LINK_ENTITY_TO_CLASS(item_918e, CElitesAmmo);