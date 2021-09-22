#pragma once
#include "Object.h"
#include "OBJModel.h"

class MapBox :
    public Object
{
public:
	MapBox(OBJModel* model, const Vector3& position = {}, const Vector3& scale = { 1,1,1 }, const Vector3& rotation = {}, const Vector3& colliderScale = {1,1,1});
	void Draw() override;
};

