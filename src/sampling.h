#pragma once

#include "intersections.h"

//Computes a cosine-weighted random direction in a hemisphere around the provided surface normal
__host__ __device__ glm::vec3 calculateRandomDirectionInHemisphere(glm::vec3 normal, thrust::default_random_engine &rng) 
{
    thrust::uniform_real_distribution<float> u01(0, 1);

    float up = sqrt(u01(rng)); // cos(theta)
    float over = sqrt(1 - up * up); // sin(theta)
    float around = u01(rng) * TWO_PI;

    // Find a direction that is not the normal based off of whether or not the
    // normal's components are all equal to sqrt(1/3) or whether or not at
    // least one component is less than sqrt(1/3). Learned this trick from
    // Peter Kutz.

    glm::vec3 directionNotNormal;
    if (abs(normal.x) < SQRT_OF_ONE_THIRD) 
	{
        directionNotNormal = glm::vec3(1, 0, 0);
    } 
	else if (abs(normal.y) < SQRT_OF_ONE_THIRD) 
	{
        directionNotNormal = glm::vec3(0, 1, 0);
    } 
	else 
	{
        directionNotNormal = glm::vec3(0, 0, 1);
    }

    // Use not-normal direction to generate two perpendicular directions
    glm::vec3 perpendicularDirection1 =
        glm::normalize(glm::cross(normal, directionNotNormal));
    glm::vec3 perpendicularDirection2 =
        glm::normalize(glm::cross(normal, perpendicularDirection1));

    return up * normal
		+ cos(around) * over * perpendicularDirection1
        + sin(around) * over * perpendicularDirection2;
}

__host__ __device__ Point3f sampling_SquareToSphereUniform(Point2f &sample)
{
	float z = 1 - 2 * sample[0];
	float r = std::sqrt(std::max(0.0f, 1.0f - z*z));
	float phi = 2 * PI*sample[1];
	return Point3f(r*std::cos(phi), r* std::sin(phi), z);
}

__host__ __device__ Point3f sampling_SquareToDiskConcentric(Point2f &sample)
{
	glm::vec2 sampleOffset = 2.0f*sample - glm::vec2(1, 1);
	if (sampleOffset.x == 0 && sampleOffset.y == 0)
	{
		return Point3f(0.0f, 0.0f, 0.0f);
	}

	float theta, r;
	if (std::abs(sampleOffset.x) > std::abs(sampleOffset.y))
	{
		r = sampleOffset.x;
		theta = (PI / 4.0f) * (sampleOffset.y / sampleOffset.x);
	}
	else
	{
		r = sampleOffset.y;
		theta = (PI / 2.0f) - (PI / 4.0f) * (sampleOffset.x / sampleOffset.y);
	}
	return r*Point3f(std::cos(theta), std::sin(theta), 0.0f);
}

//http://corysimon.github.io/articles/uniformdistn-on-sphere/
__host__ __device__ Color3f SphereSample(const Point2f &sample)
{
	float theta = 2.f * PI * sample[0];
	float phi = glm::acos(1.f - 2.f * sample[1]);
	float x = glm::sin(phi) * glm::cos(theta);
	float y = glm::sin(phi) * glm::sin(theta);
	float z = glm::cos(phi);

	return glm::vec3(x, y, z);
}
