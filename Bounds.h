#ifndef BOUNDS_H
#define BOUNDS_H
#include<glm.hpp>
#include<iostream>


enum class BoundTypes {
	AABB=0x00,//Axis-Aligned bounding box
	SPHERE=0x01
	//takes less memory 


};

class BoundingRegion {
public:
	BoundTypes type;

	//Sphere values
	glm::vec3 center;
	float radius;

	//AABB values
	glm::vec3 min;
	glm::vec3 max;

	/*
		Constructors
	*/
	BoundingRegion();
	//initialize as Type
	BoundingRegion(BoundTypes type);

	//initialize as Sphere
	BoundingRegion(glm::vec3 center, float radius);

	//initialize as AABB
	BoundingRegion(glm::vec3 min, glm::vec3 max);

	/*
		Calculating values for the region
	*/

	//center
	glm::vec3 calculateCenter();

	//calculate dimensions
	glm::vec3 calculateDimensions();

	/*
		testing methods
	*/

	//determine if point inside
	bool containsPoint(glm::vec3 point);

	//determine if region completely inside
	bool containsRegion(BoundingRegion region);

	//determine if region intersects
	bool intersectsWith(BoundingRegion region);


};



#endif // !BOUNDS_H

