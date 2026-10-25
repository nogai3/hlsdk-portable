//
// Created by glackus on 22.10.2026.
//
#include "hud.h"
#include "cl_util.h"
#include "hud_crosshair.h"

static cvar_t* cl_crosshair_size = NULL;
static cvar_t* cl_crosshair_color = NULL;

int CHudCrosshair::Init(void) {
    cl_crosshair_size = CVAR_CREATE("cl_crosshair_size", "10", FCVAR_ARCHIVE);
    cl_crosshair_color = CVAR_CREATE("cl_crosshair_color", "0 255 0", FCVAR_ARCHIVE);
    m_iFlags |= HUD_ACTIVE;

    gHUD.AddHudElem(this);
    return 1;
}

int CHudCrosshair::VidInit(void) {
    m_iFlags |= HUD_ACTIVE;
    return 1;
}

int CHudCrosshair::Draw(float flTime) {
    if (gEngfuncs.IsSpectateOnly() || gHUD.m_fPlayerDead)
        return 1;

    int x = ScreenWidth / 2;
    int y = ScreenHeight / 2;

    int size = (cl_crosshair_size && cl_crosshair_size->value > 0) ? (int)cl_crosshair_size->value : 10;
    int gap = 4;

    int r = 0, g = 255, b = 0;
    if (cl_crosshair_color && cl_crosshair_color->string) {
        sscanf(cl_crosshair_color->string, "%d %d %d", &r, &g, &b);
    }

    FillRGBA(x - gap - size, y - 1, size, 2, r, g, b, 255);
    FillRGBA(x + gap, y - 1, size, 2, r, g, b, 255);
    FillRGBA(x - 1, y - gap - size, 2, size, r, g, b, 255);
    FillRGBA(x - 1, y + gap, 2, size, r, g, b, 255);

    return 1;
}