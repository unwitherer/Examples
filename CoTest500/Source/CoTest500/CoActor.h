
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Generator.h"
#include "CoActor.generated.h"

UCLASS()
class COTEST500_API ACoActor : public AActor
{
	GENERATED_BODY()
	
public:
	ACoActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	TOptional<FGenerator> CoGenerator;
	FGenerator TestCoroutine();

};
