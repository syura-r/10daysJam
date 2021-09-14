#pragma once
class DrawMode
{
public:
	enum MODE
	{
		Bloom,
		NormalMap,
	};
	static void SetMode(const MODE mode) { DrawMode::mode = mode; }
	static const MODE& GetMode() { return mode; }
private:
	static MODE mode;
};

