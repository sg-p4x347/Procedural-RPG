#pragma once
#include "JsonParser.h"
#include "Room.h"
#include "Voxel.h"
#include "JSON.h"

namespace Architecture {
	enum Edge {
		Left,
		Right,
		Top,
		Bottom
	};
class Building : public JSON
{
public:
	Building(Rectangle footprint, JsonParser & config,string type);
	~Building();
	// JSON implimentaion
	virtual JsonParser Export();
	virtual void Import(JsonParser & building);
private:
	
	//bool MeetsConstraints(Rectangle & rect, JsonParser & room);
	bool MeetsAreaConstraint(Rectangle & rect, JsonParser & room);
	// returns the relative error of the rectangle as it fits the room constraints
	// 0 is most fit, positive infinity is least fit
	double RoomFitness(Rectangle & rect, JsonParser & room);
	pair<int, JsonParser> BestFit(vector<Rectangle> & rects);
	//void BestFit(int parentIndex, JsonParser & room);
	bool HasRequiredRooms();
	// returns the number of occurences of the specified room type
	int RoomCount(string type);
	// Room Generation ================================
	vector<Rectangle> DivideRect(Rectangle & rect, double division, bool percent = true, bool horizontal = false, bool vertical = false);
	void CreateRoom(int parentIndex, JsonParser & roomConfig, bool recursive = true);
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
	Architecture::Rectangle m_footprint;
	vector<Architecture::Rectangle> m_rects;
	vector<Room> m_rooms;
	vector<vector<Voxel>> m_voxels;
	// generation constants
	double m_goldenRatio;
	int m_hallWidth;
	double m_divisionDeviation;
	double m_minFitness;
};
}