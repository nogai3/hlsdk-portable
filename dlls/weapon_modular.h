/***
 * WarLands | A LighSync Games (Half-Life) Mod
 * Created by glackus. At 22.10.2026.
 * (C) Copyright. LighSync Games. 2023-2026.
 * (C) Copyright. Gleb Ustimenko. 2026.
***/

#ifndef HLSDK_PORTABLE_WEAPON_MODULAR_H
#define HLSDK_PORTABLE_WEAPON_MODULAR_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"

extern int gmsgWeaponList;
extern int gmsgCurWeapon;

#define ANIMEXT_CARBINE      "carbine"
#define ANIMEXT_ONEHANDED    "onehanded"
#define ANIMEXT_DUALPISTOLS  "dualpistols"
#define ANIMEXT_RIFLE        "rifle"
#define ANIMEXT_MP5          "mp5"
#define ANIMEXT_SHOTGUN      "shotgun"
#define ANIMEXT_M249         "m249"
#define ANIMEXT_AK47         "ak47"
#define ANIMEXT_C4           "c4"
#define ANIMEXT_KNIFE        "knife"
#define ANIMEXT_GRENADE      "grenade"
#define ANIMEXT_SHIELDGUN    "shieldgun"
#define ANIMEXT_SHIELDGREN   "shieldgren"
#define ANIMEXT_SHIELDKNIFE  "shieldknife"

struct ModularWeaponInfo {
    const char* classname;
    const char* ammoType;
    const char* viewModel;
    const char* playerModel;
    const char* worldModel;

    int maxClip;
    int maxAmmo;
    float fireRate;
    float damage;
    float recoil;

    Vector vecSpread;
    float spreadCrouch;
    float spreadMove;

    bool isShotgun;
    int pelletsCount;
    int bulletType;

    const char* soundFire;
    const char* soundEmpty;

    int slot;
    int position;
    const char* animExtension;
    int weaponId;

    int animIdle;
    int animReload;
    int animDraw;
    int animFireMin;
    int animFireMax;

    float reloadTime;

    const char* shellModel;
};

class CBaseModularWeapon : public CBasePlayerWeapon {
protected:
    ModularWeaponInfo m_Info;
    int m_iShell;
    bool m_bAltShoot;

    static inline float WTime() { return gpGlobals->time; }

    void SyncClip() {
        if (!m_pPlayer)
            return;
        MESSAGE_BEGIN(MSG_ONE, gmsgCurWeapon, NULL, m_pPlayer->pev);
            WRITE_BYTE(1);
            WRITE_BYTE(m_iId);
            WRITE_BYTE(m_iClip);
        MESSAGE_END();
    }

    void SanitizeTimers() {
        const float t = WTime();
        const float kMaxDelay = 10.0f;

        if (m_flNextPrimaryAttack > t + kMaxDelay)
            m_flNextPrimaryAttack = t;
        if (m_flNextSecondaryAttack > t + kMaxDelay)
            m_flNextSecondaryAttack = t;
        if (m_flTimeWeaponIdle > t + kMaxDelay)
            m_flTimeWeaponIdle = t;
    }

    void BuildPlayerSeqName(char* out, size_t size, const char* state) const {
        const char* stance = (m_pPlayer->pev->flags & FL_DUCKING) ? "crouch" : "ref";
        snprintf(out, size, "%s_%s_%s", stance, state, m_pPlayer->m_szAnimExtention);
    }

    bool HasPlayerSeq(const char* state) const {
        if (!m_pPlayer)
            return false;
        char name[64];
        BuildPlayerSeqName(name, sizeof(name), state);
        return m_pPlayer->LookupSequence(name) != -1;
    }

    bool PlayPlayerSeq(const char* state) {
        if (!m_pPlayer)
            return false;

        char name[64];
        BuildPlayerSeqName(name, sizeof(name), state);

        const int seq = m_pPlayer->LookupSequence(name);
        if (seq == -1)
            return false;

        m_pPlayer->pev->sequence = seq;
        m_pPlayer->pev->frame = 0;
        m_pPlayer->ResetSequenceInfo();
        m_pPlayer->m_Activity = ACT_RANGE_ATTACK1;
        return true;
    }

    void PlayThirdPersonShoot() {
        if (m_bAltShoot && PlayPlayerSeq("shoot2")) {
            m_bAltShoot = false;
            return;
        }
        m_pPlayer->SetAnimation(PLAYER_ATTACK1);
        m_bAltShoot = HasPlayerSeq("shoot2");
    }

    void PlayThirdPersonReload() {
        PlayPlayerSeq("reload");
    }

