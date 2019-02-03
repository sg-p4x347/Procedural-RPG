#include "pch.h"
#include "BuildingGenerator.h"
#include "Line.h"
#include "JsonParser.h"
#include "Utility.h"
#include "Room.h"
#include "BuildingGenerator.h"
#include "Rectangle.h"
#include "ModelVoxel.h"
#include "AssetManager.h"
using namespace Architecture;
using RoomPtr = std::shared_ptr<Room>;
using RoomPtrs = vector<shared_ptr<Room>>;
namespace world {
	BuildingGenerator::BuildingGenerator()
	{
	}

	VoxelGridModel BuildingGenerator::Create(SimpleMath::Rectangle footprint, JsonParser & config)
	{
		m_config = config;
		// initialize configuration settings
		m_goldenRatio = config["goldenRatio"].To<double>();
		m_hallWidth = config["hallWidth"].To<int>();
		m_divisionDeviation = config["divisionDeviation"].To<double>();
		m_minFitness = config["minFitness"].To<double>();
		// initialize rects vector
		auto m_rects = vector<Architecture::Rectangle>{ Architecture::Rectangle(footprint) };
		auto rooms = RoomPtrs();

		int roomIndex = -1;
		// keep creating rooms as long as there are rectangles
		while (m_rects.size() > 0) {
			if (!HasRequiredRooms(rooms)) {

				JsonParser roomCfg;
				do {
					roomIndex++;
					if (roomIndex >= config["rooms"].Count()) roomIndex = 0;
					roomCfg = config["rooms"][roomIndex];
				} while (!roomCfg["required"].To<bool>() || RoomCount(roomCfg["type"].To<string>(), rooms) >= m_config["rooms"][roomIndex]["count"].To<int>());

				// search for the best rect out of all current rects
				double bestFitness = 0.0;
				int bestRect = 0;
				for (int i = 0; i < m_rects.size(); i++) {
					double fitness = RoomFitness(m_rects[i], roomCfg);
					if (i == 0 || fitness < bestFitness) {
						bestFitness = fitness;
						bestRect = i;
					}
				}
				// recursively divide this rect until a room is established
				CreateRoom(bestRect, roomCfg, rooms, m_rects);

			}
			else {
				// search for the best rectangle-room match
				pair<int, JsonParser> bestFit = BestFit(m_rects, rooms);
				if (std::get<0>(bestFit) >= 0) {
					CreateRoom(std::get<0>(bestFit), std::get<1>(bestFit), rooms, m_rects);
				}
				else {
					break;
				}
			}
		}
		// Network the rooms
		NetworkRooms(rooms);
		// Voxelize
		auto voxels = Voxelize(rooms, SimpleMath::Rectangle(0, 0, footprint.width, footprint.height));

		vector<Room> finalRooms;
		for (RoomPtr & room : rooms) {
			finalRooms.push_back(*room);
		}
		// Return the building
		return VoxelGridModel(voxels);
	}
	int BuildingGenerator::RoomCount(string type, RoomPtrs & rooms)
	{
		int counter = 0;
		for (RoomPtr & room : rooms) {
			if (type == room->type) counter++;
		}
		return counter;
	}

	//bool BuildingGenerator::MeetsConstraints(Rectangle & rect,JsonParser & room)
	//{
	//	return (rect.width * rect.height >= room["minArea")
	//		&& std::abs(double(std::max(rect.width, rect.height)) / double(std::min(rect.width, rect.height) - m_goldenRatio)) / m_goldenRatio <= m_deviation
	//		);
	//}

	bool BuildingGenerator::MeetsAreaConstraint(Architecture::Rectangle & rect, JsonParser & room)
	{
		return (rect.width * rect.height >= room["minArea"].To<int>());
	}

	// Higher the fitness, the better
	double BuildingGenerator::RoomFitness(Architecture::Rectangle & rect, JsonParser & room)
	{
		double optimalArea = room["area"].To<double>();

		double optimalWidth = sqrt(optimalArea / m_goldenRatio); // short side
		double optimalLength = m_goldenRatio * optimalWidth; // long side
		double error = pow((min(rect.width, rect.height) - optimalWidth) / optimalWidth, 2)
			+ pow((max(rect.width, rect.height) - optimalLength) / optimalLength, 2);
		if (error == 0) {
			return std::numeric_limits<double>::infinity();
		}
		else {
			return 1.0 / error;
		}
	}

