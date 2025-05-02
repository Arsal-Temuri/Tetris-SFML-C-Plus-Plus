
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Arsal Temuri
// 23i-0016
// SE-A
// PF Project 2024
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////

const char title[] = "PF-Project-Arsal-Temuri-2024";

const int M = 20;
const int N = 10;

int gameGrid[M][N] = { 0 };

int point_1[4][2], point_2[4][2];


bool anamoly() {
    for (int i = 0; i < 4; i++)
        if (point_1[i][0] < 0 || point_1[i][0] >= N || point_1[i][1] >= M)
            return 0;
        else if (gameGrid[point_1[i][1]][point_1[i][0]])
            return 0;

    return 1;
};

