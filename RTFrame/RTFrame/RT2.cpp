/**
 * 两天学会光线追踪的C++版
 * 在MacOS上需要使用C++17编译，因为使用了std::clamp，其他系统还没有试过
*/

#define _USE_MATH_DEFINES
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <stddef.h>
#include <cmath>
#include <assert.h>

using namespace std;

#pragma region Float3
struct Float3
{
	float x;
	float y;
	float z;

	Float3();
	Float3(float ax, float ay, float az);

	void Set(float ax, float ay, float az);
	void Scale(float scale);
	void Scale(const Float3 &scale);
	float Length() const;
	void Normalize();

	Float3 operator+(const Float3& b) const;
	Float3 operator-(const Float3& b) const;
	Float3 operator*(float b) const;

	static float Dot(Float3 a, Float3 b);
	static Float3 Cross(Float3 a, Float3 b);
	static Float3 Lerp(Float3 a, Float3 b, float t);

	void Print() const;
	void PrintRGB(FILE* file) const;
};

Float3::Float3() :x(0), y(0), z(0)
{}

Float3::Float3(float ax, float ay, float az) : x(ax), y(ay), z(az)
{}

void Float3::Set(float ax, float ay, float az)
{
	x = ax;
	y = ay;
	z = az;
}

void Float3::Scale(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

void Float3::Scale(const Float3 &scale)
{
	x *= scale.x;
	y *= scale.y;
	z *= scale.z;
}

void Float3::Print() const
{
	printf("vec:(%f, %f, %f)\n", x, y, z);
}

float Float3::Length() const
{
	return sqrtf(x * x + y * y + z * z);
}

void Float3::Normalize()
{
	float len = Length();
	if (len <= 0.0001f)
	{
		x = 0;
		y = 0;
		z = 0;
	}
	else
	{
		Scale(1 / len);
	}
}

float Float3::Dot(Float3 a, Float3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Float3 Float3::Cross(Float3 a, Float3 b)
{
	return Float3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

Float3 Float3::Lerp(Float3 a, Float3 b, float t)
{
	return a + (b - a) * t;
}

Float3 Float3::operator+(const Float3 &b) const
{
	return Float3(x + b.x, y + b.y, z + b.z);
}

Float3 Float3::operator-(const Float3& b) const
{
	return Float3(x - b.x, y - b.y, z - b.z);
}

Float3 Float3::operator*(float b) const
{
	return Float3(x * b, y * b, z * b);
}

int clamp(int v, int min, int max)
{
	if (v < min)
	{
		return min;
	}

	if (v > max)
	{
		return max;
	}

	return v;
}

void Float3::PrintRGB(FILE* file) const
{
	int r = clamp(int((x + 1) * 0.5f * 255), 0, 255);
	int g = clamp(int((y + 1) * 0.5f * 255), 0, 255);
	int b = clamp(int((z + 1) * 0.5f * 255), 0, 255);
	fprintf(file, "%d %d %d\n", r, g, b);
}
#pragma endregion

#pragma region Ray
struct Ray
{
	Float3 origin;//起点
	Float3 direction;//方向

	Ray();
	Ray(const Float3 &origin, const Float3 &direction);
	Float3 GetPoint(float t) const;
};

Ray::Ray()
{

}

Ray::Ray(const Float3 &origin, const Float3 &direction)
{
	this->origin = origin;
	this->direction = direction;
}

Float3 Ray::GetPoint(float t) const
{
	return origin + direction * t;
}
#pragma endregion

#pragma region Hit
struct Hit;

class Material
{
public:
	virtual void Scatter(const Ray &rayIn, const Hit& hit, Float3 *attenuation, Ray* scatterRay) = 0;
};

struct Hit
{
	float t;
	Float3 point;
	Float3 normal;
	Material *mat;
};

class IHitable
{
public:
	virtual bool HitTest(const Ray &ray, Hit *pHit) = 0;
};

class HitableList : public IHitable
{
	vector<IHitable*> _hitables;

public:
	void Add(IHitable* obj);
	virtual bool HitTest(const Ray &ray, Hit *pHit);
};

void HitableList::Add(IHitable* obj)
{
	_hitables.push_back(obj);
}

bool HitCompare(const Hit &a, const Hit &b)
{
	return a.t < b.t;
}

bool HitableList::HitTest(const Ray &ray, Hit *pHit)
{
	vector<Hit> hits;
	for (auto it = _hitables.begin(); it != _hitables.end(); it++)
	{
		Hit hit;
		if ((*it)->HitTest(ray, &hit))
		{
			hits.push_back(hit);
		}
	}

	if (hits.size() > 0)
	{
		auto min = min_element(hits.begin(), hits.end(), HitCompare);
		*pHit = *min;

		return true;
	}

	return false;
}
#pragma endregion

class Cube : public IHitable
{
private:
	Float3 center;
	float sideLength;
	Material* mat;

public:
	Cube(Float3 center, float sideLength, Material *mat);
	//virtual bool HitTest(const Ray &ray, Hit *pHit);
};

Cube::Cube(Float3 center, float sideLength, Material * mat)
{
	this->center = center;
	this->sideLength = sideLength;
	this->mat = mat;
}

#pragma region Sphere

class Sphere : public IHitable
{
private:
	Float3 center;
	float radius;
	Material* mat;

public:
	Sphere(Float3 center, float radius, Material *mat);
	virtual bool HitTest(const Ray &ray, Hit *pHit);
};

Sphere::Sphere(Float3 center, float radius, Material *mat)
{
	this->center = center;
	this->radius = radius;
	this->mat = mat;
}

bool Sphere::HitTest(const Ray &ray, Hit *pHit)
{
	assert(pHit != 0);

	// dot(ray.direction * t + ray.origin - center) = radius * radius
	// dot(ray.direction * t) + 2 * dot(ray.direction * t, ray.origin - center) + dot(ray.origin - center) = radius * radius

	// ax2 + bx + c = 0; x = (-b +- sqrt(b2 - 4ac))/2a

	Float3 oc = ray.origin - center;
	float a = Float3::Dot(ray.direction, ray.direction);
	float b = Float3::Dot(ray.direction, oc) * 2;
	float c = Float3::Dot(oc, oc) - radius * radius;

	float delta = b * b - 4 * a * c;
	if (delta > 0)
	{
		float t = (-b - sqrtf(delta)) / 2 / a; // 因为a>0, 所以较小的解一定是 + 

		/*
		if (t <= 0)
		{
			t = (-b + sqrtf(delta)) / 2 / a;
		}
		*/

		if (t > 0.001f)
		{
			pHit->t = t;
			pHit->mat = mat;
			pHit->point = ray.GetPoint(t);
			pHit->normal = pHit->point - center;
			pHit->normal.Normalize();
			return true;
		}
	}

	return false;
}
#pragma endregion

Float3 GetDefaultSkyboxColor(const Float3 &normal)
{
	float y = normal.y / normal.Length();
	float t = 0.5f * (y + 1.0f);
	return Float3::Lerp(Float3(1, 1, 1), Float3(0.3f, 0.5f, 1.0f), t);
}

#pragma region Camera
class Camera
{
private:
	Float3 position;
	Float3 topleft;
	Float3 horizontal;  // 向右
	Float3 vertical;    // 向下

public:
	Camera(Float3 position, Float3 lookat, Float3 up, float near, float fov, float aspect);


	// xRate, yRate, 0 - 1, y从上到下
	Ray GetRay(float xRate, float yRate) const;
};

Camera::Camera(Float3 position, Float3 lookat, Float3 up, float near, float fov, float aspect)
{
	this->position = position;
	Float3 direction = lookat - position;
	direction.Normalize();
	printf("direction"); direction.Print();

	Float3 right = Float3::Cross(up, direction);
	right.Normalize();
	printf("right"); right.Print();

	Float3 down = Float3::Cross(right, direction);
	printf("down"); down.Print();

	float tanHalfFov = tanf(fov * 0.5f / 180 * M_PI);
	float height = near * tanHalfFov * 2;
	float width = height * aspect;

	horizontal = right * width;
	vertical = down * height;
	topleft = position + direction * near - horizontal * 0.5f - vertical * 0.5f;
}

Ray Camera::GetRay(float xRate, float yRate) const
{
	Float3 from = topleft + horizontal * xRate + vertical * yRate;
	Float3 direction = from - position;
	direction.Normalize();
	return Ray(from, direction);
}
#pragma endregion

void SavePPM(Float3* pixels, int width, int height, const char *filename)
{
	FILE *ppmFile = fopen(filename, "w+");

	fprintf(ppmFile, "P3\n");
	fprintf(ppmFile, "%d %d\n", width, height);
	fprintf(ppmFile, "255\n");

	int len = width * height;
	for (int i = 0; i < len; ++i)
	{
		pixels[i].PrintRGB(ppmFile);
	}

	fclose(ppmFile);
}

void SimpleRTFrame(
	const Camera &camera,
	IHitable* hitable,
	int width,
	int height,
	const char* filename,
	Float3(*rtfunc)(const Ray&, IHitable*))
{

	Float3* screen = new Float3[height * width];

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			float xRate = float(i) / width;
			float yRate = float(j) / height;


			Ray ray = camera.GetRay(xRate, yRate);
			screen[j * width + i] = rtfunc(ray, hitable);
		}
	}

	SavePPM(screen, width, height, filename);

	delete[] screen;
}

void AntialiasingRTFrame(
	const Camera &camera,
	IHitable* hitable,
	int width,
	int height,
	const char* filename,
	Float3(*rtfunc)(const Ray&, IHitable*))
{

	Float3* screen = new Float3[height * width];

	const int SAMPLECOUNT = 5;

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{

			float xRate = float(i) / width;
			float yRate = float(j) / height;

			Float3 sum;

			for (int s = 0; s < SAMPLECOUNT; ++s)
			{
				float randXRate = xRate + (rand() % 1000 / 1000.0f - 0.5f) / width;
				float randYRate = yRate + (rand() % 1000 / 1000.0f - 0.5f) / height;

				Ray ray = camera.GetRay(randXRate, randYRate);
				sum = sum + rtfunc(ray, hitable);
			}

			screen[j * width + i] = sum * (1.0f / SAMPLECOUNT);
		}
	}

	SavePPM(screen, width, height, filename);

	delete[] screen;
}

