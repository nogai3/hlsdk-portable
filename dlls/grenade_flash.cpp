#include "grenade_modular.h"
#include "weapon_definitions.h"

static ModularGrenadeInfo kFlashInfo = {
    "weapon_flashbang",           // classname
    "Flashbang",                  // ammoType
    "models/v_flashbang.mdl",     // viewModel
    "models/p_flashbang.mdl",     // playerModel
    "models/w_flashbang.mdl",     // worldModel
    2,                            // maxAmmo
    4, 2,                         // slot, position
    WEAPON_GRENADE,                           // weaponId
    GRENADE_FLASH                 // grenadeType
};

class CFlashbang : public CBaseModularGrenade {
public:
    void Spawn() override {
        SetGrenadeInfo(kFlashInfo);
        CBaseModularGrenade::Spawn();
    }

    void Precache() override {
        SetGrenadeInfo(kFlashInfo);
        CBaseModularGrenade::Precache();
    }
};

LINK_ENTITY_TO_CLASS(weapon_flashbang, CFlashbang);