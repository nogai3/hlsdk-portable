#include "grenade_modular.h"
#include "weapon_definitions.h"

static ModularGrenadeInfo kHEInfo = {
    "weapon_hegrenade",           // classname
    "HEGrenade",                  // ammoType
    "models/v_hegrenade.mdl",     // viewModel
    "models/p_hegrenade.mdl",     // playerModel
    "models/w_hegrenade.mdl",     // worldModel
    1,                            // maxAmmo
    4, 1,                         // slot, position
    WEAPON_GRENADE,                           // weaponId
    GRENADE_HE                    // grenadeType
};

class CHEGrenade : public CBaseModularGrenade {
public:
    void Spawn() override {
        SetGrenadeInfo(kHEInfo);
        CBaseModularGrenade::Spawn();
    }

    void Precache() override {
        SetGrenadeInfo(kHEInfo);
        CBaseModularGrenade::Precache();
    }
};

LINK_ENTITY_TO_CLASS(weapon_hegrenade, CHEGrenade);