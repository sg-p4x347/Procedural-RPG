#pragma once
#include "Building.h"
#include "BuildingVoxel.h"
#include "Room.h"
using namespace Components;
class BuildingGenerator
{
public:
	BuildingGenerator();

public:
	Building Create(Architecture::Rectangle footprint, JsonParser & config, string type);
	DirectX::SimpleMath::Rectangle GetFootprint();
private:

	//bool MeetsConstraints(Rectangle & rect, JsonParser & room);
	bool MeetsAreaConstraint(Architecture::Rectangle & rect, JsonParser & room);
	// returns the relative error of the rectangle as it fits the room constraints
	// 0 is most fit, positive infinity is least fit
	double RoomFitness(Architecture::Rectangle & rect, JsonParser & room);
	pair<int, JsonParser> BestFit(vector<Architecture::Rectangle> & rects, vector<Room> & rooms);
	//void BestFit(int parentIndex, JsonParser & room);
	bool HasRequiredRooms();
	// returns the number of occurences of the specified room type
	int RoomCount(string type, vector<Room> & rooms);
	// Room Generation ================================
	vector<Architecture::Rectangle> DivideRect(Architecture::Rectangle & rect, double division, bool percent = true, bool horizontal = false, bool vertical = false);
	void CreateRoom(int parentIndex, JsonParser & roomConfig, vector<Room> & rooms,vector<Architecture::Rectangle> & rects, bool recursive = true);
	// Room Networking ================================
	void NetworkRooms();
	void LinkRooms();
	// determines what rooms are networked and returns an array of room arrays
	vector< vector<Room*> > FindNetworks();
	// pathfinds a hallway between two disconnected room networks
	void LinkNetworks(vector< vector<Room*> > networks);
	// takes two vectors of rooms, returns the closest pair of rooms, and the distance between them
	tuple<Room*, Room*, double> FindClosest(vector<Room*> A, vector<Room*> B, bool linkable = false, bool touching = false);

	vector<Room*> RoomsTouching(Room* room);
	void CreateHallway(Room* previous, Room* current, Room* target);
	bool EdgesAdjacent(Edge & A, Edge & B);
	void CreateHallwayFromEdge(Room* room, Edge edge);

	// Voxelization ================================
	void Voxelize();

	//=================================================
	// Properties
	//=================================================

	string m_type;
	JsonParser m_config;
	// generation constants
	double m_goldenRatio;
	int m_hallWidth;
	double m_divisionDeviation;
	double m_minFitness;
};

