#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

class CBuyZone : public CBaseEntity {
public:
    void Spawn(void) override {
        pev->solid = SOLID_TRIGGER;
        pev->movetype = MOVETYPE_NONE;
        SET_MODEL(ENT(pev), STRING(pev->model));
        SetTouch(&CBuyZone::BuyTouch);
    }

    void BuyTouch(CBaseEntity *pOther) {
        if (pOther->IsPlayer()) {
            CBasePlayer *pPlayer = (CBasePlayer *)pOther;
            pPlayer->m_bInBuyzone = true;
        }
    }
};

LINK_ENTITY_TO_CLASS(func_buyzone, CBuyZone);