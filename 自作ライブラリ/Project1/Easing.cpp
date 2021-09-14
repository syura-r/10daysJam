#include "Easing.h"
#include<math.h>

float Easing::Lerp(const float & start, const float & end, const float& t)
{
	return start * (1.0f - t) + end * t;
}

float Easing::EaseInCubic(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (t * t * t));
}

float Easing::EaseOutCubic(const float & start, const float & end, const float & count, const float & time)
{	
	return  Lerp(start, end, 1 - powf(1 - time, 3));
}

float Easing::EaseInOutCubic(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, t < 0.5 ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2);
}

float Easing::EaseInQuart(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (t * t * t * t));
}

float Easing::EaseOutQuart(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (1 - powf(1 - t, 4)));
}

float Easing::EaseInOutQuart(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (t < 0.5 ? 8 * t * t * t * t : 1 - powf(-2 * t + 2, 4) / 2));
}

float Easing::EaseInQuint(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (t * t * t * t * t));
}

float Easing::EaseOutQuint(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (1 - powf(1 - t, 5)));
}

float Easing::EaseInOutQuint(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (t < 0.5 ? 16 * t * t * t * t * t : 1 - powf(-2 * t + 2, 5) / 2));
}

float Easing::EaseInExpo(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (t == 0 ? 0 : powf(2, 10 * t - 10)));
}

float Easing::EaseOutExpo(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (t == 1 ? 1 : 1 - powf(2, -10 * t)));
}

float Easing::EaseInOutExpo(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (t == 0? 0: t == 1? 1: t < 0.5 ? powf(2, 20 * t - 10) / 2: (2 - powf(2, -20 * t + 10)) / 2));
}

float Easing::EaseInCirc(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (1 - sqrt(1 - powf(t, 2))));
}

float Easing::EaseOutCirc(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (sqrt(1 - powf(t - 1, 2))));
}

float Easing::EaseInOutCirc(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	return Lerp(start, end, (t < 0.5? (1 - sqrt(1 - powf(2 * t, 2))) / 2: (sqrt(1 - powf(-2 * t + 2, 2)) + 1) / 2));
}

float Easing::EaseInBack(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	const float c1 = 1.70158f;
	const float c2 = c1 + 1;
	return Lerp(start, end, (c2 * t * t * t - c1 * t * t));
}

float Easing::EaseOutBack(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	const float c1 = 1.70158f;
	const float c2 = c1 + 1;

	return Lerp(start, end, (1 + c2 * powf(t - 1, 3) + c1 * powf(t - 1, 2)));
}

float Easing::EaseInOutBack(const float & start, const float & end, const float & count, const float & time)
{
	float t = time;
	t /= count;
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;
	return Lerp(start, end, (t < 0.5? (powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
		: (powf(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2));
}
