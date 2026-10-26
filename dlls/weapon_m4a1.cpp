#include "weapon_modular.h"
#include "weapon_definitions.h"

static ModularWeaponInfo kM4A1Info = {
    "weapon_m4a1",                // classname
    "556",                        // ammoType
    "models/v_m4a1.mdl",          // viewModel
    "models/p_m4a1.mdl",          // playerModel
    "models/w_m4a1.mdl",          // worldModel
    30, 90,                       // maxClip, maxAmmo
    0.0875f, 32.0f, 0.95f,        // fireRate, damage, recoil
    Vector(0.015f, 0.015f, 0.015f),// vecSpread
    0.8f, 1.3f,                   // spreadCrouch, spreadMove
    false, 1,                     // isShotgun, pelletsCount
    BULLET_PLAYER_556,          // bulletType
    "weapons/m4a1_1.wav",         // soundFire
    "weapons/dryfirevalue.wav",   // soundEmpty
    1, 3,                         // slot, position
    "rifle", WEAPON_M4A1,                  // animExtension, weaponId
    0, 4, 5,                      // animIdle (0), animReload (1), animDraw (2)
    1, 3,                         // animFireMin (3), animFireMax (5)
    3.08f, "models/rshell.mdl"           // shellModel
};
static ModularWeaponInfo kM4A1UnsilInfo = {
    "weapon_m4a1u",                // classname
    "556",                        // ammoType
    "models/v_m4a1.mdl",          // viewModel
    "models/p_m4a1.mdl",          // playerModel
    "models/w_m4a1.mdl",          // worldModel
    30, 90,                       // maxClip, maxAmmo
    0.0875f, 32.0f, 0.95f,        // fireRate, damage, recoil
    Vector(0.015f, 0.015f, 0.015f),// vecSpread
    0.8f, 1.3f,                   // spreadCrouch, spreadMove
    false, 1,                     // isShotgun, pelletsCount
    BULLET_PLAYER_556,          // bulletType
    "weapons/m4a1_unsil_1.wav",         // soundFire
    "weapons/dryfirevalue.wav",   // soundEmpty
    1, 3,                         // slot, position
    ANIMEXT_RIFLE, WEAPON_M4A1,                  // animExtension, weaponId
    7, 11, 12,                      // animIdle (0), animReload (1), animDraw (2)
    8, 10,                         // animFireMin (3), animFireMax (5)
    3.08f,
    "models/rshell.mdl"           // shellModel
};

class CM4A1 : public CBaseModularWeapon {
public:
    int i_reloadSoundState = 0;
    bool m_bSilenced = true;

    void Spawn() override {
        SetWeaponInfo(kM4A1Info);
        CBaseModularWeapon::Spawn();
    }

    void Precache() override {
        SetWeaponInfo(kM4A1Info);
        CBaseModularWeapon::Precache();

        PRECACHE_SOUND("weapons/m4a1_unsil_1.wav");
        PRECACHE_SOUND("weapons/m4a1_silencer_off.wav");
        PRECACHE_SOUND("weapons/m4a1_silencer_on.wav");

        // Reload & Draw
        PRECACHE_SOUND("weapons/m4a1_cliout.wav");
        PRECACHE_SOUND("weapons/m4a1_clipin.wav");
        PRECACHE_SOUND("weapons/m4a1_boltpull.wav");
    }

    void SecondaryAttack() override {
        if (m_fInReload || m_flNextSecondaryAttack > WTime() || m_flNextPrimaryAttack > WTime())
            return;

        float t = WTime();

        if (m_bSilenced) {
            SendWeaponAnim(13);
            m_bSilenced = false;
            SetWeaponInfo(kM4A1UnsilInfo);
            // EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/m4a1_silencer_off.wav", 1.0, ATTN_NORM);
        } else {
            SendWeaponAnim(6);
            m_bSilenced = true;
            SetWeaponInfo(kM4A1Info);
            // EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/m4a1_silencer_on.wav", 1.0, ATTN_NORM);
        }

        m_flNextPrimaryAttack   = t + 2.0f;
        m_flNextSecondaryAttack = t + 2.0f;
        m_flTimeWeaponIdle      = t + 2.0f;
    }

    BOOL Deploy() override {
        EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/m4a1_boltpull.wav", 1.0, ATTN_NORM);
        CBaseModularWeapon::Deploy();
        return TRUE;
    }

    void Reload() override {
        if (!m_pPlayer || m_fInReload)
            return;
        if (m_iClip >= m_Info.maxClip || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
            return;

        CBaseModularWeapon::Reload();

        i_reloadSoundState = 0;
        SetThink(&CM4A1::ReloadSoundThink);
        pev->nextthink = WTime() + 0.1f;
    }

    void ReloadSoundThink() {
        switch (i_reloadSoundState) {
            case 0:
                EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/m4a1_clipout.wav", 1.0, ATTN_NORM);
                i_reloadSoundState = 1;
                pev->nextthink = WTime() + 0.9f;
                break;

            case 1:
                EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/m4a1_clipin.wav", 1.0, ATTN_NORM);
                i_reloadSoundState = 2;
                pev->nextthink = WTime() + 0.8f;
                break;

            case 2:
                EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/m4a1_boltpull.wav", 1.0, ATTN_NORM);
                SetThink(NULL);
                break;
        }
    }
};

class CM4Ammo : public CBasePlayerAmmo {
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
            if (pPlayer->GiveAmmo(30, "556", 90) != -1) {
                EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0, ATTN_NORM);
                return TRUE;
            }
        }
        return FALSE;
    }
};

LINK_ENTITY_TO_CLASS(weapon_m4a1, CM4A1);
LINK_ENTITY_TO_CLASS(item_556, CM4Ammo);