#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "decals.h"
#include "weapons.h"
#include "shake.h"
#include "grenade_shared.h"

class CCGrenadeProjectile : public CBaseEntity {
public:
    GrenadeType m_type;
    unsigned short m_usExplo;

    void Spawn() override {
        Precache();

        pev->movetype = MOVETYPE_BOUNCE;
        pev->solid = SOLID_BBOX;
        pev->friction = 0.55f;

        m_type = (GrenadeType)pev->iuser1;

        switch (m_type) {
            case GRENADE_HE:
                SET_MODEL(ENT(pev), "models/w_hegrenade.mdl");
                break;
            case GRENADE_FLASH:
                SET_MODEL(ENT(pev), "models/w_flashbang.mdl");
                break;
            case GRENADE_SMOKE:
                SET_MODEL(ENT(pev), "models/w_smokegrenade.mdl");
                break;
        }

        UTIL_SetSize(pev, Vector(-1, -1, -1), Vector(1, 1, 1));

        SetTouch(&CCGrenadeProjectile::BounceTouch);
        SetThink(&CCGrenadeProjectile::Detonate);

        pev->nextthink = gpGlobals->time + 1.5f;
    }

    void Precache() override {
        PRECACHE_MODEL("sprites/zerogxplode.spr");
        PRECACHE_MODEL("sprites/steam1.spr");

        PRECACHE_SOUND("weapons/grenade_hit1.wav");
        PRECACHE_SOUND("weapons/grenade_hit2.wav");
        PRECACHE_SOUND("weapons/grenade_hit3.wav");
        PRECACHE_SOUND("weapons/he_explode.wav");
        PRECACHE_SOUND("weapons/flashbang_1.wav");
        PRECACHE_SOUND("weapons/sg_explode.wav");
    }

    void BounceTouch(CBaseEntity *pOther) {
        if (pev->flags & FL_ONGROUND) {
            pev->velocity = pev->velocity * 0.8f;
        } else {
            if (pev->velocity.Length() > 100.0f) {
                switch (RANDOM_LONG(0, 2)) {
                    case 0: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit1.wav", 0.25f, ATTN_NORM); break;
                    case 1: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit2.wav", 0.25f, ATTN_NORM); break;
                    case 2: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit3.wav", 0.25f, ATTN_NORM); break;
                }
            }
            pev->velocity = pev->velocity * 0.45f;
        }
    }

    void Detonate() {
        SetTouch(NULL);
        SetThink(NULL);

        switch (m_type) {
            case GRENADE_HE:
                ExplodeHE();
                break;
            case GRENADE_FLASH:
                ExplodeFlash();
                break;
            case GRENADE_SMOKE:
                ExplodeSmoke();
                break;
        }
        pev->flags |= FL_KILLME;
    }

private:
    void ExplodeHE() {
        TraceResult tr;
        Vector vecEnd = pev->origin - Vector(0, 0, 64);
        UTIL_TraceLine(pev->origin, vecEnd, ignore_monsters, ENT(pev), &tr);

        if (tr.flFraction < 1.0f) {
            MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, tr.vecEndPos);
                WRITE_BYTE(TE_WORLDDECAL);
                WRITE_COORD(tr.vecEndPos.x);
                WRITE_COORD(tr.vecEndPos.y);
                WRITE_COORD(tr.vecEndPos.z);
                WRITE_BYTE(DECAL_SCORCH1 + RANDOM_LONG(0, 1));
            MESSAGE_END();
        }
        int sprIndex = PRECACHE_MODEL("sprites/zerogxplode.spr");
        MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
            WRITE_BYTE(TE_EXPLOSION);
            WRITE_COORD(pev->origin.x);
            WRITE_COORD(pev->origin.y);
            WRITE_COORD(pev->origin.z + 16.0f);
            WRITE_SHORT(sprIndex);
            WRITE_BYTE(30);
            WRITE_BYTE(15);
            WRITE_BYTE(TE_EXPLFLAG_NONE);
        MESSAGE_END();
        EMIT_SOUND(ENT(pev), CHAN_STATIC, "weapons/he_explode.wav", 1.0f, ATTN_NORM);
        RadiusDamage(
            pev->origin,
            pev,
            VARS(pev->owner),
            100.0f,
            350.0f,
            CLASS_NONE,
            DMG_BLAST
        );
    }

    void ExplodeFlash() {
        EMIT_SOUND(ENT(pev), CHAN_STATIC, "weapons/flashbang_1.wav", 1.0f, ATTN_NORM);

        CBaseEntity *pEntity = NULL;
        while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, 1500.0f)) != NULL) {
            if (pEntity->IsPlayer()) {
                TraceResult tr;
                UTIL_TraceLine(pev->origin, pEntity->EyePosition(), ignore_monsters, ENT(pev), &tr);

                if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict()) {
                    UTIL_ScreenFade(pEntity, Vector(255, 255, 255), 4.0f, 1.0f, 255, FFADE_IN);
                }
            }
        }
    }

    void ExplodeSmoke() {
        EMIT_SOUND(ENT(pev), CHAN_STATIC, "weapons/sg_explode.wav", 1.0f, ATTN_NORM);

        int sprIndex = PRECACHE_MODEL("sprites/steam1.spr");

        MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
            WRITE_BYTE(TE_SMOKE);
            WRITE_COORD(pev->origin.x);
            WRITE_COORD(pev->origin.y);
            WRITE_COORD(pev->origin.z + 8.0f);
            WRITE_SHORT(sprIndex);
            WRITE_BYTE(25);
            WRITE_BYTE(10);
        MESSAGE_END();
    }
};

LINK_ENTITY_TO_CLASS(wgrenade_proj, CCGrenadeProjectile);