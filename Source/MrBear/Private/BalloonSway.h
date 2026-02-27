// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BalloonSway.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UBalloonSway : public UActorComponent
{
    GENERATED_BODY()

public:
    UBalloonSway();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balloon Sway")
    float SwaySpeed = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balloon Sway")
    float SwayAmount = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balloon Sway")
    float RotationAmount = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balloon Sway")
    bool bSwayPosition = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balloon Sway")
    bool bSwayRotation = true;

private:
    FVector StartLocation;
    FRotator StartRotation;
};
