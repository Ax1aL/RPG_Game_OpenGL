#include "Bounds.h"


/*
	Constructors
*/

BoundingRegion::BoundingRegion()
{
	type = BoundTypes::AABB;
	min = max = glm::vec3(0.f);
}

//initialize as Type
BoundingRegion::BoundingRegion(BoundTypes type) :type(type) {};

//initialize as Sphere
BoundingRegion::BoundingRegion(glm::vec3 center, float radius) : type(BoundTypes::SPHERE), center(center), radius(radius) {};

//initialize as AABB
BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max) :type(BoundTypes::AABB), min(min), max(max) {};

/*
	Calculating values for the region
*/

//center
glm::vec3 BoundingRegion::calculateCenter(){
	return (type == BoundTypes::AABB) ? (min + max) / 2.0f : center;
}

//calculate dimensions
glm::vec3 BoundingRegion::calculateDimensions(){
	return (type == BoundTypes::AABB) ? (max - min) : glm::vec3(2.0f * radius);
}

/*
	testing methods
*/

//determine if point inside
bool BoundingRegion::containsPoint(glm::vec3 point) {
	if (type == BoundTypes::AABB) {
		//box-point must be larger than min and lower than max
		return (point.x >= min.x) && (point.x <= max.x) &&
			(point.y >= min.y) && (point.y <= max.y) &&
			(point.z >= min.z) && (point.z <= max.z);

	}
	else {
		//sphere -distance must be less han radius
		//x^2+y^2+z^2<=r^2
		float distSquare = 0.0f;
		for (int i = 0; i < 3; ++i) {
			distSquare += (center[i]-point[i]) * (center[i] - point[i]);
		}
		return distSquare <= radius * radius;
	}
	std::cout << "Error Collision!";
}

//determine if region completely(with center) inside
bool BoundingRegion::containsRegion(BoundingRegion region) {
	if (region.type == BoundTypes::AABB) {
		//if region is a box,just has to contain min and max 
		return containsPoint(region.min) && containsPoint(region.max);
	}
	else if (type == BoundTypes::SPHERE && region.type == BoundTypes::SPHERE) {
		//if both spheres,combination if distance from center and region.redius is less than radius
		return (glm::length(center - region.center) + region.radius) < radius;
	}
	else {
		//if this is box,region is Sphere

		if (!containsPoint(region.center)) {
			return false;
		}
		//center inside the box
		for (int i = 0; i < 3; ++i) {
			if (abs(max[i] - region.center[i]) < region.radius ||
				abs(region.center[i] - min[i]) < region.radius) {
				return false;
			}
		}

		return true;
	}
	std::cout << "Error Collision!";
	return false;
}

//determine if region intersects
bool BoundingRegion::intersectsWith(BoundingRegion region) {
	//overlap on all axis

	if (type == BoundTypes::AABB && region.type == BoundTypes::AABB) {
		//both boxes-overlap on all axis

		glm::vec3 rad = calculateDimensions() / 2.0f;//"radius" of this box
		glm::vec3 radRegion = region.calculateDimensions() / 2.0f; //"radius" of region

		glm::vec3 center = calculateCenter();//center of this
		glm::vec3 centerRegion = region.calculateCenter();//center of region

		glm::vec3 dist = center - centerRegion;
		for (int i = 0; i < 3; ++i) {
			dist[i] = abs(dist[i]);//convert to positive for the distance
			if (dist[i] > rad[i] + radRegion[i]) {
				return false;
			}
		}
		return true;
	}
	else if (type == BoundTypes::SPHERE && region.type == BoundTypes::SPHERE) {
		//both spheres-distance between centers must be less thancombied radius
		return glm::length(center - region.center) < (radius + region.radius);
	}
	else if(type==BoundTypes::SPHERE) {
		//this is Sphere,region is box

		//determine if sphere is above the top,below,etc
		//find distance(squred) to closest plane
		float distSquared = 0.0f;
		for (int i = 0; i < 3; ++i) {
			if (center[i] < region.min[i]) {
				distSquared += (region.min[i] - center[i]) * (region.min[i] - center[i]);
			}
			else if (center[i] > region.max[i]) {
				distSquared += (center[i] - region.max[i]) * (center[i] - region.max[i]);
			}
			//else inside
		}
		return distSquared < radius* radius;
	}
	else {
		//this is Box,region is sphere
		//call the preciding else if statment but inverse
		return region.intersectsWith(*this);
	}
	std::cout << "Error Collision!";
	return false;
}