	pair<int, JsonParser> BuildingGenerator::BestFit(vector<Architecture::Rectangle>& rects, RoomPtrs & rooms)
	{
		// search for the best rectangle--room match
		double bestFitness = 0.0;
		int bestRect = -1;
		JsonParser bestRoomCfg;
		for (JsonParser roomCfg : m_config["rooms"].GetElements()) {
			if (roomCfg["count"].To<int>() == -1 || RoomCount(roomCfg["type"].To<string>(), rooms) < roomCfg["count"].To<int>()) {
				for (int rectIndex = 0; rectIndex < rects.size(); rectIndex++) {
					double fitness = RoomFitness(rects[rectIndex], roomCfg);
					if (fitness > bestFitness) {
						bestFitness = fitness;
						bestRect = rectIndex;
						bestRoomCfg = roomCfg;
					}
				}
			}
		}

		return pair<int, JsonParser>(bestRect, bestRoomCfg);
	}

	//void BuildingGenerator::BestFit(int parentIndex, JsonParser & room)
	//{
	//	Rectangle & parentRect = m_rects[parentIndex];
	//	// test each width in the range to determine which one yields the best rectangle
	//	Rectangle subRectA(parentRect);
	//	// determine which side to adjust (the longer side)
	//	bool adjustWidth = max(parentRect.width, parentRect.height) == parentRect.width;
	//	// find the rect with the best fitness
	//	double bestFitness = 0.0;
	//	for (int length = 1; length <= max(parentRect.width, parentRect.height); length++) {
	//		Rectangle testRect(subRectA);
	//		if (adjustWidth) {
	//			testRect.width = length;
	//		} else {
	//			testRect.height = length;
	//		}
	//		double fitness = RoomFitness(testRect, room);
	//		if (length == 1 || fitness < bestFitness ) {
	//			bestFitness = fitness;
	//			subRectA = testRect;
	//		}
	//	}
	//	// if the best (sub) rect is better than the original parent rect
	//	if (bestFitness < RoomFitness(parentRect,room)) {
	//		// Add a rect for the left over area
	//		Rectangle subRectB(parentRect);
	//		if (adjustWidth) {
	//			subRectB.width -= subRectA.width;
	//			subRectB.x = subRectA.width;
	//		}
	//		else {
	//			subRectB.height -= subRectA.height;
	//			subRectB.y = subRectA.height;
	//		}
	//		// add subRectB (non-optimized)
	//		m_rects.push_back(subRectB);
	//	}
	//	// remove the parent rect
	//	m_rects.erase(m_rects.begin() + parentIndex);
	//	// turn the optimized rect into a room
	//	m_rooms.push_back(new Room(subRectA, room.Get<string>("name")));
	//}

	vector<Architecture::Rectangle> BuildingGenerator::DivideRect(Architecture::Rectangle & rect, double division, bool percent, bool horizontal, bool vertical)
	{
		int largeSide = horizontal ? rect.width : (vertical ? rect.height : max(rect.width, rect.height));
		int smallSide = vertical ? rect.width : (horizontal ? rect.height : min(rect.width, rect.height));
		bool xIsLarger = !vertical && (largeSide == rect.width || horizontal);

		int divisionSideA = (int)(percent ? round(largeSide * division) : division);
		int divisionSideB = largeSide - divisionSideA;

		Architecture::Rectangle subRectA(
			rect.x,
			rect.y,
			(xIsLarger ? divisionSideA : smallSide),
			(xIsLarger ? smallSide : divisionSideA)
		);
		Architecture::Rectangle subRectB(
			rect.x + (xIsLarger ? subRectA.width : 0),
			rect.y + (xIsLarger ? 0 : subRectA.height),
			(xIsLarger ? divisionSideB : smallSide),
			(xIsLarger ? smallSide : divisionSideB)
		);
		vector<Architecture::Rectangle> rects;
		rects.push_back(subRectA);
		rects.push_back(subRectB);
		return rects;
	}

