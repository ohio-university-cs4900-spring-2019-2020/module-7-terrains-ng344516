#include "WOPhysXTerrain.h"
#include <iostream>

using namespace Aftr;


WOPhysXTerrain* WOPhysXTerrain::New(const VectorD& upperLeft, const VectorD& lowerRight, unsigned int granularity, const VectorD& offset, const VectorD& scale, std::string elevationData, int splits, float exageration, bool useColors) {
	WOPhysXTerrain* grid = new WOPhysXTerrain();
	grid->onCreate(upperLeft, lowerRight, granularity, offset, scale, elevationData, splits, exageration, useColors);
	return grid;
}

WOPhysXTerrain::WOPhysXTerrain() : IFace(this), WOGridECEFElevation() {
}

void WOPhysXTerrain::onCreate(const VectorD& upperLeft, const VectorD& lowerRight, unsigned int granularity, const VectorD& offset, const VectorD& scale, std::string elevationData, int splits = 2, float exageration = 0, bool useColors = false) {
	WOGridECEFElevation::onCreate(upperLeft, lowerRight, 0, offset, scale, elevationData, splits, exageration, useColors);
}