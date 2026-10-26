#include "weapon_modular.h"
#include "weapon_definitions.h"

static ModularWeaponInfo kGlock18Info = {
    "weapon_glock18",
    "918",
    "models/v_glock18.mdl",
    "models/p_glock18.mdl",
    "models/w_glock18.mdl",
    20, 120,
    0.15f, 20.0f, 1.0f,
    Vector(0.05f, 0.05f, 0.05f),
    0.9f, 1.5f,
    false, 1,
    BULLET_PLAYER_918,
    "weapons/glock18_1.wav",
    "weapons/dryfirevalue.wav",
    2, 1,
    "onehanded", WEAPON_GLOCK18,
    0, 7, 8,
    3, 5,
    2.17f, "models/rshell.mdl"
};

static ModularWeaponInfo kGlock18BurstInfo = {
    "weapon_glock18",
        "918",
        "models/v_glock18.mdl",
        "models/p_glock18.mdl",
        "models/w_glock18.mdl",
        255, 255,
        0.05f, 20.0f, 1.2f,
        Vector(0.06f, 0.06f, 0.06f),
    0.75f, 1.5f,
        false, 1,
        BULLET_PLAYER_918,
        "weapons/glock18_1.wav",
        "weapons/dryfirevalue.wav",
        2, 1,
        ANIMEXT_ONEHANDED, WEAPON_GLOCK18,
        0, 7, 8,
        3, 5,
        2.17f, "models/rshell.mdl"
};

class CGlock18 : public CBaseModularWeapon {
public:
    bool m_bBurst = false;

    void Spawn() override {
        SetWeaponInfo(kGlock18Info);
        CBaseModularWeapon::Spawn();
    }

    void Precache() override {
        SetWeaponInfo(kGlock18Info);
        CBaseModularWeapon::Precache();
    }

    void SecondaryAttack() override {
        m_bBurst = !m_bBurst;

        hudtextparms_t textParams;
        textParams.x = -1.0;
        textParams.y = 0.35;
        textParams.effect = 0;
        textParams.r1 = 255;
        textParams.g1 = 180;
        textParams.b1 = 0;
        textParams.r2 = 255;
        textParams.g2 = 255;
        textParams.b2 = 255;
        textParams.fadeinTime = 0.2f;
        textParams.fadeoutTime = 0.5f;
        textParams.holdTime = 4.0f;
        textParams.fxTime = 0.5f;
        textParams.channel = 1;

        const char *pszMode = m_bBurst ? "Switched to burst mode" : "Switched to semi-auto mode";
        UTIL_HudMessage(m_pPlayer, textParams, pszMode);

        m_flNextSecondaryAttack = WTime() + 0.3f;
    }
};

class CGlock18Ammo : public CBasePlayerAmmo {
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
            if (pPlayer->GiveAmmo(255, "918", 255) != -1) {
                EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0, ATTN_NORM);
                return TRUE;
            }
        }
        return FALSE;
    }
};

LINK_ENTITY_TO_CLASS(weapon_glock18, CGlock18);
LINK_ENTITY_TO_CLASS(item_918, CGlock18Ammo);