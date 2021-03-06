// distributionCentre_angle.cpp : Defines the entry point for the console application.
//


// Packages
#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>

// Definitions
#define PI 3.141593
//#define NAN "thefuckdoiknowaboutcoding"

// Settings
using namespace std;

// Structures
struct position {
	double latitude;
	double longitude;
};
struct point {
	string name;
	position pos;
	vector <double> relDistance;
	double angle;
	double relPopulation;
	double demand;
};
struct truck {
	double supply;
	vector <string> routeNames;
	vector <int> route;

};

// Functions
vector<string> getNextLineAndSplitIntoTokens(istream& str);
double haversineDistance(point Loc1, point Loc2);
vector<double> relativeDistance(vector <point> List, int i);
bool sortByAngle(point i, point j);
double minimumTravellingSalesman(vector <int> route, vector <point> placeList);


int main() {
	point baseLocation;
	double degToRad = (PI / 180);
	double radToDeg = (180 / PI);
	baseLocation.name = "Base"; 
	baseLocation.pos.latitude = 52.634*degToRad;
	baseLocation.pos.longitude = -1.444*degToRad;
	baseLocation.angle = 0;
	baseLocation.relPopulation = 0;
	baseLocation.demand = 0;

	/// Import data as a vector structure

	//string fileName;
	//cout << "Which file would you like to load (use full file name)?\n";
	//cin >> fileName;
	//ifstream dataFile(fileName);										// Load file which has been selected  
	ifstream dataFile("GBplaces.csv");
	vector <point> placeList;

	if (dataFile.is_open()) {
		cout << "File loaded successfully\n";
		vector <string> list;
		point dataPoint;
		placeList.push_back(baseLocation);
		while (!dataFile.eof()) {										// Add elements to data structure
			list = getNextLineAndSplitIntoTokens(dataFile);
			dataPoint.name = list[0];
			dataPoint.relPopulation = ceil(atof(list[2].c_str()) / 100000);		dataPoint.demand = dataPoint.relPopulation;
			dataPoint.pos.latitude = atof(list[3].c_str())*degToRad;			dataPoint.pos.longitude = atof(list[4].c_str())*degToRad;
			dataPoint.angle = atan2((dataPoint.pos.latitude - baseLocation.pos.latitude), (dataPoint.pos.longitude - baseLocation.pos.longitude))+ (PI / 10);
			placeList.push_back(dataPoint);

		}
		dataFile.close();

	}
	else {
		cout << "File could not be opened or does not exist\n";
		exit(1);
	}

	
	double record = 10e100;

	for (int k = 0; k < 10; k++) {

		for (int i = 1; i < placeList.size(); i++) {
			placeList[i].angle += -PI/10;
			if (placeList[i].angle < 0) { placeList[i].angle += 2 * PI; }
			placeList[i].demand = placeList[i].relPopulation;
		}
		sort(placeList.begin(), placeList.end(), sortByAngle);
		truck truckList[32];
		for (int i = 0; i < 32; i++) {
			truckList[i].supply = 9;
		}

		int j = 1;
		for (int i = 0; i < 32; i++) {
			truckList[i].routeNames.push_back("Base");
			truckList[i].route.push_back(0);
			while (truckList[i].supply != 0) {

				if (placeList[j].demand > 0) {
					placeList[j].demand += -truckList[i].supply;
					truckList[i].supply = 0;
					truckList[i].routeNames.push_back(placeList[j].name);
					truckList[i].route.push_back(j);

				}
				if (placeList[j].demand < 0) {
					truckList[i].supply += -placeList[j].demand;
					placeList[j].demand = 0;
				}

				if (placeList[j].demand == 0) { j++; }
			}
		}


		for (int i = 0; i < 32; i++) {
			int m = truckList[i].routeNames.size();
			//cout << i << ": ";
			//for (int j = 0; j < m; j++) {
				//cout << truckList[i].routeNames[j] << ", ";
			//}
			//cout << "\n";
		}

		double totalDistance = 0;
		for (int i = 0; i < 32; i++) {
			totalDistance += minimumTravellingSalesman(truckList[i].route, placeList);
		}
		cout << totalDistance << "\n";

		if (totalDistance < record) { record = totalDistance; }
	}





	
	




	cout << "Base located at: (" << baseLocation.pos.latitude*radToDeg << ", " << baseLocation.pos.longitude*radToDeg << ")\n";
	cout << "Total distance as the crow flies: " << record << " km.\n";
	cout << "Total approximate road distance: " << record*1.6 << " km.\n";







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
double haversineDistance(point Loc1, point Loc2) {



	double dLat = Loc2.pos.latitude - Loc1.pos.latitude;
	double dLong = Loc2.pos.longitude - Loc1.pos.longitude;

	double a = pow(sin(0.5*dLat), 2) + cos(Loc1.pos.latitude)*cos(Loc2.pos.latitude)*pow(sin(0.5*dLong), 2);
	double c = 2 * asin(pow(a, 0.5));
	double dist = 6371 * c;

	return dist;


}
vector<double> relativeDistance(vector <point> List, int i) {

	vector <double> distanceVector;
	double distance;
	//  List.size() -1
	for (int j = 0; j < List.size(); j++) {
		distance = haversineDistance(List[i], List[j]);
		distanceVector.push_back(distance);

	}
	return distanceVector;
}
bool sortByAngle(point i, point j) { return (i.angle<j.angle); }
double minimumTravellingSalesman(vector <int> route, vector <point> placeList) {
	vector <point> reducedPlaceList;
	int reducedSize = route.size();

	for (int i = 0; i < reducedSize; i++) {
		reducedPlaceList.push_back(placeList[route[i]]);
	}
	vector <vector <double>> reducedDistanceMatrix;
	 
	for (int i = 0; i < reducedSize; i++) {
		vector <double> reducedDistance = relativeDistance(reducedPlaceList, i);
			reducedDistanceMatrix.push_back(reducedDistance);
	}

	int range = reducedDistanceMatrix.size();
	vector <int> myInts;
	for (int i = 0; i < range; i++) {
		myInts.push_back(i);
	}
	

	double minimumDistance = 10e100;
	do {
		double sumDistance = 0;
		for (int i = 0; i < range; i++) {
			if (i + 2 > range) {
				sumDistance += reducedDistanceMatrix[myInts[i]][myInts[0]];
			}
			else {
				sumDistance += reducedDistanceMatrix[myInts[i]][myInts[i + 1]];
			}
			
		}
		if (sumDistance < minimumDistance) { minimumDistance = sumDistance; }
	} while (next_permutation(myInts.begin(), myInts.end()));



	//cout << perms << " || " << minimumDistance << "\n";
	
	return minimumDistance;


}
