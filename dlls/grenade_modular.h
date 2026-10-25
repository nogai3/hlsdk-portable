#ifndef HLSDK_PORTABLE_MODULAR_GRENADE_H
#define HLSDK_PORTABLE_MODULAR_GRENADE_H

#include "weapon_modular.h"
#include "grenade_shared.h"

struct ModularGrenadeInfo {
    const char *classname;
    const char *ammoType;
    const char *viewModel;
    const char *playerModel;
    const char *worldModel;

    int maxAmmo;
    int slot;
    int position;
    int weaponId;

    GrenadeType grenade;
};

class CBaseModularGrenade : public CBasePlayerWeapon {
protected:
    ModularGrenadeInfo m_Info;
    bool m_bStartThrow = false;
    float m_flReleaseThrow = -1.0f;

public:
    void SetGrenadeInfo(const ModularGrenadeInfo &info) { m_Info = info; }

    void Spawn() override {
        Precache();
        m_iId = m_Info.weaponId;
        SET_MODEL(ENT(pev), m_Info.worldModel);
        m_iDefaultAmmo = 1;
        m_iClip = WEAPON_NOCLIP;
        FallInit();
    }

    void Precache() override {
        m_iId = m_Info.weaponId;
        if (m_Info.viewModel && *m_Info.viewModel)
            PRECACHE_MODEL((char*)m_Info.viewModel);
        if (m_Info.playerModel && *m_Info.playerModel)
            PRECACHE_MODEL((char*)m_Info.playerModel);
        if (m_Info.worldModel && *m_Info.worldModel)
            PRECACHE_MODEL((char*)m_Info.worldModel);
    }

    int GetItemInfo(ItemInfo *p) override {
        p->pszName   = m_Info.classname;
        p->pszAmmo1  = m_Info.ammoType;
        p->iMaxAmmo1 = m_Info.maxAmmo;
        p->pszAmmo2  = NULL;
        p->iMaxAmmo2 = -1;
        p->iMaxClip  = WEAPON_NOCLIP;
        p->iSlot     = m_Info.slot - 1;
        p->iPosition = m_Info.position - 1;
        p->iFlags    = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
        p->iId       = m_Info.weaponId;
        p->iWeight   = 5;
        return 1;
    }

    BOOL AddToPlayer(CBasePlayer *pPlayer) override {
        if (CBasePlayerWeapon::AddToPlayer(pPlayer)) {
            MESSAGE_BEGIN(MSG_ONE, gmsgWeaponList, NULL, pPlayer->pev);
                WRITE_STRING(m_Info.classname);
                WRITE_BYTE(pPlayer->GetAmmoIndex(m_Info.ammoType));
                WRITE_BYTE(m_Info.maxAmmo);
                WRITE_BYTE(-1);
                WRITE_BYTE(-1);
                WRITE_BYTE(m_Info.slot - 1);
                WRITE_BYTE(m_Info.position - 1);
                WRITE_BYTE(m_iId);
                WRITE_BYTE(ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE);
            MESSAGE_END();

            pPlayer->GiveAmmo(1, (char*)m_Info.ammoType, m_Info.maxAmmo);
            return TRUE;
        }
        return FALSE;
    }

    int AddDuplicate(CBasePlayerItem *pOriginal) override {
        if (m_pPlayer) {
            return m_pPlayer->GiveAmmo(1, (char*)m_Info.ammoType, m_Info.maxAmmo);
        }
        return FALSE;
    }

    BOOL Deploy() override {
        m_bStartThrow = false;
        m_flReleaseThrow = -1.0f;

        if (m_pPlayer) {
            m_iPrimaryAmmoType = m_pPlayer->GetAmmoIndex(m_Info.ammoType);
        }

        return DefaultDeploy((char*)m_Info.viewModel, (char*)m_Info.playerModel, 3, "grenade");
    }

    BOOL IsUseable() override {
        return TRUE;
    }

    void PrimaryAttack() override {
        if (!m_pPlayer || m_bStartThrow)
            return;

        SendWeaponAnim(1);
        m_bStartThrow = true;
        m_flReleaseThrow = -1.0f;
        m_flNextPrimaryAttack = gpGlobals->time + 0.5f;
        m_flTimeWeaponIdle = gpGlobals->time + 0.5f;
    }

    void ItemPostFrame() override {
        if (!m_pPlayer)
            return;
        if ((m_pPlayer->pev->button & IN_ATTACK) && m_flNextPrimaryAttack <= gpGlobals->time) {
            if (!m_bStartThrow) {
                PrimaryAttack();
            }
        }

        if (m_bStartThrow && !(m_pPlayer->pev->button & IN_ATTACK)) {
            if (m_flReleaseThrow < 0.0f) {
                SendWeaponAnim(2); // Анимация броска
                m_pPlayer->SetAnimation(PLAYER_ATTACK1);
                m_flReleaseThrow = gpGlobals->time + 0.1f;
            }
        }
        if (m_flReleaseThrow > 0.0f && gpGlobals->time >= m_flReleaseThrow) {
            ThrowGrenade();
            m_flReleaseThrow = -1.0f;
            m_bStartThrow = false;

            int iAmmo = m_pPlayer->GetAmmoIndex(m_Info.ammoType);
            if (iAmmo >= 0 && m_pPlayer->m_rgAmmo[iAmmo] > 0) {
                m_pPlayer->m_rgAmmo[iAmmo]--;
            }

            m_flNextPrimaryAttack = gpGlobals->time + 0.5f;
            m_flTimeWeaponIdle = gpGlobals->time + 0.5f;
            return;
        }
        if (!m_bStartThrow && m_flNextPrimaryAttack <= gpGlobals->time) {
            int iAmmo = m_pPlayer->GetAmmoIndex(m_Info.ammoType);
            if (iAmmo >= 0 && m_pPlayer->m_rgAmmo[iAmmo] <= 0) {
                RetireWeapon();
                return;
            }
        }

        WeaponIdle();
    }

    void ThrowGrenade() {
        if (!m_pPlayer)
            return;

        UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

        Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16.0f;
        Vector vecThrow = gpGlobals->v_forward * 1050.0f + gpGlobals->v_up * 50.0f + m_pPlayer->pev->velocity;

        CBaseEntity *pEntity = CBaseEntity::Create(
            "wgrenade_proj", vecSrc, m_pPlayer->pev->angles, m_pPlayer->edict()
        );

        if (pEntity) {
            pEntity->pev->iuser1 = (int)m_Info.grenade;
            pEntity->pev->velocity = vecThrow;
            pEntity->pev->owner = m_pPlayer->edict();
            pEntity->Spawn();
        }
    }
};

#endif // HLSDK_PORTABLE_MODULAR_GRENADE_H