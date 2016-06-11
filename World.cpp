#include "pch.h"
#include "World.h"

World::World() {
}
void World::init(shared_ptr<ID3D11Device> device,unsigned int worldWidthIn, unsigned int regionWidthIn, unsigned int loadWidthIn, string workingPathIn) {
	// DirectX
	m_device = device;
	// regions are regionSize bytes long
	m_worldWidth = worldWidthIn;
	m_regionWidth = regionWidthIn;
	m_workingPath = workingPathIn;
	m_loadWidth = loadWidthIn;
	m_regionSize = (m_regionWidth + 1)*(m_regionWidth + 1) * 2;
	m_regions->init(m_loadWidth, m_loadWidth);
	loadPlayer();
	fillRegions();
	
}
void World::loadRegions() {
	// calculate displacement
	int regionX = floor(m_player->getPosition().x / float(m_regionWidth));
	int regionZ = floor(m_player->getPosition().z / float(m_regionWidth));
	// get the sector displacement
	short displacementX = regionX - m_lastX;
	short displacementZ = regionZ - m_lastZ;
	// update the position of last sector
	m_lastX = regionX;
	m_lastZ = regionZ;
	if (displacementX == 0 && displacementZ == 0) {
		// don't load anything
		return;
	} else if (abs(displacementX) > 0 && abs(displacementZ) > 0) {
		// roll the new regions into the old one's places
		m_regions->offsetX(displacementX);
		if (displacementX == -1 || displacementX == 1) {
			// the left or right most collumn is replaced with the new regions
			int x;
			if (displacementX == -1) {
				x = 0;
			} else if (displacementX == 1) {
				x = m_loadWidth - 1;
			}
			for (int z = 0; z < m_loadWidth; z++) {
				// place the new region into memory
				m_regions->set(x, z, m_device, regionX + displacementX*(m_loadWidth / 2), regionZ - m_loadWidth / 2 + z, m_worldWidth, m_regionWidth, m_workingPath);
			}
		}
		m_regions->offsetY(displacementZ);
		if (displacementZ == -1 || displacementZ == 1) {
			// the left or right most row is replaced with the new regions
			int z;
			if (displacementZ == -1) {
				z = 0;
			} else if (displacementZ == 1) {
				z = m_loadWidth - 1;
			}
			for (int x = 0; x < m_loadWidth; x++) {
				// place the new region into memory
				m_regions->set(x, z, m_device, regionX - m_loadWidth / 2 + x, regionZ + displacementZ*(m_loadWidth / 2), m_worldWidth, m_regionWidth, m_workingPath);
			}
		}
	} else {
		// load a whole new set of regions and reset the circular array
		fillRegions();
	}
}
void World::fillRegions() {
	m_regions->zeroOffsets();
	int index = 0;
	// find the region coordinate that the player is located in
	int regionZ = floor(m_player->getPosition().z / m_regionWidth);
	int regionX = floor(m_player->getPosition().x / m_regionWidth);
	// load a grid of regions around the player; loadWidth is the width and height in regions
	for (int z = regionZ - round(float(m_loadWidth) / 2); z < regionZ + round(float(m_loadWidth) / 2); z++) {
		for (int x = regionX - round(float(m_loadWidth) / 2); x < regionX + round(float(m_loadWidth) / 2); x++) {
			// load a new region for updating
			m_regions->set(x, z, m_device,x,z, m_worldWidth, m_regionWidth, m_workingPath);
			index++;
		}
	}
}
void World::update(float elapsed) {
	// update the player's physics
	//player.update(input);
	m_player->updatePhysics(elapsed);
	// load the regions around the player's position
	loadRegions();
	// player collision
	// *note* only on one triangle orientation
	// find the region coordinate that the player is located in
	int regionZ = floor(m_player->getPosition().z / m_regionWidth);
	int regionX = floor(m_player->getPosition().x / m_regionWidth);
	// current region
	/*Region * region = regions->get(regionX, regionZ);
	XMFLOAT3 position = player.getPosition();
	XMFLOAT3 triangle[3];
	XMFLOAT3 regionalCoord = globalToRegionCoord(position);
	triangle[0] = region->vertices[posToIndex(regionalCoord.x, regionalCoord.z, regionWidth + 1)].Pos;
	triangle[1] = region->vertices[posToIndex(regionalCoord.x + 1, regionalCoord.z, regionWidth + 1)].Pos;
	triangle[2] = region->vertices[posToIndex(regionalCoord.x, regionalCoord.z + 1, regionWidth + 1)].Pos;
	float yPos = yOnABC(position.x, position.z, triangle[0], triangle[1], triangle[2]) + 10;
	player.setPosition(XMFLOAT3(position.x, yPos, position.z));*/
}
XMFLOAT3 World::globalToRegionCoord(XMFLOAT3 position) {
	return XMFLOAT3(int(floor(position.x)) % (m_regionWidth+1), position.y, int(floor(position.x)) % (m_regionWidth+1));
}
float World::yOnABC(float x, float z, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C) {
	XMFLOAT3 u = XMFLOAT3(B.x - A.x, B.y - A.y, B.z - A.z);
	XMFLOAT3 v = XMFLOAT3(C.x = A.x, C.y - A.y, C.z - A.z);
	XMFLOAT3 N = XMFLOAT3(-u.y*v.z + u.z*v.y, -u.z*v.x + u.x*v.z, -u.x*v.y + u.y*v.x);
	XMFLOAT3 V = XMFLOAT3(A.x - x, 0, A.z - z);
	return A.y + ((N.z * V.z) + (N.x * V.x)) / N.y;

}
void World::render() {
	//XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;

	//// Clear the buffers to begin the scene.
	//Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	//// Generate the view matrix based on the camera's position.
	//m_player->render();

	//// Get the world, view, and projection matrices from the camera and d3d objects.
	//Direct3D->GetWorldMatrix(worldMatrix);
	//m_player->getViewMatrix(viewMatrix);
	//Direct3D->GetProjectionMatrix(projectionMatrix);
	//m_player->getBaseViewMatrix(baseViewMatrix);
	//Direct3D->GetOrthoMatrix(orthoMatrix);

	//// Render regions
	//for (int i = m_regions->size()-1; i >= 0; i--) {
	//	if (!m_regions->data[i].isNull()) {
	//		m_regions->data[i].render(Direct3D->GetDeviceContext());
	//	}
	//}
	//
	//// Present the rendered scene to the screen.
	//Direct3D->EndScene();

}
shared_ptr<CircularArray> World::getRegions()
{
	return m_regions;
}
void World::loadPlayer() {
	ifstream playerFile(m_workingPath + "player.bin", ios::binary);
	if (playerFile.is_open()) {
		// read the file into the player class
		m_player = make_unique<Player>(XMFLOAT3(128,64,128), 100.0f);
	}
}
World::~World() {
	
}