    void ApplyAnimExtension() {
        if (!m_pPlayer)
            return;

        const char* ext = (m_Info.animExtension && *m_Info.animExtension)
                          ? m_Info.animExtension
                          : ANIMEXT_CARBINE;

        strncpy(m_pPlayer->m_szAnimExtention, ext, sizeof(m_pPlayer->m_szAnimExtention) - 1);
        m_pPlayer->m_szAnimExtention[sizeof(m_pPlayer->m_szAnimExtention) - 1] = '\0';

        if (!HasPlayerSeq("aim")) {
            ALERT(at_console, "[Modular] %s: player model has no sequence '%s_aim_%s' (animExtension must be only the suffix)\n",
                  m_Info.classname, (m_pPlayer->pev->flags & FL_DUCKING) ? "crouch" : "ref", ext);
        }
    }

public:
    void SetWeaponInfo(const ModularWeaponInfo& info) {
        m_Info = info;
        if (m_pPlayer && m_pPlayer->m_pActiveItem == this)
            ApplyAnimExtension();
    }

    void Spawn() override {
        Precache();
        m_iId = m_Info.weaponId;
        SET_MODEL(ENT(pev), m_Info.worldModel);
        m_iDefaultAmmo = m_Info.maxClip;
        m_iClip = m_Info.maxClip;
        m_fInReload = FALSE;
        m_bAltShoot = false;
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
        if (m_Info.soundFire && *m_Info.soundFire)
            PRECACHE_SOUND((char*)m_Info.soundFire);
        if (m_Info.soundEmpty && *m_Info.soundEmpty)
            PRECACHE_SOUND((char*)m_Info.soundEmpty);

        if (m_Info.shellModel && *m_Info.shellModel)
            m_iShell = PRECACHE_MODEL((char*)m_Info.shellModel);
        else
            m_iShell = PRECACHE_MODEL("models/shell.mdl");
    }

    int GetItemInfo(ItemInfo *p) override {
        p->pszName   = m_Info.classname;
        p->pszAmmo1  = m_Info.ammoType;
        p->iMaxAmmo1 = m_Info.maxAmmo;
        p->pszAmmo2  = NULL;
        p->iMaxAmmo2 = -1;
        p->iMaxClip  = m_Info.maxClip;
        p->iSlot     = m_Info.slot - 1;
        p->iPosition = m_Info.position - 1;
        p->iFlags    = 0;
        p->iId       = m_Info.weaponId;
        p->iWeight   = 10;
        return 1;
    }

    inline int GetModularAmmoIndex(CBasePlayer* pPlayer, const char* szAmmoName) {
        if (!pPlayer || !szAmmoName)
            return -1;

        int iAmmo = pPlayer->GetAmmoIndex(szAmmoName);
        if (iAmmo == -1) {
            iAmmo = pPlayer->GiveAmmo(0, szAmmoName, m_Info.maxAmmo);
        }
        return iAmmo;
    }

    BOOL AddToPlayer(CBasePlayer *pPlayer) override {
        if (CBasePlayerWeapon::AddToPlayer(pPlayer)) {
            MESSAGE_BEGIN(MSG_ONE, gmsgWeaponList, NULL, pPlayer->pev);
                WRITE_STRING(m_Info.classname);
                WRITE_BYTE(GetModularAmmoIndex(pPlayer, m_Info.ammoType));
                WRITE_BYTE(m_Info.maxAmmo);
                WRITE_BYTE(-1);
                WRITE_BYTE(-1);
                WRITE_BYTE(m_Info.slot - 1);
                WRITE_BYTE(m_Info.position - 1);
                WRITE_BYTE(m_iId);
                WRITE_BYTE(0);
            MESSAGE_END();
            return TRUE;
        }
        return FALSE;
    }

    BOOL AddDuplicate(CBasePlayerItem *pItem) override {
        CBasePlayer* pTargetPlayer = m_pPlayer ? m_pPlayer : (pItem ? pItem->m_pPlayer : nullptr);

        if (!pTargetPlayer)
            return FALSE;

        if (pTargetPlayer->GiveAmmo(m_Info.maxClip, (char*)m_Info.ammoType, m_Info.maxAmmo) != -1) {
            EMIT_SOUND(ENT(pTargetPlayer->pev), CHAN_ITEM, "items/9mmclip1.wav", 1.0, ATTN_NORM);
            return TRUE;
        }

        return FALSE;
    }

    BOOL Deploy() override {
        m_bAltShoot = false;
        ApplyAnimExtension();

        const char* ext = (m_Info.animExtension && *m_Info.animExtension) ? m_Info.animExtension : ANIMEXT_CARBINE;
        return DefaultDeploy((char*)m_Info.viewModel, (char*)m_Info.playerModel, m_Info.animDraw, (char*)ext);
    }

