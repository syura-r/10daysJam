#include "Collision.h"
Window* Collision::window = nullptr;
using namespace DirectX;
#include<cmath>
void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR & point, const Triangle & triangle, DirectX::XMVECTOR * closest)
{
	//pointがp0の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	XMVECTOR p0_pt = point - triangle.p0;

	float d1 = XMVector3Dot(p0_p1, p0_pt).m128_f32[0];
	float d2 = XMVector3Dot(p0_p2, p0_pt).m128_f32[0];

	if (d1 <= 0.0f&&d2 <= 0.0f)
	{
		//p0が最近値
		*closest = triangle.p0;
		return;
	}

	//pointがp1の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p1_pt = point - triangle.p1;

	float d3 = XMVector3Dot(p0_p1, p1_pt).m128_f32[0];
	float d4 = XMVector3Dot(p0_p2, p1_pt).m128_f32[0];

	if (d3 > -0.0f&&d4 <= d3)
	{
		//p1が最近傍
		*closest = triangle.p1;
		return;
	}

	//pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0 && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	//pointがp2の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p2_pt = point - triangle.p2;

	float d5 = XMVector3Dot(p0_p1, p2_pt).m128_f32[0];
	float d6 = XMVector3Dot(p0_p2, p2_pt).m128_f32[0];
	if (d6 >= 0.0f && d5 <= d6)
	{
		*closest = triangle.p2;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	// pointは面領域の中にある。closestを重心座標を用いて計算する
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;

}
bool Collision::CheckSphereBox(const Sphere& sphere, const Box& box, XMVECTOR* inter, XMVECTOR* reject)
{

	XMVECTOR rejectVec = XMVectorZero();
	float sqDistance = 0.0f;
	float d;

	d = sphere.center.m128_f32[0];
	if (d < box.minPosition.x)
	{
		sqDistance += (d - box.minPosition.x) * (d - box.minPosition.x);
		rejectVec.m128_f32[0] = box.minPosition.x - (d + sphere.radius);
	}
	if (d > box.maxPosition.x)
	{
		sqDistance += (d - box.maxPosition.x) * (d - box.maxPosition.x);
		rejectVec.m128_f32[0] =  box.maxPosition.x - (d - sphere.radius);
	}

	d = sphere.center.m128_f32[1];
	if (d < box.minPosition.y)
	{
		sqDistance += (d - box.minPosition.y) * (d - box.minPosition.y);
		rejectVec.m128_f32[1] = box.minPosition.y - (d + sphere.radius);
	}
	if (d > box.maxPosition.y)
	{
		sqDistance += (d - box.maxPosition.y) * (d - box.maxPosition.y);
		rejectVec.m128_f32[1] = box.maxPosition.y - (d - sphere.radius);

	}
	d = sphere.center.m128_f32[2];
	if (d < box.minPosition.z)
	{
		sqDistance += (d - box.minPosition.z) * (d - box.minPosition.z);
		rejectVec.m128_f32[2] = box.minPosition.z - (d + sphere.radius);
	}
	if (d > box.maxPosition.z)
	{
		sqDistance += (d - box.maxPosition.z) * (d - box.maxPosition.z);
		rejectVec.m128_f32[1] = box.maxPosition.z - (d - sphere.radius);
	}

	if (sqDistance > sphere.radius * sphere.radius)
		return false;


	if (reject)
	{
		*reject = rejectVec;
	}
	if(inter)
	{
		//合っているのか...?
		*inter = (sphere.center + rejectVec) +  XMVector3Normalize(-rejectVec) * sphere.radius;
	}
	return true;


}
bool Collision::CheckBoxBox(const Box& box1, const Box& box2, XMVECTOR* inter, XMVECTOR* reject)
{
	if (box2.maxPosition.x >= box1.minPosition.x && box2.minPosition.x <= box1.maxPosition.x &&
		box2.maxPosition.y >= box1.minPosition.y && box2.minPosition.y <= box1.maxPosition.y &&
		box2.maxPosition.z >= box1.minPosition.z && box2.minPosition.z <= box1.maxPosition.z)
	{
		if (inter)
		{
			Vector3 collPos = {};
			collPos.x = box2.maxPosition.x - ((box2.maxPosition.x - box1.minPosition.x) / 2);
			collPos.y = box2.maxPosition.y - ((box2.maxPosition.y - box1.minPosition.y) / 2);
			collPos.z = box2.maxPosition.z - ((box2.maxPosition.z - box1.minPosition.z) / 2);
			*inter = XMVectorSet(collPos.x, collPos.y, collPos.z, 1);
		}
		if(reject)
		{
			//考え中(移動ベクトルないと無理じゃない？)
			//とりあえず2Dでの処理用にXとY軸で重なりの大きい方向を排斥ベクトルとする
			auto a = box2.maxPosition - box1.minPosition;
			auto b = box2.minPosition - box1.maxPosition;
			Vector3 rejectVec = {};
			if (abs(a.x) < abs(b.x))
				rejectVec.x = a.x;
			else
				rejectVec.x = b.x;
			if (abs(a.y) < abs(b.y))
				rejectVec.y = a.y;
			else
				rejectVec.y = b.y;
			if (abs(rejectVec.x) < abs(rejectVec.y))
				rejectVec.y = 0;
			else
				rejectVec.x = 0;

			*reject = XMVectorSet(rejectVec.x, rejectVec.y, rejectVec.z, 1);
		}
		return true;
	}
	return false;
}

bool Collision::CheckSphere2Sphere(const Sphere & sphereA, const Sphere & sphereB, DirectX::XMVECTOR * inter, DirectX::XMVECTOR * reject)
{
	// 中心点の距離の２乗 <= 半径の和の２乗　なら交差
	float dist = XMVector3LengthSq(sphereA.center - sphereB.center).m128_f32[0];

	float radius2 = sphereA.radius + sphereB.radius;
	radius2 *= radius2;

	if (dist <= radius2) {
		if (inter) {
			// Aの半径が0の時座標はBの中心　Bの半径が0の時座標はAの中心　となるよう補完
			float t = sphereB.radius / (sphereA.radius + sphereB.radius);
			*inter = XMVectorLerp(sphereA.center, sphereB.center, t);
		}
		//押し出すベクトルを計算
		if (reject)
		{
			float rejectLen = sphereA.radius + sphereB.radius - sqrt(dist);
			*reject = XMVector3Normalize(sphereA.center - sphereB.center);
			*reject *= rejectLen;
		}
		return true;
	}

	return false;
}

bool Collision::CheckSphere2Plane(const Sphere & sphere, const Plane & plane, DirectX::XMVECTOR * inter)
{
	// 座標系の原点から球の中心座標への距離から
	// 平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = XMVector3Dot(sphere.center, plane.normal).m128_f32[0] - plane.distance;
	// 距離の絶対値が半径より大きければ当たっていない
	if (fabsf(dist) > sphere.radius)	return false;

	// 擬似交点を計算
	if (inter)
	{
		// 平面上の再接近点を、疑似交点とする
		*inter = -dist * plane.normal + sphere.center;
	}
	return true;
}

bool Collision::CheckSphere2Triangle(const Sphere & sphere, const Triangle & triangle, DirectX::XMVECTOR * inter, DirectX::XMVECTOR * reject)
{
	XMVECTOR p;
	// 球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	// 点pと球の中心の差分ベクトル
	XMVECTOR v = p - sphere.center;
	// 距離の二乗を求める
	//（同じベクトル同士の内積は三平方の定理のルート内部の式と一致する）
	float distanceSquare = XMVector3Dot(v, v).m128_f32[0];
	// 球と三角形の距離が半径以下なら当たっていない
	if (distanceSquare > sphere.radius * sphere.radius)	return false;
	// 擬似交点を計算
	if (inter) {
		// 三角形上の最近接点pを疑似交点とする
		*inter = p;
	}
	//押し出すベクトルを計算
	if (reject)
	{
		float ds = XMVector3Dot(sphere.center, triangle.normal).m128_f32[0];
		float dt = XMVector3Dot(triangle.p0, triangle.normal).m128_f32[0];
		float rejectLen = dt - ds + sphere.radius;
		*reject = triangle.normal*rejectLen;
	}
	return true;
}

bool Collision::CheckRay2Plane(const Ray & lay, const Plane & plane, float * distance, DirectX::XMVECTOR * inter)
{
	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値

	float d1 = XMVector3Dot(plane.normal, lay.dir).m128_f32[0];
	// 裏面には当たらない
	if (d1 > -epsilon) {
		return false;
	}

	float d2 = XMVector3Dot(plane.normal, lay.start).m128_f32[0];
	float t = (plane.distance - d2) / d1;

	if (t < 0) return false;

	// 距離を書き込む
	if (distance) {
		*distance = t;
	}

	// 交点を計算
	if (inter) {
		*inter = lay.start + t * lay.dir;
	}

	return true;
}

bool Collision::CheckRay2Triangle(const Ray & lay, const Triangle & triangle, float * distance, DirectX::XMVECTOR * inter)
{
	// 三角形が乗っている平面を算出
	Plane plane;
	XMVECTOR interPlane;
	plane.normal = triangle.normal;
	plane.distance = XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];
	// レイと平面が当たっていなければ、当たっていない	
	if (!CheckRay2Plane(lay, plane, distance, &interPlane)) {
		return false;
	}
	// レイと平面が当たっていたので、距離と交点が書き込まれた

	// レイと平面の交点が三角形の内側にあるか判定
	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値
	XMVECTOR m;
	// 辺p0_p1について
	XMVECTOR pt_p0 = triangle.p0 - interPlane;
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);
	// 辺の外側
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	// 辺p1_p2について
	XMVECTOR pt_p1 = triangle.p1 - interPlane;
	XMVECTOR p1_p2 = triangle.p2 - triangle.p1;
	m = XMVector3Cross(pt_p1, p1_p2);
	// 辺の外側
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	// 辺p2_p0について
	XMVECTOR pt_p2 = triangle.p2 - interPlane;
	XMVECTOR p2_p0 = triangle.p0 - triangle.p2;
	m = XMVector3Cross(pt_p2, p2_p0);
	// 辺の外側
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {
		return false;
	}

	if (inter) {
		*inter = interPlane;
	}

	// 内側なので、当たっている
	return true;
}

