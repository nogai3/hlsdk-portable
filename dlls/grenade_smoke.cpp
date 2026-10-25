#include "grenade_modular.h"
#include "weapon_definitions.h"

static ModularGrenadeInfo kSmokeInfo = {
    "weapon_smokegrenade",        // classname
    "SmokeGrenade",               // ammoType
    "models/v_smokegrenade.mdl",   // viewModel
    "models/p_smokegrenade.mdl",   // playerModel
    "models/w_smokegrenade.mdl",   // worldModel
    1,                            // maxAmmo
    4, 3,                         // slot, position
    WEAPON_GRENADE,                           // weaponId
    GRENADE_SMOKE                 // grenadeType
};

class CSmokeGrenade : public CBaseModularGrenade {
public:
    void Spawn() override {
        SetGrenadeInfo(kSmokeInfo);
        CBaseModularGrenade::Spawn();
    }

    void Precache() override {
        SetGrenadeInfo(kSmokeInfo);
        CBaseModularGrenade::Precache();
    }
};

LINK_ENTITY_TO_CLASS(weapon_smokegrenade, CSmokeGrenade);