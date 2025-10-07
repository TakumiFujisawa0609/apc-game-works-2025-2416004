#pragma once

#include <DxLib.h>

class Transform;

class EnemyDummy
{
public:
	EnemyDummy(void);

	~EnemyDummy(void);

	void Load(void);

	void Init(void);

	void Update(void);

	void Draw(void);

	void Release(void);

	Transform& GetTransform(void);

private:
	int modelId_;

	Transform* trans_;

};

