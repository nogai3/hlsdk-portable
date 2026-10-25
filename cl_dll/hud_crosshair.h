//
// Created by glackus on 24.10.2026.
//

#ifndef HLSDK_PORTABLE_HUD_CROSSHAIR_H
#define HLSDK_PORTABLE_HUD_CROSSHAIR_H

#include "hud.h"

class CHudCrosshair : public CHudBase {
public:
    int Init(void) override;
    int VidInit(void) override;
    int Draw(float flTime) override;
};

#endif //HLSDK_PORTABLE_HUD_CROSSHAIR_H
