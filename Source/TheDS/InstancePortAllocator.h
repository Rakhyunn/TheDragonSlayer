#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InstancePortAllocator.generated.h"

UCLASS()
class THEDS_API UInstancePortAllocator : public UObject
{
	GENERATED_BODY()
	
private:
    int32 Start = 0;
    int32 End = 0; 
    int32 Next = 0;
    TSet<int32> Used;

public:
    void Init(int32 InStart = 7779, int32 InEnd = 7800);
    bool TryAcquire(int32& OutPort);
    void Release(int32 Port);
};
