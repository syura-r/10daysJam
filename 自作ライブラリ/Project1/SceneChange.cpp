#include "SceneChange.h"
#include "Easing.h"

SceneChange::SceneChange()
{
	mainSp = new Sprite();
}

SceneChange::~SceneChange()
{
	delete mainSp;
}

void SceneChange::Initialize()
{
	pos = { 1920 / 2, 1080 / 2 };
	scale = { 0,0 };
	easingcount = 0;
	toBig = false;
	toSmall = true;
	toBigEnd = false;
	toSmallEnd = false;
}

void SceneChange::Update()
{
	int maxCount = 60;
	float maxScale = 30.0f;
	//¬‚³‚­
	if (toSmall)
	{
		easingcount++;
		scale.x = Easing::EaseOutQuart(maxScale, 0.0f, maxCount, easingcount);
		scale.y = Easing::EaseOutQuart(maxScale, 0.0f, maxCount, easingcount);

		if (easingcount >= maxCount)
		{
			toSmallEnd = true;
			toSmall = false;
		}
	}
	//‘å‚«‚­
	if (toBig)
	{
		easingcount++;
		scale.x = Easing::EaseInQuart(0.0f, maxScale, maxCount, easingcount);
		scale.y = Easing::EaseInQuart(0.0f, maxScale, maxCount, easingcount);

		if (easingcount >= maxCount)
		{
			toBigEnd = true;
			toBig = false;
		}
	}
}

void SceneChange::Draw(XMFLOAT4 color)
{
	mainSp->DrawSprite("hidari_sceneChange", pos, 0, scale, color);
}

void SceneChange::ChangeStart()
{
	toBig = true;
	easingcount = 0;
}
