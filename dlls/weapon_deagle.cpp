#include "weapon_modular.h"
#include "weapon_definitions.h"

static ModularWeaponInfo kDeagleInfo = {
    "weapon_deagle",
    "50ae",
    "models/v_deagle.mdl",
    "models/p_deagle.mdl",
    "models/w_deagle.mdl",
    7, 35,
    0.22f, 54.0f, 2.5f,
    Vector(0.04f, 0.04f, 0.04f),
    0.75f, 1.5f,
    false, 1,
    BULLET_PLAYER_50AE,
    "weapons/deagle_1.wav",
    "weapons/dryfirevalue.wav",
    3, 1,
    ANIMEXT_ONEHANDED, WEAPON_DEAGLE,
    0, 4, 5,
    1, 2,
    2.2f, "models/rshell.mdl"
};

class CDeagle : public CBaseModularWeapon {
public:
    void Spawn() override {
        SetWeaponInfo(kDeagleInfo);
        CBaseModularWeapon::Spawn();
    }

    void Precache() override {
        SetWeaponInfo(kDeagleInfo);
        CBaseModularWeapon::Precache();
    }
};

class CDeagleAmmo : public CBasePlayerAmmo {
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
            if (pPlayer->GiveAmmo(7, "50ae", 35) != -1) {
                EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0, ATTN_NORM);
                return TRUE;
            }
        }
        return FALSE;
    }
};

LINK_ENTITY_TO_CLASS(weapon_deagle, CDeagle);
LINK_ENTITY_TO_CLASS(item_50ae, CDeagleAmmo);