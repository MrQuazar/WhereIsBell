#include "MapGenerator.h"
#include "Engine/World.h"

AMapGenerator::AMapGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMapGenerator::BeginPlay()
{
    Super::BeginPlay();

    float Gap = 2 * CellSpacing;

    // Base centered at origin
    SpawnBaseFloor(FVector(0, 0, 0));

    // Hill position (left side)
    FVector HillOffset(-(GridSize * CellSpacing * 0.5f + Gap), 0, 0);

    // Maze position (right side)
    FVector MazeOffset((GridSize * CellSpacing * 0.5f + Gap), 0, 0);

    // Spawn Hill
    GenerateHillMatrix();
    SpawnMap(HillOffset);

    // Spawn Maze
    GenerateMazeMatrix();
    SpawnMap(MazeOffset);
}



void AMapGenerator::GenerateHillMatrix()
{
    Matrix.SetNum(GridSize);
    for (int32 i = 0; i < GridSize; i++)
    {
        Matrix[i].Init(1, GridSize); // Default = 1
    }

    //Max Value
    TArray<FIntPoint> PreviousLevelCells;
        int32 X, Y;
        if (GetRandomEmptyCell(X, Y))
        {
            Matrix[X][Y] = GridSize;
            PreviousLevelCells.Add(FIntPoint(X, Y));
        }

    //Grow downwards
    int32 CurrentValue = GridSize-1;
    int CellsToPlace =  2;

    while (CellsToPlace > 0 && CurrentValue > 1 && PreviousLevelCells.Num() > 0)
    {
        TArray<FIntPoint> NewLevelCells;

        for (FIntPoint Cell : PreviousLevelCells)
        {
            TArray<FIntPoint> Adjacents = GetAdjacents(Cell.X, Cell.Y);

            for (FIntPoint Adjacent : Adjacents)
            {
                if (Matrix[Adjacent.X][Adjacent.Y] == 1 && CellsToPlace > 0)
                {
                    Matrix[Adjacent.X][Adjacent.Y] = CurrentValue;
                    NewLevelCells.Add(Adjacent);
                    CellsToPlace--;
                }
            }
        }

        PreviousLevelCells = NewLevelCells;
        CurrentValue--;
        CellsToPlace = GridSize + 1 - CurrentValue;
    }
}

void AMapGenerator::GenerateMazeMatrix()
{
    Matrix.SetNum(GridSize);

    float SeedOffsetX = FMath::FRandRange(-10000.f, 10000.f);
    float SeedOffsetY = FMath::FRandRange(-10000.f, 10000.f);

    float LowestNoise = 9999.f;
    FIntPoint LowestPoint(0, 0);

    for (int32 x = 0; x < GridSize; x++)
    {
        Matrix[x].SetNum(GridSize);

        for (int32 y = 0; y < GridSize; y++)
        {
            float NoiseValue = FMath::PerlinNoise2D(
                FVector2D(
                    x * NoiseScale + SeedOffsetX,
                    y * NoiseScale + SeedOffsetY
                )
            );

            if (NoiseValue < LowestNoise)
            {
                LowestNoise = NoiseValue;
                LowestPoint = FIntPoint(x, y);
            }

            Matrix[x][y] = (NoiseValue > WallThreshold) ? 10 : 1;
        }
    }

    // ---- Guarantee Entrance ----

    // Pick random edge
    int32 EdgeSide = FMath::RandRange(0, 3);
    FIntPoint Entrance;

    switch (EdgeSide)
    {
    case 0: Entrance = FIntPoint(0, FMath::RandRange(0, GridSize - 1)); break;
    case 1: Entrance = FIntPoint(GridSize - 1, FMath::RandRange(0, GridSize - 1)); break;
    case 2: Entrance = FIntPoint(FMath::RandRange(0, GridSize - 1), 0); break;
    default: Entrance = FIntPoint(FMath::RandRange(0, GridSize - 1), GridSize - 1); break;
    }

    // Carve direct tunnel to treasure
    FIntPoint Current = Entrance;

    while (Current != LowestPoint)
    {
        Matrix[Current.X][Current.Y] = 1;

        if (Current.X < LowestPoint.X) Current.X++;
        else if (Current.X > LowestPoint.X) Current.X--;

        if (Current.Y < LowestPoint.Y) Current.Y++;
        else if (Current.Y > LowestPoint.Y) Current.Y--;
    }

    // Place treasure
    Matrix[LowestPoint.X][LowestPoint.Y] = 0;
}


bool AMapGenerator::GetRandomEmptyCell(int32& OutX, int32& OutY)
{
    TArray<FIntPoint> ECells = GetEmptyCells();

    if (ECells.Num() == 0)
        return false;

    int32 Index = FMath::RandRange(0, ECells.Num() - 1);
    OutX = ECells[Index].X;
    OutY = ECells[Index].Y;

    return true;
}