	void BuildingGenerator::CreateRoom(int parentIndex, JsonParser & roomCfg, RoomPtrs & rooms, vector<Architecture::Rectangle> & rects, bool recursive)
	{
		Architecture::Rectangle parentRect = rects[parentIndex];
		// remove the original rect
		rects.erase(rects.begin() + parentIndex);

		vector<Architecture::Rectangle> divided = DivideRect(parentRect, 0.5, true);
		Architecture::Rectangle subRectA = divided[0];
		Architecture::Rectangle subRectB = divided[1];

		/* if the average fitness of the two subRects is better
		than the original rect*/
		double parentFitness = RoomFitness(parentRect, roomCfg);
		double fitnessA = RoomFitness(subRectA, roomCfg);
		double fitnessB = RoomFitness(subRectB, roomCfg);
		if (subRectA != parentRect && subRectB != parentRect && (fitnessA + fitnessB) / 2.0 > parentFitness) {


			// try to add some randomness to the mix
			// divides the rect on the long side, deviating by "divisionDeviation" away from half way (0.5)
			vector<Architecture::Rectangle> unequalRects = DivideRect(parentRect, Utility::Deviation(m_divisionDeviation, 0.5f), true);
			double fitnessUneqA = RoomFitness(unequalRects[0], roomCfg);
			double fitnessUneqB = RoomFitness(unequalRects[1], roomCfg);
			if ((fitnessUneqA + fitnessUneqB) / 2.0 > parentFitness) {
				subRectA = unequalRects[0];
				subRectB = unequalRects[1];
				fitnessA = fitnessUneqA;
				fitnessB = fitnessUneqB;
			}

			// add the two subRects
			int indexA = -1;
			int indexB = -1;
			if (subRectA.Area() > 0 && subRectB.Area() > 0) {
				rects.push_back(subRectA);
				indexA = (int)rects.size() - 1;
				rects.push_back(subRectB);
				indexB = (int)rects.size() - 1;

				// recursively break down the best rect until a room is created
				if (recursive) {
					if (fitnessA >= fitnessB) {
						CreateRoom(indexA, roomCfg, rooms, rects); // subRectA
					}
					else if (fitnessB > fitnessA) {
						CreateRoom(indexB, roomCfg, rooms, rects); // subRectB
					}
				}
			}
			else {
				int test = 0;
			}


		}
		else {
			// re-assign this room if it is just ludicrous 
			if (parentFitness <= m_minFitness) {
				//newRoom.Initialize(self.BestFit([parentRect]).config.name);
				CreateRoom(parentIndex, std::get<1>(BestFit(vector<Architecture::Rectangle>{parentRect}, rooms)), rooms, rects);

			}
			else {
				// turn the parent rect into a room
				RoomPtr newRoom = std::make_shared<Room>(parentRect, roomCfg);

				rooms.push_back(newRoom);
			}
		}
	}
	void BuildingGenerator::NetworkRooms(RoomPtrs & rooms)
	{
		vector<RoomPtrs> networks;
		int i = 0;
		do {
			// make a hallway to connect separate networks
			if (networks.size() > 0) LinkNetworks(rooms, networks);
			// link the rooms
			LinkRooms(rooms);
			// group connected rooms in to networks
			networks = FindNetworks(rooms);
			i++;
		} while (networks.size() > 1 && i < 50); // i < [int] ensures no ifinity loop
	}
	void BuildingGenerator::LinkRooms(RoomPtrs & rooms)
	{
		// clear out any pre-existing links
		for (RoomPtr & room : rooms) {
			room->links.clear();
		}
		// do the linking
		bool continueNetworking = true;
		while (continueNetworking) {
			bool linkCreated = false;
			// iterate over all rooms
			for (RoomPtr & roomA : rooms) {
				if (roomA->links.size() < roomA->config["maxLinks"].To<int>()) {
					// keep track of the best link
					RoomPtr bestRoom = nullptr;
					int bestLinkRank = -1;
					// iterate over all other rooms
					for (RoomPtr & roomB : rooms) {
						if (&roomA != &roomB && !roomA->IsLinkedWith(roomB.get()) && roomA->rect.IsTouching(roomB->rect)) {
							// determine if either room can link with the other
							int linkRank = roomA->config["linksTo"].IndexOf(roomB->type);
							if (linkRank != -1 && (bestLinkRank == -1 || linkRank < bestLinkRank)) {
								bestLinkRank = linkRank;
								bestRoom = roomB;
							}
							if (linkRank == 0) break;
						}
					}

					if (bestRoom != nullptr) {
						// make the link
						roomA->links.push_back(bestRoom);
						linkCreated = true;
					}
				}
				if (linkCreated) break;
			}
			if (!linkCreated) continueNetworking = false;
		}
	}
	vector<RoomPtrs> BuildingGenerator::FindNetworks(RoomPtrs & rooms)
	{
		// initialize the array of room networks
		vector<RoomPtrs> networks;
		for (RoomPtr & room : rooms) {
			networks.push_back(RoomPtrs{ room });
		}
		bool linkFound = true;
		while (linkFound) {
			linkFound = false;
			for (int indexA = 0; indexA < networks.size(); indexA++) {
				for (int indexB = 0; indexB < networks.size(); indexB++) {
					if (indexA != indexB) {
						RoomPtrs & networkA = networks[indexA];
						RoomPtrs & networkB = networks[indexB];
						for (int rIndexA = 0; rIndexA < networkA.size(); rIndexA++) {
							for (int rIndexB = 0; rIndexB < networkB.size(); rIndexB++) {
								RoomPtr roomA = networkA[rIndexA];
								RoomPtr roomB = networkB[rIndexB];
								if (roomA->IsLinkedWith(roomB.get())) {
									linkFound = true;
									// add networkB to networkA
									networks[indexA].insert(networkA.end(), networkB.begin(), networkB.end());
									// remove network B
									networks.erase(networks.begin() + indexB);
								}
								if (linkFound) break;
							}
							if (linkFound) break;
						}
					}
					if (linkFound) break;
				}
				if (linkFound) break;
			}
		}
		return networks;
	}
	void BuildingGenerator::LinkNetworks(RoomPtrs & rooms, vector<RoomPtrs> networks)
	{
		// find the closest two disconnected networks
		tuple<shared_ptr<Room>, shared_ptr<Room>, double> bestPair(
			nullptr,
			nullptr,
			std::numeric_limits<double>::infinity() // shortest distance between the two
		);
		for (int i = 0; i < networks.size(); i++) {
			for (int j = 0; j < networks.size(); j++) {
				if (i != j) {
					tuple<shared_ptr<Room>, shared_ptr<Room>, double> pair = FindClosest(networks[i], networks[j], true);
					// if closer than the previous closest
					if (std::get<2>(pair) < std::get<2>(bestPair)) {
						bestPair = pair;
					}
				}
			}
		}
		// travel from A to B
		CreateHallway(rooms, nullptr, shared_ptr<Room>(std::get<0>(bestPair)), shared_ptr<Room>(std::get<1>(bestPair)));
	}
	tuple<shared_ptr<Room>, shared_ptr<Room>, double> BuildingGenerator::FindClosest(RoomPtrs A, RoomPtrs B, bool linkable, bool touching)
	{
		tuple<shared_ptr<Room>, shared_ptr<Room>, double> closestPair(
			nullptr,
			nullptr,
			std::numeric_limits<double>::infinity() // shortest distance between the two
		);
		for (RoomPtr roomA : A) {
			for (RoomPtr roomB : B) {
				if (roomA != roomB && (!linkable || roomA->CanLinkWith(roomB.get())) && (!touching || roomA->rect.IsTouching(roomB->rect))) {
					double distance = (roomA->rect.Center() - roomB->rect.Center()).Length();
					if (distance < get<2>(closestPair)) {
						get<0>(closestPair) = roomA;
						get<1>(closestPair) = roomB;
						get<2>(closestPair) = distance;

					}
				}
			}
		}
		return closestPair;
	}
	RoomPtrs BuildingGenerator::RoomsTouching(RoomPtrs roomsIn, RoomPtr roomA)
	{
		RoomPtrs rooms;
		for (RoomPtr & roomB : roomsIn) {
			if (roomA->rect.IsTouching(roomB->rect)) rooms.push_back(roomB);
		}
		return rooms;
	}
	void BuildingGenerator::CreateHallway(RoomPtrs rooms, RoomPtr previous, RoomPtr current, RoomPtr target)
	{
		// find the next room in the chain
		RoomPtr nextRoom = nullptr;
		RoomPtrs roomsTouching = RoomsTouching(rooms, current);
		for (RoomPtr room : roomsTouching) {
			if (room == target) nextRoom = target;
		}
		if (nextRoom == nullptr) nextRoom = get<0>(FindClosest(roomsTouching, RoomPtrs{ target }));
		// find which sides to make the hallway on
		if (previous) {
			// connection between current and previous room// edges of the current room that would be viable for a hallway
			vector<Edge> edgesPrev;
			Line line = current->rect.Touching(previous->rect);

			if (line.SharedCoord(current->rect.x)) edgesPrev.push_back(Left);
			if (line.SharedCoord(current->rect.x + current->rect.width)) edgesPrev.push_back(Right);
			if (line.SharedCoord(current->rect.y)) edgesPrev.push_back(Bottom);
			if (line.SharedCoord(current->rect.y + current->rect.height)) edgesPrev.push_back(Top);
			// connection between current and next room
			vector<Edge> edgesNext;
			line = current->rect.Touching(previous->rect);

			if (line.SharedCoord(current->rect.x)) edgesNext.push_back(Left);
			if (line.SharedCoord(current->rect.x + current->rect.width)) edgesNext.push_back(Right);
			if (line.SharedCoord(current->rect.y)) edgesNext.push_back(Bottom);
			if (line.SharedCoord(current->rect.y + current->rect.height)) edgesNext.push_back(Top);

			// if there is a common edge, that be the hallway
			bool common = false;
			for (Edge edgePrev : edgesPrev) {
				for (Edge edgeNext : edgesNext) {
					if (edgePrev == edgeNext) {
						CreateHallwayFromEdge(rooms, current, edgePrev);
						common = true;
						break;
					}
				}
				if (common) break;
			}
			// if no common edge
			if (!common) {
				// if the edges are adjacent
				bool adjacent = false;
				for (Edge edgePrev : edgesPrev) {
					for (Edge edgeNext : edgesNext) {
						if (EdgesAdjacent(edgePrev, edgeNext)) {
							CreateHallwayFromEdge(rooms, current, edgePrev);
							CreateHallwayFromEdge(rooms, current, edgeNext);
							adjacent = true;
							break;
						}
					}
					if (adjacent) break;
				}
				// if edges are opposite
				if (!adjacent) {
					CreateHallwayFromEdge(rooms, current, edgesPrev[0]);
					CreateHallwayFromEdge(rooms, current, edgesNext[0]);
					Edge other;
					if (edgesPrev[0] == Left || edgesPrev[0] == Right) other = Top;
					if (edgesPrev[0] == Bottom || edgesPrev[0] == Top) other = Left;
					CreateHallwayFromEdge(rooms, current, other);
				}
			}
		}
		if (nextRoom == target) {
			return;
		}
		else {
			// recursively create hallways until the target is reached
			CreateHallway(rooms, current, nextRoom, target);
		}
	}
	bool BuildingGenerator::EdgesAdjacent(Edge & A, Edge & B)
	{
		return ((A == Left || A == Right) && (B == Top || B == Bottom) || (A == Top || A == Bottom) && (B == Left || B == Right));
	}
	void BuildingGenerator::CreateHallwayFromEdge(RoomPtrs rooms, RoomPtr room, Edge edge)
	{
		RoomPtr hallway;
		JsonParser hallConfig;
		for (JsonParser & roomCfg : m_config["rooms"].GetElements()) {
			if (roomCfg["type"].To<string>() == "hallway") {
				hallConfig = roomCfg;
				break;
			}
		}
		switch (edge) {
		case Left:
			if (m_hallWidth < room->rect.width) {
				hallway = shared_ptr<Room>(new Room(Architecture::Rectangle(room->rect.x, room->rect.y, m_hallWidth, room->rect.height), hallConfig));
				room->rect.width -= m_hallWidth;
				room->rect.x += m_hallWidth;
			}
			else {
				room->type = "hallway";
				room->config = hallConfig;
			}
			break;
		case Right:
			if (m_hallWidth < room->rect.width) {
				hallway = shared_ptr<Room>(new Room(Architecture::Rectangle(room->rect.x + room->rect.Right() - m_hallWidth, room->rect.y, m_hallWidth, room->rect.height), hallConfig));
				room->rect.width -= m_hallWidth;
			}
			else {
				room->type = "hallway";
				room->config = hallConfig;
			}
			break;
		case Bottom:
			if (m_hallWidth < room->rect.height) {
				hallway = shared_ptr<Room>(new Room(Architecture::Rectangle(room->rect.x, room->rect.y, room->rect.width, m_hallWidth), hallConfig));
				room->rect.height -= m_hallWidth;
				room->rect.y += m_hallWidth;
			}
			else {
				room->type = "hallway";
				room->config = hallConfig;
			}
			break;
		case Top:
			if (m_hallWidth < room->rect.height) {
				hallway = shared_ptr<Room>(new Room(Architecture::Rectangle(room->rect.x, room->rect.Top() - m_hallWidth, room->rect.width, m_hallWidth), hallConfig));
				room->rect.height -= m_hallWidth;
			}
			else {
				room->type = "hallway";
				room->config = hallConfig;
			}
			break;
		}
		// remove the room if it no longer has an area
		/*if (room->rect.Area() <= 0) {
		for (int i = 0; i < m_rooms.size(); i++) {
		if (&m_rooms[i] == room) {
		m_rooms.erase(m_rooms.begin() + i);
		}
		}
		}*/
		// if this room now looks pretty shitty, rename it
		if (!room->config["required"].To<bool>() || RoomFitness(room->rect, room->config) <= m_minFitness) {
			//room->
		}
		// add the hallway room
		if (hallway && hallway->type == "hallway") {
			rooms.push_back(hallway);
		}
	}
	VoxelGrid<ModelVoxel> BuildingGenerator::Voxelize(RoomPtrs & rooms, SimpleMath::Rectangle footprint)
	{
		// 3D voxel array
		VoxelGrid<ModelVoxel> voxels(footprint.width + 2, 2, footprint.height + 2);
		// building level assets
		//EntityPtr exteriorWallBottomAsset;
		//EntityPtr exteriorWallMiddleAsset;
		//EntityPtr exteriorCornerAsset;
		//AssetManager::Get()->Find(AssetManager::Get()->GetStaticEM(), (string)m_config["exteriorWallType"] + "_bottom", exteriorWallBottomAsset);
		//AssetManager::Get()->Find(AssetManager::Get()->GetStaticEM(), (string)m_config["exteriorWallType"] + "_middle", exteriorWallMiddleAsset);
		//AssetManager::Get()->Find(AssetManager::Get()->GetStaticEM(), (string)m_config["exteriorCornerType"], exteriorCornerAsset);
		// iterate & voxelize each room
		for (RoomPtr & room : rooms) {
			EntityPtr floorAsset;
			//EntityPtr wallBottomAsset;
			//EntityPtr wallMiddleAsset;
			//EntityPtr cornerAsset;
			AssetManager::Get()->Find(AssetManager::Get()->GetStaticEM(), (string)room->config["floorType"], floorAsset);
			//AssetManager::Get()->Find(AssetManager::Get()->GetStaticEM(), (string)room->config["wallType"] + "_bottom", wallBottomAsset);
			//AssetManager::Get()->Find(AssetManager::Get()->GetStaticEM(), (string)room->config["wallType"] + "_middle", wallMiddleAsset);
			//AssetManager::Get()->Find(AssetManager::Get()->GetStaticEM(), (string)room->config["cornerType"], cornerAsset);
			//AssetManager::Get()->Find((string)room->config["cornerType"], cornerAsset);

			for (int x = room->rect.x; x <= room->rect.x + room->rect.width; x++) {
				for (int z = room->rect.y; z <= room->rect.y + room->rect.height; z++) {
					if (x >= room->rect.x && x <= room->rect.x + room->rect.width && z >= room->rect.y && z <= room->rect.y + room->rect.height) {
						//----------------------------------------------------------------
						// Interiror

						// Unit square used for determining where interior walls go
						/*
						(-1,1)-------(1,1)
						|				|
						|     .			|
						|				|
						(-1,-1)------(1,-1)
						*/
						int unitX = (x == room->rect.x ? -1 : (x == (room->rect.x + room->rect.width) ? 1 : 0));

						int unitZ = (z == room->rect.y ? -1 : (z == (room->rect.y + room->rect.height) ? 1 : 0));

						int voxelX = std::min(room->rect.x + room->rect.width, x + 1);
						int voxelZ = std::min(room->rect.y + room->rect.height, z + 1);
						ModelVoxel bottom;
						AddFloor(bottom,floorAsset->ID());
						//AddWall(bottom,wallBottomAsset->ID(),unitX,unitZ);
						voxels.Set(bottom, voxelX, 0, voxelZ);

						ModelVoxel middle;
						//AddWall(middle, wallMiddleAsset->ID(), unitX, unitZ);
						//voxels.Set(middle, voxelX, 1, voxelZ);
					}
				}
			}
		}
		//----------------------------------------------------------------
		// Exterior
		//for (int x = 0; x <= footprint.width + 1; x++) {
		//	for (int z = 0; z <= footprint.height + 1; z++) {
		//		if (x == 0 || x == footprint.width + 1 || z == 0 || z == footprint.height + 1) {
		//			//----------------------------------------------------------------
		//			// Exterior
		//			int unitX = (x == 0 ? 1 : (x == footprint.width + 1 ? -1 : 0));
		//			int unitZ = (z == 0 ? 1 : (z == footprint.height + 1 ? -1 : 0));

		//			/*int voxelX = std::min(room->rect.x + room->rect.width + 1, x);
		//			int voxelZ = std::min(room->rect.y + room->rect.height + 1, z);*/
		//			ModelVoxel bottomVoxel;
		//			if (std::abs(unitX) == 1 && std::abs(unitZ) == 1) {
		//				AddCorner(bottomVoxel, exteriorCornerAsset->ID(), unitX, unitZ);
		//			}
		//			else {
		//				AddWall(bottomVoxel, exteriorWallBottomAsset->ID(), unitX, unitZ);
		//			}
		//			voxels.Set(bottomVoxel, x, 0, z);

		//			ModelVoxel middleVoxel;
		//			if (std::abs(unitX) == 1 && std::abs(unitZ) == 1) {
		//			}
		//			else {
		//				AddWall(middleVoxel, exteriorWallMiddleAsset->ID(), unitX, unitZ);
		//			}
		//			voxels.Set(middleVoxel, x, 1, z);
		//		}
		//	}
		//}
		return voxels;
	}

