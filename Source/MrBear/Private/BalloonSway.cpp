#include "BalloonSway.h"
#include "GameFramework/Actor.h"

UBalloonSway::UBalloonSway()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UBalloonSway::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        StartLocation = Owner->GetActorLocation();
        StartRotation = Owner->GetActorRotation();
    }
}

void UBalloonSway::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner) return;

    float Time = GetWorld()->GetTimeSeconds();
    float SwayValue = FMath::Sin(Time * SwaySpeed);

    if (bSwayPosition)
    {
        FVector NewLocation = StartLocation;
        NewLocation.Y += SwayValue * SwayAmount;  // sway left/right on Y axis
        Owner->SetActorLocation(NewLocation);
    }

    if (bSwayRotation)
    {
        FRotator NewRotation = StartRotation;
        NewRotation.Roll += SwayValue * RotationAmount;
        Owner->SetActorRotation(NewRotation);
    }
}
