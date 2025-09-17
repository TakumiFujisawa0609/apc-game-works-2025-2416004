#define NOMINMAX
#include "HitObject.h"

#include <cmath>
#include <algorithm>
#include "../../Application.h"
#include "../player.h"

void HitObject::OnPlayerHitSphere(VECTOR& playerPos, float playerRadius)
{
    // ヒット位置（中心）とプレイヤーの距離ベクトル
    VECTOR toPlayer = VSub(playerPos, GetHitPosition());

    float distSq = VDot(toPlayer, toPlayer);
    float radiusSum = playerRadius + GetHitRadius();
    float radiusSumSq = radiusSum * radiusSum;

    if (distSq < radiusSumSq && distSq > 0.0001f)
    {
        float dist = sqrtf(distSq);
        VECTOR normal = VScale(toPlayer, 1.0f / dist); // 単位ベクトル

        float pushBack = radiusSum - dist;
        VECTOR pushVec = VScale(normal, pushBack * 1.5f);

        if (pushVec.y > 0) pushVec.y = 0; // 上方向には押し返さない（任意）

        playerPos = VAdd(playerPos, pushVec); // プレイヤーを押し戻す
    }
}

void HitObject::UpdateUIVisibility(bool isHit)
{
    if (isHit)
    {
        uiVisible_ = true;
        uiHideDelayFrames_ = UI_HIDE_DELAY_MAX;
        ShowUI();
    }
    else
    {
        if (uiHideDelayFrames_ > 0)
        {
            uiHideDelayFrames_--;
            ShowUI();
        }
        else if (uiVisible_)
        {
            uiVisible_ = false;
            HideUI();
            OnPlayerExit();
        }
    }
}

void HitObject::OnPlayerHitAABB(Player* player, VECTOR& playerPos, const VECTOR& playerMin, const VECTOR& playerMax)
{
    VECTOR objMin = GetHitMin();
    VECTOR objMax = GetHitMax();

    float dx = std::min(playerMax.x, objMax.x) - std::max(playerMin.x, objMin.x);
    float dz = std::min(playerMax.z, objMax.z) - std::max(playerMin.z, objMin.z);

    if (dx > 0.0f && dz > 0.0f)
    {
        float playerCenterX = (playerMin.x + playerMax.x) * 0.5f;
        float objCenterX = (objMin.x + objMax.x) * 0.5f;
        float playerCenterZ = (playerMin.z + playerMax.z) * 0.5f;
        float objCenterZ = (objMin.z + objMax.z) * 0.5f;

        if (dx < dz)
        {
            float push = (playerCenterX > objCenterX) ? dx : -dx;
            playerPos.x += push;

            // ロック方向設定
            if (player) {
                player->SetBlockedDirX((push > 0) ? -1 : 1);
            }
        }
        else
        {
            float push = (playerCenterZ > objCenterZ) ? dz : -dz;
            playerPos.z += push;

            if (player) {
                player->SetBlockedDirZ((push > 0) ? -1 : 1);
            }
        }
    }
}
