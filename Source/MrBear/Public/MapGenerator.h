#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapGenerator.generated.h"

UCLASS()
class MRBEAR_API AMapGenerator : public AActor
{
    GENERATED_BODY()

public:
    AMapGenerator();

protected:
    virtual void BeginPlay() override;

    void GenerateHillMatrix();
    void GenerateMazeMatrix();
    void SpawnBaseFloor(FVector MapOffset);
    void SpawnMap(FVector MapOffset);
    TArray<FIntPoint> GetEmptyCells();
    bool GetRandomEmptyCell(int32& OutX, int32& OutY);
    TArray<FIntPoint> GetNeighbors(int32 X, int32 Y);
    TArray<FIntPoint> GetAdjacents(int32 X, int32 Y);
    FIntPoint DigPathToEdge(FIntPoint Start);
    void AddFakePaths(FIntPoint MainEntrance, int32 NumPaths);
    bool IsEdgeCell(FIntPoint Cell);
    FIntPoint GetRandomEdgeCell();
    TSubclassOf<AActor> GetBlockClassByHeight(int32 Value);

public:

    // Grid size (N x N)
    UPROPERTY(EditAnywhere, Category = "Map")
    int32 GridSize = 6;

    // Horizontal spacing
    UPROPERTY(EditAnywhere, Category = "Map")
    float CellSpacing = 200.f;

    // Vertical stacking spacing
    UPROPERTY(EditAnywhere, Category = "Map")
    float VerticalSpacing = 100.f;
    UPROPERTY(EditAnywhere, Category = "Map")
    float NoiseScale = 0.1f;
    UPROPERTY(EditAnywhere, Category = "Map")
    float WallThreshold = 0.1f;

    // Block actor classes
    UPROPERTY(EditAnywhere, Category = "Map")
    TArray<TSubclassOf<AActor>> BlockTypes;

    //Balloon Actor
    UPROPERTY(EditAnywhere, Category = "Friend")
    TSubclassOf<AActor> BalloonClass;
    UPROPERTY(EditAnywhere, Category = "Friend")
    TSubclassOf<AActor> FriendClass;
    UPROPERTY(EditAnywhere, Category = "Friend")
    float FriendSpawnZ;
    UPROPERTY(EditAnywhere, Category = "Friend")
    float BalloonSpawnZ;

private:
    TArray<TArray<int32>> Matrix;
};
