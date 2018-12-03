#pragma once
#include "VoxelGridModel.h"
#include "VoxelGrid.h"
#include "ModelVoxel.h"
#include "Room.h"
using RoomPtr = std::shared_ptr<Architecture::Room>;
using RoomPtrs = vector<shared_ptr<Architecture::Room>>;
namespace world {
	class BuildingGenerator
	{
	public:
		BuildingGenerator();

	public:
		VoxelGridModel Create(SimpleMath::Rectangle footprint, JsonParser & config);
	private:

		//bool MeetsConstraints(Rectangle & rect, JsonParser & room);
		bool MeetsAreaConstraint(Architecture::Rectangle & rect, JsonParser & room);
		// returns the relative error of the rectangle as it fits the room constraints
		// 0 is most fit, positive infinity is least fit
		double RoomFitness(Architecture::Rectangle & rect, JsonParser & room);
		pair<int, JsonParser> BestFit(vector<Architecture::Rectangle> & rects, RoomPtrs & rooms);
		//void BestFit(int parentIndex, JsonParser & room);
		bool HasRequiredRooms(RoomPtrs & rooms);
		// returns the number of occurences of the specified room type
		int RoomCount(string type, RoomPtrs & rooms);
		// Room Generation ================================
		vector<Architecture::Rectangle> DivideRect(Architecture::Rectangle & rect, double division, bool percent = true, bool horizontal = false, bool vertical = false);
		void CreateRoom(int parentIndex, JsonParser & roomConfig, RoomPtrs & rooms, vector<Architecture::Rectangle> & rects, bool recursive = true);
		// Room Networking ================================
		void NetworkRooms(RoomPtrs & rooms);
		void LinkRooms(RoomPtrs & rooms);
		// determines what rooms are networked and returns an array of room arrays
		vector<RoomPtrs> FindNetworks(RoomPtrs & rooms);
		// pathfinds a hallway between two disconnected room networks
		void LinkNetworks(RoomPtrs & rooms, vector<RoomPtrs > networks);
		// takes two vectors of rooms, returns the closest pair of rooms, and the distance between them
		tuple<shared_ptr<Architecture::Room>, shared_ptr<Architecture::Room>, double> FindClosest(RoomPtrs A, RoomPtrs B, bool linkable = false, bool touching = false);

		RoomPtrs RoomsTouching(RoomPtrs rooms, RoomPtr room);
		void CreateHallway(RoomPtrs rooms, RoomPtr previous, RoomPtr current, RoomPtr target);
		bool EdgesAdjacent(Architecture::Edge & A, Architecture::Edge & B);
		void CreateHallwayFromEdge(RoomPtrs rooms, RoomPtr room, Architecture::Edge edge);

		// Voxelization ================================
		VoxelGrid<ModelVoxel> Voxelize(RoomPtrs & rooms, SimpleMath::Rectangle footprint);
		void AddWall(ModelVoxel & voxel, AssetID wallAsset, int unitX, int unitZ);
		void AddCorner(ModelVoxel & voxel, AssetID cornerAsset, int unitX, int unitZ);
		void AddFloor(ModelVoxel & voxel, AssetID floorAsset);
		
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

}