Float3 GetRandom()
{
	Float3 re(rand() % 1000 / 500.0f - 1,
		rand() % 1000 / 500.0f - 1,
		rand() % 1000 / 500.0f - 1);
	return re;
}

Float3 RayTraceForLambert(const Ray &ray, IHitable *hitable)
{
	Hit hit;
	if (hitable->HitTest(ray, &hit))
	{
		Float3 randVec = GetRandom() * 0.7f;
		Float3 refdir = hit.normal + randVec;
		Ray nextRay(hit.point, refdir);
		Float3 nextColor = RayTraceForLambert(nextRay, hitable);
		nextColor.Scale(Float3(0.5f, 0.5f, 0.5f));
		return nextColor;
	}

	return GetDefaultSkyboxColor(ray.direction);
}

class LambertMaterial : public Material
{
	Float3 albedo;
public:
	LambertMaterial(const Float3 &albedo);
	virtual void Scatter(const Ray &rayIn, const Hit& hit, Float3 *attenuation, Ray* scatterRay);
};

LambertMaterial::LambertMaterial(const Float3 &albedo)
{
	this->albedo = albedo;
}

void LambertMaterial::Scatter(const Ray &rayIn, const Hit& hit, Float3 *attenuation, Ray* scatterRay)
{
	*attenuation = albedo;

	Float3 randVec = GetRandom() * 0.7f;
	Float3 refdir = hit.normal + randVec;
	*scatterRay = Ray(hit.point, refdir);
}