TArray<FIntPoint> AMapGenerator::GetEmptyCells() {
    TArray<FIntPoint> EmptyCells;

    for (int32 x = 0; x < GridSize; x++)
    {
        for (int32 y = 0; y < GridSize; y++)
        {
            if (Matrix[x][y] == 1)
            {
                EmptyCells.Add(FIntPoint(x, y));
            }
        }
    }
    return EmptyCells;
}

TArray<FIntPoint> AMapGenerator::GetAdjacents(int32 X, int32 Y)
{
    TArray<FIntPoint> Result;

    TArray<FIntPoint> Directions = {
        FIntPoint(1,0),
        FIntPoint(-1,0),
        FIntPoint(0,1),
        FIntPoint(0,-1),
        FIntPoint(-1,-1),
        FIntPoint(1,1),
        FIntPoint(-1,1),
        FIntPoint(1,-1),
    };

    for (FIntPoint Dir : Directions)
    {
        int32 NX = X + Dir.X;
        int32 NY = Y + Dir.Y;

        if (NX >= 0 && NX < GridSize && NY >= 0 && NY < GridSize)
        {
            Result.Add(FIntPoint(NX, NY));
        }
    }

    return Result;
}

TSubclassOf<AActor> AMapGenerator::GetBlockClassByHeight(int32 Value)
{
    if (Value <= 2)
    {
        return BlockTypes[0];
    }
    else if (Value >= 3 && Value <= 15)
    {
        int32 RandomMid = FMath::RandRange(1, 2);
        return BlockTypes[RandomMid];
    }
    else
    {
        return BlockTypes[3];
    }
}


void AMapGenerator::SpawnMap(FVector Offset)
{
    UWorld* World = GetWorld();
    if (!World) return;

    float HalfSize = (GridSize - 1) * CellSpacing * 0.5f;

    for (int32 x = 0; x < GridSize; x++)
    {
        for (int32 y = 0; y < GridSize; y++)
        {
            int32 Height = Matrix[x][y];

            FVector BaseLocation(
                x * CellSpacing - HalfSize,
                y * CellSpacing - HalfSize,
                0
            );

            BaseLocation += Offset;

            // If matrix value is 0 -> spawn Friend + Balloon
            if (Height == 0)
            {
                if (FriendClass)
                {
                    FVector FriendLocation = BaseLocation;
                    FriendLocation.Z = FriendSpawnZ;
                    World->SpawnActor<AActor>(FriendClass, FriendLocation, FRotator::ZeroRotator);
                }

                if (BalloonClass)
                {
                    FVector BalloonLocation = BaseLocation;
                    BalloonLocation.Z = BalloonSpawnZ;
                    World->SpawnActor<AActor>(BalloonClass, BalloonLocation, FRotator::ZeroRotator);
                }

                continue; // don't spawn blocks here
            }

            // Normal block spawning
            for (int32 z = 0; z < Height; z++)
            {
                TSubclassOf<AActor> BlockClass = GetBlockClassByHeight(z);
                if (!BlockClass) continue;

                FVector Location(
                    x * CellSpacing - HalfSize,
                    y * CellSpacing - HalfSize,
                    z * VerticalSpacing
                );

                Location += Offset;

                World->SpawnActor<AActor>(BlockClass, Location, FRotator::ZeroRotator);
            }
        }
    }
}



void AMapGenerator::SpawnBaseFloor(FVector Offset)
{
    UWorld* World = GetWorld();
    if (!World) return;

    int32 BaseSizeX = GridSize * 2 + 10;
    int32 BaseSizeY = GridSize + 10;

    float HalfBaseX = (BaseSizeX - 1) * CellSpacing * 0.5f;
    float HalfBaseY = (BaseSizeY - 1) * CellSpacing * 0.5f;

    for (int32 x = 0; x < BaseSizeX; x++)
    {
        for (int32 y = 0; y < BaseSizeY; y++)
        {
            FVector BaseLocation(
                x * CellSpacing - HalfBaseX,
                y * CellSpacing - HalfBaseY,
                -VerticalSpacing
            );

            BaseLocation += Offset;

            // Spawn base floor
            World->SpawnActor<AActor>(BlockTypes[4], BaseLocation, FRotator::ZeroRotator);

            // Check if edge cell
            bool bIsEdge = (x == 0 || x == BaseSizeX - 1 ||
                y == 0 || y == BaseSizeY - 1);

            if (bIsEdge)
            {
                // Spawn 3 additional stacked blocks
                for (int32 z = 1; z <= 3; z++)
                {
                    FVector WallLocation = BaseLocation + FVector(0, 0, z * VerticalSpacing);
                    World->SpawnActor<AActor>(BlockTypes[4], WallLocation, FRotator::ZeroRotator);
                }
            }
        }
    }
}


