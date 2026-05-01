#pragma once
#include "../UnitBase.h"
#include <vector>

class Player;

class WaterAttack : public UnitBase
{
public:
    WaterAttack(void);
    ~WaterAttack(void) override;

    void Load(void) override;
    void Initialize(void) override;
    void Initialize(Player* player, const VECTOR& offsetPos);
    void Update(void) override;
    void Draw(void) const override;
    void Release(void) override;

    bool IsActive(void) const;
    void SetActive(bool active);

protected:
    void InitCollider(void) override;
private:
    Player* player_;
    VECTOR offsetPos_;
    float duration_;
    float elapsedTime_;
    bool isActive_;
    int effectHandle_;

    static constexpr float WATER_RANGE = 150.0f;
    static constexpr float WATER_DURATION = 1.0f;
    static constexpr float WATER_OFFSET_Z = -100.0f;
    static constexpr float WATER_OFFSET_Y = 50.0f;
};