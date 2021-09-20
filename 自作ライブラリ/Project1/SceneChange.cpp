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
	//¬‚³‚­
	if (toSmall)
	{
		int maxCount = 120;
		float maxScale = 20.0f;
		easingcount++;
		scale.x = Easing::EaseOutQuart(maxScale, 0.0f,maxCount, easingcount);
		scale.y = Easing::EaseOutQuart(maxScale, 0.0f, maxCount, easingcount);

		float firstposY = 1080 / 2;
		pos.y = Easing::EaseOutQuart(firstposY-1200, firstposY, maxCount, easingcount);

		if (easingcount >= maxCount)
		{
			toSmallEnd = true;
			toSmall = false;
		}
	}
	//‘å‚«‚­
	if (toBig)
	{
		int maxCount = 120;
		float maxScale = 20.0f;
		easingcount++;
		scale.x = Easing::EaseOutQuart(0.0f, maxScale, maxCount, easingcount);
		scale.y = Easing::EaseOutQuart(0.0f, maxScale, maxCount, easingcount);

		float firstposY = 1080 / 2;
		pos.y = Easing::EaseOutQuart(firstposY, firstposY - 1200, maxCount, easingcount);

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
