#pragma once
#include "pch.h"

template<typename KeyType>
class MappedFile
{
	struct Block {
		Block() : position(0), size(0), buffer(0) {

		}
		uint32_t position;
		uint32_t size;
		uint32_t buffer;
		size_t GetAllocationSize() {
			return size + buffer;
		}
	};
public:
	MappedFile() {

	}
	MappedFile(Filesystem::path indexFile, Filesystem::path dataFile) : m_indexFile(indexFile), m_dataFile(dataFile) {
		std::ifstream ifs(m_indexFile, std::ios::binary | std::ios::ate);
		size_t fileSize = ifs.tellg();
		
		
		if (ifs.is_open()) {
			// Initialize from index file
			size_t offset = 0;
			ifs.seekg(offset);
			ifs.read((char *)&m_size, sizeof(size_t));
			offset += sizeof(size_t);
			
			// Begin reading each key value pair of the map
			while (offset < fileSize) {
				// read the key
				KeyType key = KeyType();
				ifs.seekg(offset);
				ifs.read((char *)&key, sizeof(KeyType));
				offset += sizeof(KeyType);
				

				// read the block count
				size_t blockCount = 0;
				ifs.seekg(offset);
				ifs.read((char *)&blockCount, sizeof(size_t));
				offset += sizeof(size_t);
				if (blockCount > 0) {
					// read all blocks
					std::vector<Block> blocks(blockCount);
					ifs.seekg(offset);
					ifs.read((char *)&(blocks[0]), sizeof(Block) * blockCount);
					offset += sizeof(Block) * blockCount;
					// map
					m_index.insert(std::make_pair(key, blocks));
				}
			}
		}
		else {
			// Initialize default values
			m_size = 0;
		}
	}
	~MappedFile() {
		Save();
	}
	inline void Insert(KeyType key, std::vector<uint8_t> & data,float bufferSpace = 1.f) {
		const char * ptr = data.size() > 0 ? &data[0] : nullptr;
		Insert(key, ptr, data.size(),bufferSpace);
	}
	inline void Insert(KeyType key, std::string data, float bufferSpace = 1.f) {
		Insert(key, data.c_str(), data.length(), bufferSpace);
	}
	// Overwrites all data associated with key
	inline void Insert(KeyType key, const char * data, size_t size, float bufferSpace = 1.f) {
		
		std::vector<Block> & blocks = FindBlocks(key);
		size_t offset = 0;
		if (blocks.size()) {
			std::fstream ofs(m_dataFile, std::ios::binary | std::ios::out | std::ios::in);

			for (Block & block : blocks) {
				size_t blockAllocation = block.GetAllocationSize();
				size_t transferSize = std::min(blockAllocation, size - offset);
				if (transferSize > 0) {
					ofs.seekp(block.position);
					ofs.write((const char *)(data + offset), transferSize);
				}
				block.size = transferSize;
				block.buffer = blockAllocation - transferSize;
				offset += transferSize;
			}
			ofs.close();
		}
		// create new block for extra
		if (offset < size) {
			blocks.push_back(Allocate(data + offset, size - offset, bufferSpace));
		}
	}
	
	// Returns all data associated with key
	inline void Search(KeyType key,char *& data, size_t & size) {
		// get total size needed to contain all data
		size = GetSize(key);
		if (size > 500000000)
			assert("dataSize is in excess of 500 MB");
		data = new char[size];
		
		if (size > 0) {
			// read all data into the vector
			std::ifstream ifs(m_dataFile, std::ios::binary);
			size_t offset = 0;
			for (Block & block : FindBlocks(key)) {
				if (block.size == 0) break;
				ifs.seekg(block.position);
				ifs.read((char *)(data + offset), block.size);
				offset += block.size;
			}
		}
	}
	// De-stripes all entries and cleans up empty or orphaned blocks
	inline void Defragment(float bufferSpace = 0.f) {
		std::map<KeyType, std::string> data;
		// load all data into the map
		for (auto & index : m_index) {
			if (index.second.size() > 0)
				data.insert(std::make_pair(index.first, Search(index.first)));
		}
		// clear the index
		m_index.clear();
		m_size = 0;
		// clear the data file
		std::ofstream ofs(m_dataFile, std::ios::binary);
		ofs.write(nullptr, 0);
		ofs.close();
		// re-insert all data
		for (auto & index : data) {
			Insert(index.first, index.second,bufferSpace);
		}
	}
	// Returns the size of data associated with the given key
	inline size_t GetSize(KeyType key) {
		size_t dataSize = 0;
		if (m_index.find(key) != m_index.end()) {
			for (Block & block : m_index[key]) {
				dataSize += block.size;
			}
		}
		return dataSize;
	}
	// Returns keys that have data
	inline std::vector<KeyType> IndexedKeys() {
		std::vector<KeyType> keys;
		for (auto & index : m_index) {
			if (GetSize(index.first) > 0) keys.push_back(index.first);
		}
		return keys;
	}
private:
	Filesystem::path m_indexFile;
	Filesystem::path m_dataFile;
	std::map<KeyType, std::vector<Block>> m_index;
	size_t m_size;

private:
	// Returns all blocks associated with key
	inline std::vector<Block> & FindBlocks(KeyType key) {
		auto pos = m_index.find(key);
		if (pos != m_index.end()) {
			return pos->second;
		}
		else {
			m_index.insert(std::make_pair(key, std::vector<Block>()));
			return FindBlocks(key);
		}
	}
	// Allocates and writes new block space for the given data
	inline Block Allocate(const char * data, size_t size,float bufferSpace) {
		std::ofstream ofs(m_dataFile, std::ofstream::binary | std::ofstream::app);
		Block block;
		block.position = m_size;
		block.size = size;
		block.buffer = size * bufferSpace;
		m_size += block.GetAllocationSize();
		ofs.write((const char *)data, block.GetAllocationSize());
		return block;
	}
	// Serialize the index to file
	inline void Save() {
		std::ofstream ofs(m_indexFile, std::ios::binary);
		
		size_t offset = 0;

		ofs.write((const char*)&m_size, sizeof(size_t));
		offset += sizeof(size_t);
		for (auto & index : m_index) {
			// must have at least one block to write an index
			if (index.second.size() > 0) {
				// write the key
				ofs.seekp(offset);
				ofs.write((const char *)&index.first, sizeof(KeyType));
				offset += sizeof(KeyType);

				// write the block count
				size_t count = index.second.size();
				ofs.seekp(offset);
				ofs.write((const char *)&count, sizeof(size_t));
				offset += sizeof(size_t);

				// write all blocks
				ofs.seekp(offset);
				ofs.write((const char *)&(index.second[0]), sizeof(Block) * count);
				offset += sizeof(Block) * count;
			}
		}
	}
};

