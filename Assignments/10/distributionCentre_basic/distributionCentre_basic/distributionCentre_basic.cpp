// distributionCentre_basic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>

#define PI 3.141593
using namespace std;

// Structures
struct position {
	double latitude;
	double longitude;
};
struct settlement {
	//string name;
	//double type;
	int population;
	position pos;
};
struct corners {
	position LeftUpper;
	position LeftLower;
	position RightUpper;
	position RightLower;
};

// Functions
vector<string> getNextLineAndSplitIntoTokens(istream& str);
double haversineDistance(position Loc1, position Loc2);
double totalDistance(position base, vector <settlement> settlementList);
corners testBases(corners prevCorners, vector <settlement> settlementList);





int main() {

	ifstream dataFile("GBplaces.csv");
	vector <settlement> placeList;
	double degToRad = (PI / 180);

	if (dataFile.is_open()) {
		cout << "File loaded successfully\n";
		vector <string> list;
		settlement dataPoint;
		while (!dataFile.eof()) {										// Add elements to data structure
			list = getNextLineAndSplitIntoTokens(dataFile);

			dataPoint.population = stoi(list[2].c_str());
			dataPoint.pos.latitude = atof(list[3].c_str())*degToRad;
			dataPoint.pos.longitude = atof(list[4].c_str())*degToRad;
			placeList.push_back(dataPoint);

		}
		dataFile.close();

	}
	else {
		cout << "File could not be opened or does not exist\n";
		exit(1);
	}


	// 





	corners Corns;
	Corns.LeftUpper.latitude	= 58.4 * degToRad;		Corns.LeftUpper.longitude	= -10 * degToRad;
	Corns.RightLower.latitude	= 50 * degToRad;		Corns.RightLower.longitude	= 2.1 * degToRad;
	Corns.LeftLower.latitude	= 50 *degToRad;			Corns.LeftLower.longitude	= -10 * degToRad;
	Corns.RightUpper.latitude	= 58.4 * degToRad;		Corns.RightUpper.longitude	= 2.1 * degToRad;







	for (int j = 0; j < 200; j++) {
		Corns = testBases(Corns, placeList);

		
	}
		
	double averageLat  = (Corns.LeftUpper.latitude + Corns.RightLower.latitude) / 2;
	double averageLong = (Corns.LeftUpper.longitude + Corns.RightLower.longitude) / 2;

	cout << averageLat/degToRad << ", " << averageLong / degToRad << "\n";
	







    return 0;
}


vector<string> getNextLineAndSplitIntoTokens(istream& str) {
	vector<string>			result;
	string					line;
	getline(str, line);

	stringstream lineStream(line);
	string					cell;

	while (getline(lineStream, cell, ','))
	{
		result.push_back(cell);
	}
	// This checks for a trailing comma with no data after it.
	if (!lineStream && cell.empty())
	{
		// If there was a trailing comma then add an empty element.
		result.push_back("");
	}
	return result;
}
double haversineDistance(position Loc1, position Loc2) {



	double dLat = Loc2.latitude - Loc1.latitude;
	double dLong = Loc2.longitude - Loc1.longitude;

	double a = pow(sin(0.5*dLat), 2) + cos(Loc1.latitude)*cos(Loc2.latitude)*pow(sin(0.5*dLong), 2);
	double c = 2 * asin(pow(a, 0.5));
	double dist = 6371 * c;

	return dist;
	
}
double totalDistance(position base, vector <settlement> settlementList) {

	double sumDistance = 0;
	for (int i = 0; i < settlementList.size(); i++) {
		sumDistance += haversineDistance(base, settlementList[i].pos)/pow(settlementList[i].population,20);
	}
	return sumDistance;
}
corners testBases(corners prevCorners, vector <settlement> settlementList) {
	double distances[4];
	distances[0] = totalDistance(prevCorners.LeftUpper, settlementList);
	distances[1] = totalDistance(prevCorners.RightUpper, settlementList);
	distances[2] = totalDistance(prevCorners.RightLower, settlementList);
	distances[3] = totalDistance(prevCorners.LeftLower, settlementList);
	double minDistance = 10e100;
	int corner;
	for (int i = 0; i < 4; i++) {
		if (distances[i] < minDistance) {
			minDistance = distances[i];
			corner = i;

		}
	}
	corners newCorners = prevCorners;

	switch (corner) {
	case 0: 
		newCorners.LeftLower.latitude	= (prevCorners.LeftLower.latitude	+ prevCorners.LeftUpper.latitude) / 2;
		newCorners.RightLower.latitude = newCorners.LeftLower.latitude;
		newCorners.RightUpper.longitude = (prevCorners.RightUpper.longitude + prevCorners.LeftUpper.longitude) / 2;
		newCorners.RightLower.longitude = newCorners.RightUpper.longitude;
		break;
	
	case 1: 
		newCorners.LeftLower.latitude	= (prevCorners.LeftLower.latitude + prevCorners.LeftUpper.latitude) / 2;
		newCorners.RightLower.latitude	= newCorners.LeftLower.latitude;
		newCorners.LeftUpper.longitude	= (prevCorners.RightUpper.longitude + prevCorners.LeftUpper.longitude) / 2;
		newCorners.LeftLower.longitude	= newCorners.LeftUpper.longitude;
		break;

	case 2:
		newCorners.LeftUpper.latitude  = (prevCorners.LeftLower.latitude + prevCorners.LeftUpper.latitude) / 2;
		newCorners.RightUpper.latitude = newCorners.LeftUpper.latitude;
		newCorners.LeftUpper.longitude = (prevCorners.RightUpper.longitude + prevCorners.LeftUpper.longitude) / 2;
		newCorners.LeftLower.longitude = newCorners.LeftUpper.longitude;
		break;

	case 3:
		newCorners.LeftUpper.latitude	= (prevCorners.LeftLower.latitude + prevCorners.LeftUpper.latitude) / 2;
		newCorners.RightUpper.longitude = (prevCorners.RightUpper.longitude + prevCorners.LeftUpper.longitude) / 2;
		newCorners.RightLower.longitude	= newCorners.RightUpper.longitude;
		newCorners.RightUpper.latitude	= newCorners.LeftUpper.latitude;
		break;

	}

return newCorners;


}