class MetalMaterial : public Material
{
public:
	float fuzz;
	MetalMaterial(float fuzz = 0);
	virtual void Scatter(const Ray &rayIn, const Hit& hit, Float3 *attenuation, Ray* scatterRay);
};

MetalMaterial::MetalMaterial(float fuzz)
{
	this->fuzz = fuzz;
}

void MetalMaterial::Scatter(const Ray &rayIn, const Hit& hit, Float3 *attenuation, Ray* scatterRay)
{
	*attenuation = Float3(0.99f, 0.99f, 0.99f);
	Float3 refdir = rayIn.direction + (hit.normal * 2) + (GetRandom() * fuzz);
	*scatterRay = Ray(hit.point, refdir);
}

Float3 RayTraceWithMaterialRec(const Ray &ray, IHitable *hitable, int depth)
{
	if (depth <= 0)
	{
		return Float3(0, 0, 0);
	}

	Hit hit;
	if (hitable->HitTest(ray, &hit))
	{
		Float3 att;
		Ray nextRay;

		hit.mat->Scatter(ray, hit, &att, &nextRay);
		Float3 nextColor = RayTraceWithMaterialRec(nextRay, hitable, depth - 1);

		nextColor.Scale(att);
		return nextColor;
	}

	return GetDefaultSkyboxColor(ray.direction);
}

Float3 RayTraceWithMaterial(const Ray &ray, IHitable *hitable)
{
	return RayTraceWithMaterialRec(ray, hitable, 5);
}

void TestRTFrame()
{
	int width = 1024;
	int height = 512;

	LambertMaterial lambertGray(Float3(0.5f, 0.5f, 0.5f));
	LambertMaterial lambertRed(Float3(1.0f, 0.5f, 0.5f));
	MetalMaterial metal(0.02f);
	MetalMaterial metalGround(0.3f);

	Camera camera(Float3(0, 1, -4), Float3(0, 0, 0), Float3(0, 1, 0), 1, 45, width / (float)height);
	HitableList hitables;

	Sphere sphere1(Float3(0, 0, 0), 1, &lambertGray);
	hitables.Add(&sphere1);

	Sphere sphere2(Float3(-2.5, 0, 1), 1, &lambertRed);
	hitables.Add(&sphere2);

	Sphere sphere3(Float3(2.5f, 0, 2), 1, &metal);
	hitables.Add(&sphere3);

	Cube cube1(Float3(1, 1, 1), 1, &metal);
	hitables.Add(cube1);

	Sphere sphere0(Float3(0, -1000, 0), 999, &metalGround);
	hitables.Add(&sphere0);

	SimpleRTFrame(camera, &hitables, width, height, "testLambert.ppm", RayTraceForLambert);
	AntialiasingRTFrame(camera, &hitables, width, height, "testLambertAL.ppm", RayTraceForLambert);
	AntialiasingRTFrame(camera, &hitables, width, height, "testLambertAA.ppm", RayTraceWithMaterial);
}

int main()
{
	//printf("Hello World! %d\n", _LIBCPP_STD_VER);
	TestRTFrame();

	system("pause");
	return 0;
}

