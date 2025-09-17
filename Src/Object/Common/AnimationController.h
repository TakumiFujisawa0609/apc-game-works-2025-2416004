#pragma once

#include <string>
#include <map>

class AnimationController
{
public:
    enum class Mode {
        Internal,
        External
    };

    struct Animation {
        int model = -1;         // 外部モデル（内部アニメの場合は -1）
        int attachNo = -1;      // アタッチ番号
        int animIndex = 0;      // アニメインデックス（内部用）
        float speed = 1.0f;
        float totalTime = 0.0f;
        float step = 0.0f;
        Mode mode = Mode::Internal;
    };

    AnimationController(int modelId);
    ~AnimationController(void);

    void AddInternal(int type, int animIndex, float speed);
    void AddExternal(int type, const std::string& path, float speed);

    void Play(int type, bool isLoop = true);
    void Update(void);
    bool IsEnd(void) const;
    int GetPlayType(void) const;
    void Release(void);

private:
    void Add(int type, Animation anim);

    int modelId_;
    std::map<int, Animation> animations_;
    Animation playAnim_;
    int playType_;
    bool isLoop_;
};