	void BuildingGenerator::AddWall(ModelVoxel & voxel, AssetID wallAsset, int unitX, int unitZ)
	{
		/*
		^
		|
		+X
				  1
		 +--------+--------+
		 |		  |		   |
		 |		  |		   |
		 |		  |		   |
		2+--------*--------+0
		 |		  |		   |
		 |		  |		   |
		 |		  |		   |
		 +--------+--------+
				  3
		+Z --->

		The '*' in the above diagram represents the local
		origin of this voxel

		Each '+' in the above diagram represents a valid
		Unit square position; the two interior walls that
		intersect this point are selected
		*/
		if (abs(unitZ) == 1)
			voxel.AddComponent(wallAsset,2 - (unitZ + 1));

		if (abs(unitX) == 1) {
			voxel.AddComponent(wallAsset,3 - (unitX + 1));
		}
	}

	void BuildingGenerator::AddCorner(ModelVoxel & voxel, AssetID cornerAsset, int unitX, int unitZ)
	{
		/*
		^
		|
		+X
		1				  0
		+-----------------+
		|		 |		  |
		|		 |		  |
		|		 |		  |
		|--------*--------|
		|		 |		  |
		|		 |		  |
		|		 |		  |
		+-----------------+
		2				  3
		+Z --->
		*/
		if (unitZ == 1) {
			if (unitX == 1) {
				voxel.AddComponent(cornerAsset, Transforms::None);
			}
			else {
				voxel.AddComponent(cornerAsset, Transforms::Rotate270);
			}
		}
		else {
			if (unitX == 1) {
				voxel.AddComponent(cornerAsset, Transforms::Rotate90);
			}
			else {
				voxel.AddComponent(cornerAsset, Transforms::Rotate180);
			}
		}
	}

	void BuildingGenerator::AddFloor(ModelVoxel & voxel, AssetID floorAsset)
	{
		voxel.AddComponent(floorAsset, Transforms::None);
	}

	bool BuildingGenerator::HasRequiredRooms(RoomPtrs & rooms)
	{
		int totalRequired = 0;
		int totalCreated = 0;
		for (int i = 0; i < m_config["rooms"].Count(); i++) {
			JsonParser roomConfig = m_config["rooms"][i];
			if (roomConfig["required"].To<bool>()) {
				totalRequired++;
				for (RoomPtr & room : rooms) {
					if (room->type == roomConfig["type"].To<string>()) {
						totalCreated++;
						break;
					}
				}
			}
		}
		return totalCreated >= totalRequired;
	}

}