#include "pch.h"

#include "Line.h"
#include "JsonParser.h"
#include "Utility.h"
#include "Room.h"
#include "Building.h"
#include "Rectangle.h"
#include "Voxel.h"

using namespace std;
using namespace Architecture;
using namespace Utility;

namespace Architecture {
Building::Building(Architecture::Rectangle footprint, JsonParser & config, string type)
{
	// initialize configuration settings
	m_config = config[type];
	m_goldenRatio = config["goldenRatio"].To<double>();
	m_hallWidth = m_config["hallWidth"].To<int>();
	m_divisionDeviation = m_config["divisionDeviation"].To<double>();
	m_minFitness = m_config["minFitness"].To<double>();
	// initialize rects vector
	m_rects = vector<Architecture::Rectangle>{ footprint };
	m_footprint = footprint;

	
	int roomIndex = -1;
	// keep creating rooms as long as there are rectangles
	while (m_rects.size() > 0) {
		if (!HasRequiredRooms()) {
			
			JsonParser roomCfg;
			do {
				roomIndex++;
				if (roomIndex >= m_config["rooms"].Count()) roomIndex = 0;
				roomCfg = m_config["rooms"][roomIndex];
			} while (!roomCfg["required"].To<bool>() || RoomCount(roomCfg["type"].To<string>()) >= m_config["rooms"][roomIndex]["count"].To<int>());

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
			CreateRoom(bestRect, roomCfg);

		} else {
			// search for the best rectangle-room match
			pair<int, JsonParser> bestFit = BestFit(m_rects);
			if (std::get<0>(bestFit) >= 0) {
				CreateRoom(std::get<0>(bestFit), std::get<1>(bestFit));
			} else {
				break;
			}
		}
	}
	// Network the rooms
	NetworkRooms();
	// Voxelize and save the building
	Voxelize();
}

Building::~Building()
{
}
JsonParser Building::Export()
{
	JsonParser building;
	building.Set("type", m_type);
	building.Set("footprint", m_footprint.Export());
	JsonParser voxels;
	for (vector<Voxel> col : m_voxels) {
		JsonParser column;
		for (Voxel voxel : col) {
			column.Add(voxel.Export());
		}
		voxels.Add(column);
	}
	building.Set("voxels", voxels);
	return building;
}
void Building::Import(JsonParser & building)
{
	m_type = building["type"].To<string>();
	m_footprint = Rectangle(building["footprint"]);
	JsonParser voxels;
	for (int x = 0; x < building["voxels"].Count(); x++) {
		m_voxels.push_back(building["voxels"][x].ToVector<Voxel>());
	}
}
int Building::RoomCount(string type)
{
	int counter = 0;
	for (Room & room : m_rooms) {
		if (type == room.type) counter++;
	}
	return counter;
}

//bool Building::MeetsConstraints(Rectangle & rect,JsonParser & room)
//{
//	return (rect.width * rect.height >= room["minArea")
//		&& std::abs(double(std::max(rect.width, rect.height)) / double(std::min(rect.width, rect.height) - m_goldenRatio)) / m_goldenRatio <= m_deviation
//		);
//}

bool Building::MeetsAreaConstraint(Rectangle & rect, JsonParser & room)
{
	return (rect.width * rect.height >= room["minArea"].To<int>());
}

// Higher the fitness, the better
double Building::RoomFitness(Rectangle & rect, JsonParser & room)
{
	double optimalArea = room["area"].To<double>();

	double optimalWidth = sqrt(optimalArea / m_goldenRatio); // short side
	double optimalLength = m_goldenRatio * optimalWidth; // long side
	double error = pow((min(rect.width, rect.height) - optimalWidth) / optimalWidth, 2)
		+ pow((max(rect.width, rect.height) - optimalLength) / optimalLength, 2);
	if (error == 0) {
		return std::numeric_limits<double>::infinity();
	} else {
		return 1.0 / error;
	}
}

pair<int, JsonParser> Building::BestFit(vector<Rectangle>& rects)
{
	// search for the best rectangle--room match
	double bestFitness = 0.0;
	int bestRect = -1;
	JsonParser bestRoomCfg;
	for (JsonParser roomCfg : m_config["rooms"].GetElements()) {
		if (roomCfg["count"].To<int>() == -1 || RoomCount(roomCfg["type"].To<string>()) < roomCfg["count"].To<int>()) {
			for (int rectIndex = 0; rectIndex < m_rects.size(); rectIndex++) {
				double fitness = RoomFitness(m_rects[rectIndex],roomCfg);
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

//void Building::BestFit(int parentIndex, JsonParser & room)
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

vector<Rectangle> Building::DivideRect(Rectangle & rect, double division, bool percent, bool horizontal, bool vertical)
{
	int largeSide = horizontal ? rect.width : (vertical ? rect.height : max(rect.width, rect.height));
	int smallSide = vertical ? rect.width : (horizontal ? rect.height : min(rect.width, rect.height));
	bool xIsLarger = !vertical && (largeSide == rect.width || horizontal);

	int divisionSideA = (int)(percent ? round(largeSide * division) : division);
	int divisionSideB = largeSide - divisionSideA;

	Rectangle subRectA(
		rect.x,
		rect.y,
		(xIsLarger ? divisionSideA : smallSide),
		(xIsLarger ? smallSide : divisionSideA)
	);
	Rectangle subRectB(
		rect.x + (xIsLarger ? subRectA.width : 0),
		rect.y + (xIsLarger ? 0 : subRectA.height),
		(xIsLarger ? divisionSideB : smallSide),
		(xIsLarger ? smallSide : divisionSideB)
	);
	vector<Rectangle> rects;
	rects.push_back(subRectA);
	rects.push_back(subRectB);
	return rects;
}

void Building::CreateRoom(int parentIndex, JsonParser & roomCfg, bool recursive)
{
	Rectangle parentRect = m_rects[parentIndex];
	// remove the original rect
	m_rects.erase(m_rects.begin() + parentIndex);

	vector<Rectangle> rects = DivideRect(parentRect, 0.5, true);
	Rectangle subRectA = rects[0];
	Rectangle subRectB = rects[1];
	//// pick a spot to divide the rectangle
	//long largeSide = max(parentRect.width, parentRect.height);
	//long smallSize = min(parentRect.width, parentRect.height);

	//long divisionSideA = (long)Utility::randWithin((float(largeSide) / 2.f) - (float(largeSide)*m_divisionDeviation), float(largeSide) / 2.f);
	//long divisionSideB = largeSide - divisionSideA;

	//Rectangle subRectA = Rectangle(
	//	parentRect.x,
	//	parentRect.y,
	//	divisionSideA,
	//	smallSize
	//);
	//Rectangle subRectB = Rectangle(
	//	parentRect.x + (largeSide == parentRect.width ? divisionSideA : 0),
	//	parentRect.y + (largeSide == parentRect.height ? divisionSideA : 0),
	//	divisionSideB,
	//	smallSize
	//);

	/* if the average fitness of the two subRects is better
	than the original rect*/
	double parentFitness = RoomFitness(parentRect, roomCfg);
	double fitnessA = RoomFitness(subRectA, roomCfg);
	double fitnessB = RoomFitness(subRectB, roomCfg);
	if (subRectA != parentRect && subRectB != parentRect && (fitnessA + fitnessB) / 2.0 > parentFitness) {
		

		// try to add some randomness to the mix
		// divides the rect on the long side, deviating by "divisionDeviation" away from half way (0.5)
		vector<Rectangle> unequalRects = DivideRect(parentRect, Deviation(m_divisionDeviation,0.5),true);
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
		if (subRectA.Area() > 0 && subRectB.Area() > 0)  {
			m_rects.push_back(subRectA);
			indexA = m_rects.size() - 1;
			m_rects.push_back(subRectB);
			indexB = m_rects.size() - 1;

			// recursively break down the best rect until a room is created
			if (recursive) {
				if (fitnessA >= fitnessB) {
					CreateRoom(indexA, roomCfg); // subRectA
				}
				else if (fitnessB > fitnessA) {
					CreateRoom(indexB, roomCfg); // subRectB
				}
			}
		} else {
			int test = 0;
		}

		
	}
	else {
		// re-assign this room if it is just ludicrous 
		if (parentFitness <= m_minFitness) {
			//newRoom.Initialize(self.BestFit([parentRect]).config.name);
			CreateRoom(parentIndex, std::get<1>(BestFit(vector<Rectangle>{parentRect})));

		}
		else {
			// turn the parent rect into a room
			Room newRoom = Room(parentRect, roomCfg);

			m_rooms.push_back(newRoom);
		}
	}
}
void Building::NetworkRooms()
{
	vector< vector<Room*> > networks;
	int i = 0;
	do {
		// make a hallway to connect separate networks
		if (networks.size() > 0) LinkNetworks(networks);
		// link the rooms
		LinkRooms();
		// group connected rooms in to networks
		networks = FindNetworks();
		i++;
	} while (networks.size() > 1 && i < 50); // i < [int] ensures no ifinity loop
}
void Building::LinkRooms()
{
	// clear out any pre-existing links
	for (Room & room : m_rooms) {
		room.links.clear();
	}
	// do the linking
	bool continueNetworking = true;
	while (continueNetworking) {
		bool linkCreated = false;
		// iterate over all rooms
		for ( Room & roomA : m_rooms) {
			if (roomA.links.size() < roomA.config["maxLinks"].To<int>()) {
				// keep track of the best link
				Room * bestRoom = nullptr;
				int bestLinkRank = -1;
				// iterate over all other rooms
				for (Room & roomB : m_rooms) {
					if (&roomA != &roomB && !roomA.IsLinkedWith(&roomB) && roomA.rect.IsTouching(roomB.rect)) {
						// determine if either room can link with the other
						int linkRank = roomA.config["linksTo"].IndexOf(roomB.type);
						if (linkRank != -1 && ( bestLinkRank == -1 || linkRank < bestLinkRank)) {
							bestLinkRank = linkRank;
							bestRoom = &roomB;
						}
						if (linkRank == 0) break;
					}
				}

				if (bestRoom != nullptr) {
					// make the link
					roomA.links.push_back(bestRoom);
					linkCreated = true;
				}
			}
			if (linkCreated) break;
		}
		if (!linkCreated) continueNetworking = false;
	}
}
vector<vector<Room*>> Building::FindNetworks()
{
	// initialize the array of room networks
	vector<vector<Room*>> networks;
	for (Room & room : m_rooms) {
		networks.push_back(vector<Room*>{&room});
	}
	bool linkFound = true;
	while (linkFound) {
		linkFound = false;
		for (int indexA = 0; indexA < networks.size(); indexA++) {
			for (int indexB = 0; indexB < networks.size(); indexB++) {
				if (indexA != indexB) {
					vector<Room*> & networkA = networks[indexA];
					vector<Room*> & networkB = networks[indexB];
					for (int rIndexA = 0; rIndexA < networkA.size(); rIndexA++) {
						for (int rIndexB = 0; rIndexB < networkB.size(); rIndexB++) {
							Room* roomA = networkA[rIndexA];
							Room* roomB = networkB[rIndexB];
							if (roomA->IsLinkedWith(roomB)) {
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
void Building::LinkNetworks(vector<vector<Room*>> networks)
{
	// find the closest two disconnected networks
	tuple<Room*, Room*, double> bestPair(
		nullptr,
		nullptr,
		std::numeric_limits<double>::infinity() // shortest distance between the two
		);
	for (int i = 0; i < networks.size(); i++) {
		for (int j = 0; j < networks.size(); j++) {
			if (i != j) {
				tuple<Room*, Room*, double> pair = FindClosest(networks[i], networks[j], true);
				// if closer than the previous closest
				if (std::get<2>(pair) < std::get<2>(bestPair)) {
					bestPair = pair;
				}
			}
		}
	}
	// travel from A to B
	CreateHallway(nullptr, std::get<0>(bestPair), std::get<1>(bestPair));
}
tuple<Room*, Room*, double> Building::FindClosest(vector<Room*> A, vector<Room*> B, bool linkable, bool touching)
{
	tuple<Room*, Room*, double> closestPair;
	get<2>(closestPair) = std::numeric_limits<double>::infinity();
	for (Room* roomA : A) {
		for (Room* roomB : B ) {
			if (roomA != roomB && (!linkable || roomA->CanLinkWith(roomB)) && (!touching || roomA->rect.IsTouching(roomB->rect))) {
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
vector<Room*> Building::RoomsTouching(Room * roomA)
{
	vector<Room*> rooms;
	for (Room & roomB : m_rooms) {
		if (roomA->rect.IsTouching(roomB.rect)) rooms.push_back(&roomB);
	}
	return rooms;
}
void Building::CreateHallway(Room * previous, Room * current, Room * target)
{
	// find the next room in the chain
	Room * nextRoom = nullptr;
	vector<Room*> roomsTouching = RoomsTouching(current);
	for (Room * room : roomsTouching) {
		if (room == target) nextRoom = target;
	}
	if (nextRoom == nullptr) nextRoom = get<0>(FindClosest(roomsTouching, vector<Room*>{target}));
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
					CreateHallwayFromEdge(current, edgePrev);
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
						CreateHallwayFromEdge(current, edgePrev);
						CreateHallwayFromEdge(current, edgeNext);
						adjacent = true;
						break;
					}
				}
				if (adjacent) break;
			}
			// if edges are opposite
			if (!adjacent) {
				CreateHallwayFromEdge(current, edgesPrev[0]);
				CreateHallwayFromEdge(current, edgesNext[0]);
				Edge other;
				if (edgesPrev[0] == Left || edgesPrev[0] == Right) other = Top;
				if (edgesPrev[0] == Bottom || edgesPrev[0] == Top) other = Left;
				CreateHallwayFromEdge(current, other);
			}
		}
	}
	if (nextRoom == target) {
		return;
	}
	else {
		// recursively create hallways until the target is reached
		CreateHallway(current, nextRoom, target);
	}
}
bool Building::EdgesAdjacent(Edge & A, Edge & B)
{
	return ((A == Left || A == Right) && (B == Top || B == Bottom) || (A == Top || A == Bottom) && (B == Left || B == Right));
}
void Building::CreateHallwayFromEdge(Room * room, Edge edge)
{
	Room hallway;
	JsonParser hallConfig;
	for (JsonParser roomCfg : m_config["rooms"].GetElements()) {
		if (roomCfg["type"].To<string>() == "hallway") {
			hallConfig = roomCfg;
			break;
		}
	}
	switch (edge) {
	case Left: 
		if (m_hallWidth <  room->rect.width) {
			hallway = Room(Rectangle(room->rect.x, room->rect.y, m_hallWidth, room->rect.height), hallConfig);
			room->rect.width -= m_hallWidth;
			room->rect.x += m_hallWidth;
		} else {
			room->type = "hallway";
			room->config = hallConfig;
		}
		break;
	case Right: 
		if (m_hallWidth < room->rect.width) {
			hallway = Room(Rectangle(room->rect.x + room->rect.Right() - m_hallWidth, room->rect.y, m_hallWidth, room->rect.height), hallConfig);
			room->rect.width -= m_hallWidth;
		}
		else {
			room->type = "hallway";
			room->config = hallConfig;
		}
		break;
	case Bottom: 
		if (m_hallWidth < room->rect.height) {
			hallway = Room(Rectangle(room->rect.x, room->rect.y, room->rect.width,m_hallWidth), hallConfig);
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
			hallway = Room(Rectangle(room->rect.x, room->rect.Top() - m_hallWidth, room->rect.width,  m_hallWidth), hallConfig);
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
	if (hallway.type == "hallway") {
		m_rooms.push_back(hallway);
	}
}
void Building::Voxelize()
{
	// 2d voxel array
	m_voxels = vector < vector<Voxel> >(m_footprint.width, vector<Voxel>(m_footprint.height));
	// iterate & voxelize each room
	for (Room room : m_rooms) {
		for (int x = room.rect.x; x < room.rect.x + room.rect.width; x ++) {
			for (int y = room.rect.y; y < room.rect.y + room.rect.height; y++) {
				// Unit square used for determining where walls go
				/*
				(-1,1)-------(1,1)
				   |           |
				   |     .     |
				   |           |
				(-1,-1)------(1,-1)
				*/
				short unitX = (short)floor(((float)x - room.rect.Center().x) / ((float)room.rect.width / 2.f));
				short unitY = (short)floor(((float)y - room.rect.Center().y) / ((float)room.rect.height / 2.f));
				m_voxels[x][y].Floor(
					unitX,
					unitY,
					room.config["floorType"].To<short>()
				);
				m_voxels[x][y].Wall(
					unitX,
					unitY,
					room.config["wallType"].To<short>()
				);
			}
		}
	}
}

bool Building::HasRequiredRooms()
{
	int totalRequired = 0;
	int totalCreated = 0;
	for (int i = 0; i < m_config["rooms"].Count(); i++) {
		JsonParser roomConfig = m_config["rooms"][i];
		if (roomConfig["required"].To<bool>()) {
			totalRequired++;
			for (Room & room : m_rooms) {
				if (room.type == roomConfig["type"].To<string>()) {
					totalCreated++;
					break;
				}
			}
		}
	}
	return totalCreated >= totalRequired;
}
}
