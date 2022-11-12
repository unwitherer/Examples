
#include "CoActor.h"

ACoActor::ACoActor()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ACoActor::BeginPlay()
{
	Super::BeginPlay();
	CoGenerator = TestCoroutine();
}
void ACoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CoGenerator.GetValue().Resume();
}
FGenerator ACoActor::TestCoroutine()
{
	co_await 1;
	co_await 2;
	co_await 3; // suspend!
	co_await 4;
	co_await 5;
	co_await 6; // suspend!
	co_await 7;
	co_await 8;
	co_await 9; // suspend!
	co_await 10;
	co_await 11;
	co_await 12; // suspend!
	co_await 13; // suspend!
	co_await 14;
	co_await 15; // suspend!
}