#pragma once
#include <glm/glm.hpp>
#include <vector>

using namespace std;

#define PI 3.14
#define RAD(x) ((x*PI)/180.0f)
#define TODEGREE(x) ((x*180.0f)/PI)
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(glm::vec3& p, glm::vec3& q, glm::vec3& r);

// Prints convex hull of a set of n points.
vector<glm::vec3> convexHull(glm::vec3 points[], int n);

double calculate_polygon_area(glm::vec3 points[], int n);
