#pragma once
#include "../UnitBase.h"
#include <vector>

class Player;

class FireAttack : public UnitBase
{
public:
    FireAttack(void);
    ~FireAttack(void) override;

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

    static constexpr float FIRE_RANGE = 180.0f;
    static constexpr float FIRE_DURATION = 0.8f;
    static constexpr float FIRE_OFFSET_Z = -120.0f;
    static constexpr float FIRE_OFFSET_Y = 50.0f;
};