#include "ChunkParser.h"

#include <iostream>
#include <cstdint>
#include <filesystem>

#include <real_core/GameTime.h>

ChunkParser::~ChunkParser()
{
    if (m_OpenFile.second.is_open())
        m_OpenFile.second.close();
}

void ChunkParser::Init(uint32_t seed)
{
	m_Seed = seed;

    m_Path = "resources/saves/" + std::to_string(m_Seed) + "/";
    const std::filesystem::path filepath = m_Path;
    // Create parent directories if they don't exist
    if (!std::filesystem::exists(filepath.parent_path()))
    {
        if (!std::filesystem::create_directories(filepath.parent_path()))
        {
            std::cerr << "Failed to create directories: " << filepath.parent_path() << std::endl;
            return;
        }
    }

    m_OpenFile.first = { -1,-1 };
}

void ChunkParser::LoadChunk(const glm::ivec2& chunkPos, block_array& blocks, int& highestY, int& lowestY) const
{
    const auto id = real::GameTime::GetInstance().StartTimer();

	const auto filePath = m_Path + CreateFileName(chunkPos);
    std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open file for reading: " << filePath << std::endl;
        return;
    }

    while (file.peek() != EOF)
    {
        uint16_t pos;
        uint8_t type;

        file.read(reinterpret_cast<char*>(&pos), sizeof(pos));
        file.read(reinterpret_cast<char*>(&type), sizeof(type));

        const auto change = BlockChange::Decode(pos, type);
        blocks[change.x][change.z][change.y] = change.type;

        if (change.type != EBlock::air && change.type != EBlock::water)
        {
            highestY = std::max(highestY, static_cast<const int&>(change.y));
            lowestY = std::min(lowestY, static_cast<const int&>(change.y));
        }
    }

    file.close();

    const auto time = real::GameTime::GetInstance().EndTimer(id);
    std::cout << "time to load chunk : " << std::to_string(time) << '\n';
}

void ChunkParser::SaveBlock(const glm::ivec2& chunkPos, const glm::ivec3& blockPos, EBlock block)
{
    const auto id = real::GameTime::GetInstance().StartTimer();

    if (m_OpenFile.first != chunkPos)
    {
        const auto filePath = m_Path + CreateFileName(chunkPos);

    	m_OpenFile.second.close();
    	m_OpenFile.second.open(filePath, std::ios_base::out | std::ios_base::binary | std::ios_base::app);

    	if (!m_OpenFile.second.is_open())
        {
            std::cerr << "Failed to open or create file for writing: " << filePath << std::endl;
            return;
        }

        m_OpenFile.first = chunkPos;
    }


    BlockChange change{ static_cast<uint8_t>(blockPos.y), static_cast<uint8_t>(blockPos.x), static_cast<uint8_t>(blockPos.z), block };
    const uint16_t pos = change.EncodePosition();
    const uint8_t type = static_cast<uint8_t>(change.type);

    m_OpenFile.second.write(reinterpret_cast<const char*>(&pos), sizeof(pos));
    m_OpenFile.second.write(reinterpret_cast<const char*>(&type), sizeof(type));

    const auto time = real::GameTime::GetInstance().EndTimer<std::chrono::microseconds>(id);
    std::cout << "time to save block to chunk file : " << std::to_string(time) << " microseconds\n";
}

bool ChunkParser::HasChunkData(const glm::ivec2& chunkPos) const
{
    const auto filePath = m_Path + CreateFileName(chunkPos);

    return std::filesystem::exists(filePath);
}

std::string ChunkParser::CreateFileName(const glm::ivec2& chunkPos) const
{
    std::string fileName = std::to_string(chunkPos.x);
    fileName.append("x");
    fileName.append(std::to_string(chunkPos.y));
    fileName.append(".chunk");

    return fileName;
}
