#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EndingBookData.generated.h"

USTRUCT(BlueprintType)
struct FEndingPage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
	FText Body;
};

UCLASS()
class THEDS_API UEndingBookData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEndingPage> Pages;
};
