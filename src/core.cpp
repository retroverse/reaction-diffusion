#include <iostream>
#include "core.h"

GridData::GridData()
{
    // Initialise grids
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            GridCell a {0.0f, 0.0f};
            GridCell b {0.0f, 0.0f};
            data[i][j] = a;
            swap[i][j] = b;
        }
    }
}

void GridData::swapGrids()
{
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            GridCell t = data[i][j];
            data[i][j] = swap[i][j];
            swap[i][j] = t;
        }
    }
}

long double GridData::getCellValue(CellType cellType, int i, int j)
{
    if (cellType == CellType::A) {
        return data[i][j].a;
    } else {
        return data[i][j].b;
    }
}

long double GridData::laplace(RDConfig* config, CellType cellType, int i, int j)
{
    long double s = 0;
    for (int di = 0; di < 3; di++)
    {
        for (int dj = 0; dj < 3; dj++)
        {
            int x = i + di - 1;
            int y = j + dj - 1;
            long double val = getCellValue(cellType, x, y);
            s += val * config->laplaceWeights[di][dj];
        }
    }

    return s;
}

void GridData::update(RDConfig* config)
{
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            // Calculate deltas
            long double a = data[i][j].a;
            long double b = data[i][j].b;
            long double deltaA =
                  (config->diffA * laplace(config, CellType::A, i, j))
                - (a * b * b)
                + (config->feed * (1 - a));
            long double deltaB =
                  (config->diffB * laplace(config, CellType::B, i, j))
                + (a * b * b)
                - (config->kill + config->feed) * b;

            // Update Step
            swap[i][j].a = a + deltaA * config->dt;
            swap[i][j].b = b + deltaB * config->dt;
        }
    }

    // Swap grids
    this->swapGrids();
}

void GridData::print()
{
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            std::cout << "[" << data[i][j].a << "|" << data[i][j].b << "]";
        }
        std::cout << std::endl;
    }
}