    void PrimaryAttack() override {
        if (!m_pPlayer)
            return;
        if (m_flNextPrimaryAttack > WTime() || m_fInReload)
            return;

        if (m_iClip <= 0) {
            if (m_Info.soundEmpty && *m_Info.soundEmpty)
                EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, m_Info.soundEmpty, 0.8, ATTN_NORM);
            else
                PlayEmptySound();

            m_flNextPrimaryAttack = WTime() + 0.2f;
            Reload();
            return;
        }

        m_iClip--;
        SyncClip();

        m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
        m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

        EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, m_Info.soundFire, 1.0, ATTN_NORM);
        PlayThirdPersonShoot();

        int animToPlay = m_Info.animFireMin;
        if (m_Info.animFireMax > m_Info.animFireMin) {
            animToPlay += RANDOM_LONG(0, m_Info.animFireMax - m_Info.animFireMin);
        }
        SendWeaponAnim(animToPlay);

        Vector vecSrc = m_pPlayer->GetGunPosition();
        Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
        Vector vecSpread = GetCurrentSpread();

        int shots = (m_Info.pelletsCount > 0) ? m_Info.pelletsCount : 1;
        float maxDistance = m_Info.isShotgun ? 4096.0f : 8192.0f;

        m_pPlayer->FireBullets(
            shots,
            vecSrc, vecAiming, vecSpread,
            maxDistance, m_Info.bulletType, 0, (int)m_Info.damage
        );

        m_pPlayer->pev->punchangle.x -= m_Info.recoil;
        m_pPlayer->pev->punchangle.y += RANDOM_FLOAT(-m_Info.recoil * 0.5f, m_Info.recoil * 0.5f);

        float t = WTime();
        m_flNextPrimaryAttack   = t + m_Info.fireRate;
        m_flNextSecondaryAttack = t + m_Info.fireRate;
        m_flTimeWeaponIdle      = t + 1.5f;
    }

    void Reload() override {
        if (!m_pPlayer || m_fInReload)
            return;

        if (m_iPrimaryAmmoType < 0) {
            m_iPrimaryAmmoType = GetModularAmmoIndex(m_pPlayer, m_Info.ammoType);
        }

        if (m_iClip >= m_Info.maxClip || m_iPrimaryAmmoType < 0)
            return;

        if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
            return;

        m_fInReload = TRUE;
        SendWeaponAnim(m_Info.animReload);
        PlayThirdPersonReload();

        float t = WTime();
        m_flNextPrimaryAttack   = t + m_Info.reloadTime;
        m_flNextSecondaryAttack = t + m_Info.reloadTime;
        m_flTimeWeaponIdle      = t + m_Info.reloadTime + 0.1f;
    }

    void ItemPostFrame() override {
        if (!m_pPlayer)
            return;

        SanitizeTimers();

        if (m_fInReload && m_flNextPrimaryAttack <= WTime()) {
            if (m_iPrimaryAmmoType >= 0) {
                int reserve = m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType];
                int needed = m_Info.maxClip - m_iClip;
                int take = (reserve < needed) ? reserve : needed;

                m_iClip += take;
                m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= take;

                SyncClip();
            }
            m_fInReload = FALSE;
        }

        if (m_pPlayer->pev->button & IN_ATTACK) {
            PrimaryAttack();
        }
        else if (m_pPlayer->pev->button & IN_ATTACK2) {
            SecondaryAttack();
        }
        else if (m_pPlayer->pev->button & IN_RELOAD) {
            Reload();
        }
        else {
            ResetEmptySound();
        }

        WeaponIdle();
    }

    void WeaponIdle() override {
        if (m_flTimeWeaponIdle <= WTime() && !m_fInReload) {
            SendWeaponAnim(m_Info.animIdle);
            m_flTimeWeaponIdle = WTime() + 5.0f;
        }
    }

    Vector GetCurrentSpread() {
        Vector spread = m_Info.vecSpread;
        if (!m_pPlayer)
            return spread;

        if (!(m_pPlayer->pev->flags & FL_ONGROUND)) {
            spread = spread * 2.5f;
        } else if (m_pPlayer->pev->velocity.Length2D() > 140.0f) {
            spread = spread * m_Info.spreadMove;
        } else if (m_pPlayer->pev->flags & FL_DUCKING) {
            spread = spread * m_Info.spreadCrouch;
        }

        return spread;
    }
};

#endif // HLSDK_PORTABLE_WEAPON_MODULAR_H