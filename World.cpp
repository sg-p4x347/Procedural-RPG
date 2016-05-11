#include "World.h"

World::World() {

}
void World::init(unsigned int worldWidthIn, unsigned int regionWidthIn, unsigned int loadWidthIn, string workingPathIn) {
	// regions are regionSize bytes long
	worldWidth = worldWidthIn;
	regionWidth = regionWidthIn;
	workingPath = workingPathIn;
	loadWidth = loadWidthIn;
	regionSize = (regionWidth + 1)*(regionWidth + 1) * 2;
	regions = new CircularArray(loadWidth, loadWidth);
}
void World::loadRegions(ID3D11Device* device) {
	// calculate displacement
	int regionX = floor(player.getPosition().x / float(regionWidth));
	int regionZ = floor(player.getPosition().z / float(regionWidth));
	// get the sector displacement
	short displacementX = regionX - lastX;
	short displacementZ = regionZ - lastZ;
	// update the position of last sector
	lastX = regionX;
	lastZ = regionZ;
	if (displacementX == 0 && displacementZ == 0) {
		// don't load anything
		return;
	} else if (abs(displacementX) > 0 && abs(displacementZ) > 0) {
		// roll the new regions into the old one's places
		regions->offsetX(displacementX);
		if (displacementX == -1 || displacementX == 1) {
			// the left or right most collumn is replaced with the new regions
			int x;
			if (displacementX == -1) {
				x = 0;
			} else if (displacementX == 1) {
				x = loadWidth - 1;
			}
			for (int z = 0; z < loadWidth; z++) {
				// place the new region into memory
				regions->set(x, z, device, regionX + displacementX*(loadWidth / 2), regionZ - loadWidth / 2 + z, worldWidth, regionWidth, workingPath);
			}
		}
		regions->offsetY(displacementZ);
		if (displacementZ == -1 || displacementZ == 1) {
			// the left or right most collumn is replaced with the new regions
			int z;
			if (displacementZ == -1) {
				z = 0;
			} else if (displacementZ == 1) {
				z = loadWidth - 1;
			}
			for (int x = 0; x < loadWidth; x++) {
				// place the new region into memory
				regions->set(x, z, device, regionX - loadWidth / 2 + x, regionZ + displacementZ*(loadWidth / 2), worldWidth, regionWidth, workingPath);
			}
		}
	} else {
		// load a whole new set of regions and reset the circular array
		fillRegions(device);
	}
}
void World::fillRegions(ID3D11Device* device) {
	regions->zeroOffsets();
	int index = 0;
	// find the region coordinate that the player is located in
	int regionZ = floor(player.getPosition().z / regionWidth);
	int regionX = floor(player.getPosition().x / regionWidth);
	// load a grid of regions around the player; loadWidth is the width and height in regions
	for (int z = regionZ - round(float(loadWidth) / 2); z < regionZ + round(float(loadWidth) / 2); z++) {
		for (int x = regionX - round(float(loadWidth) / 2); x < regionX + round(float(loadWidth) / 2); x++) {
			// load a new region for updating
			regions->set(x, z, device,x,z,worldWidth,regionWidth,workingPath);
			index++;
		}
	}
}
void World::update(ID3D11Device* device, InputClass * input, double elapsed) {
	// update the player's physics
	//player.setAcceleration(XMFLOAT3(0, 0, 0));
	player.update(input);
	player.updatePhysics(elapsed);
	// load the regions around the player's position
	loadRegions(device);
	// player collision
	// *note* only on one triangle orientation
	// find the region coordinate that the player is located in
	int regionZ = floor(player.getPosition().z / regionWidth);
	int regionX = floor(player.getPosition().x / regionWidth);
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
	return XMFLOAT3(int(floor(position.x)) % (regionWidth+1), position.y, int(floor(position.x)) % (regionWidth+1));
}
float World::yOnABC(float x, float z, XMFLOAT3 A, XMFLOAT3 B, XMFLOAT3 C) {
	XMFLOAT3 u = XMFLOAT3(B.x - A.x, B.y - A.y, B.z - A.z);
	XMFLOAT3 v = XMFLOAT3(C.x = A.x, C.y - A.y, C.z - A.z);
	XMFLOAT3 N = XMFLOAT3(-u.y*v.z + u.z*v.y, -u.z*v.x + u.x*v.z, -u.x*v.y + u.y*v.x);
	XMFLOAT3 V = XMFLOAT3(A.x - x, 0, A.z - z);
	return A.y + ((N.z * V.z) + (N.x * V.x)) / N.y;

}
void World::render(D3DClass* Direct3D, ShaderManager* shaderManager) {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;

	// Clear the buffers to begin the scene.
	Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	// Generate the view matrix based on the camera's position.
	player.render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	Direct3D->GetWorldMatrix(worldMatrix);
	player.getViewMatrix(viewMatrix);
	Direct3D->GetProjectionMatrix(projectionMatrix);
	player.getBaseViewMatrix(baseViewMatrix);
	Direct3D->GetOrthoMatrix(orthoMatrix);

	// Render regions
	for (int i = regions->size()-1; i >= 0; i--) {
		if (!regions->data[i].isNull()) {
			regions->data[i].render(Direct3D->GetDeviceContext());
			shaderManager->RenderColorShader(Direct3D->GetDeviceContext(), regions->data[i].getIndexCount(), &worldMatrix, &viewMatrix, &projectionMatrix);
		}
	}
	
	// Present the rendered scene to the screen.
	Direct3D->EndScene();

}
void World::loadPlayer() {
	ifstream playerFile(workingPath + "player.bin", ios::binary);
	if (playerFile.is_open()) {
		// read the file into the player class
		player = Player(XMFLOAT3(128,64,128), 100.0f);
	}
}
World::~World() {
	
}