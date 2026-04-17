#ifndef CHUNK_H
#define CHUNK_H
#include <vector>
#define CS 16
#define CH 32
enum Block { AIR, WATER, SAND, DIRT, GRASS, STONE, BEDROCK, WOOD, LEAVES };
struct Chunk {
    int cx, cz;
    Block data[CS][CH][CS];
    std::vector<float> verts;
    void generate();
    void buildMesh();
    bool isSolid(int x,int y,int z);
    Block getBlock(int x,int y,int z);
};
#endif
