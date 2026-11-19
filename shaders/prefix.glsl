#version 430 core

layout(std430, binding = 2) readonly buffer TileCount {
    int tileCounts[];
};

layout(std430, binding = 3) buffer TileOffsets {
    int tileOffsets[];
};

uniform int tileCount;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
    int sum = 0;
    int i = 0;

    for (; i + 4 <= tileCount; i += 4) {
        int c0 = tileCounts[i];
        int c1 = tileCounts[i + 1];
        int c2 = tileCounts[i + 2];
        int c3 = tileCounts[i + 3];

        tileOffsets[i] = sum;
        tileOffsets[i + 1] = sum + c0;
        tileOffsets[i + 2] = sum + c0 + c1;
        tileOffsets[i + 3] = sum + c0 + c1 + c2;

        sum += c0 + c1 + c2 + c3;
    }

    for (; i < tileCount; i++) {
        tileOffsets[i] = sum;
        sum += tileCounts[i];
    }

    tileOffsets[tileCount] = sum;
}