bool Collision::CheckRay2Sphere(const Ray & lay, const Sphere & sphere, float * distance, DirectX::XMVECTOR * inter)
{
	XMVECTOR m = lay.start - sphere.center;
	float b = XMVector3Dot(m, lay.dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;
	// layの始点がsphereの外側にあり(c > 0)、layがsphereから離れていく方向を
	// 差している場合(b > 0)、当たらない
	if (c > 0.0f && b > 0.0f) {
		return false;
	}

	float discr = b * b - c;
	// 負の判別式はレイが球を外れていることに一致
	if (discr < 0.0f) {
		return false;
	}

	// レイは球と交差している。
	// 交差する最小の値tを計算
	float t = -b - sqrtf(discr);
	// tが負である場合、レイは球の内側から開始しているのでtをゼロにクランプ
	if (t < 0) t = 0.0f;

	if (distance) {
		*distance = t;
	}

	if (inter) {
		*inter = lay.start + t * lay.dir;
	}

	return true;
}

bool Collision::CheckRay2Box(const Ray& lay, const Box& box, float* distance, DirectX::XMVECTOR* inter)
{

	if (box.center.m128_f32[1] > lay.start.m128_f32[1])
		return false;
	
	// 交差判定
	float p[3], d[3], min[3], max[3];
	memcpy(p, &lay.start, sizeof(Vector3));
	memcpy(d, &lay.dir, sizeof(Vector3));
	memcpy(min, &box.minPosition, sizeof(Vector3));
	memcpy(max, &box.maxPosition, sizeof(Vector3));

	float t = -FLT_MAX;
	float t_max = FLT_MAX;

	for (int i = 0; i < 3; ++i) {
		if (abs(d[i]) < FLT_EPSILON) {
			if (p[i] < min[i] || p[i] > max[i])
				return false; // 交差していない
		}
		else {
			// スラブとの距離を算出
			// t1が近スラブ、t2が遠スラブとの距離
			float odd = 1.0f / d[i];
			float t1 = (min[i] - p[i]) * odd;
			float t2 = (max[i] - p[i]) * odd;
			if (t1 > t2) {
				float tmp = t1; t1 = t2; t2 = tmp;
			}

			if (t1 > t) t = t1;
			if (t2 < t_max) t_max = t2;

			// スラブ交差チェック
			if (t >= t_max)
				return false;
		}
	}
	*distance = t;
	// 交差している
	if (inter) {
		*inter = lay.start + t * (lay.dir);
	}

	return true;
}
