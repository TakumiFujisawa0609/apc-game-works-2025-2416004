#pragma once

#include <DxLib.h>

#include "Interact/HitObject.h"

class NullHitObject : public HitObject {
public:
    HIT_TYPE GetHitType(void) const override;
    
    VECTOR GetHitPosition(void) const override;

	float GetHitRadius(void) const override;

	VECTOR GetHitMin(void) const override;

	VECTOR GetHitMax(void) const override;
    
    void ShowUI(void) override;
    
    void HideUI(void) override;
    
    bool IsValid(void) const override;

    void OnPlayerHit(void) override;

    void OnPlayerExit(void) override;
};


