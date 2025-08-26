#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PartyType.h"
#include "TheDSPlayerState.generated.h"

UCLASS()
class THEDS_API ATheDSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_PartyLeader)
	APlayerState* PartyLeader;

	UPROPERTY(ReplicatedUsing = OnRep_PartyMembers)
	TArray<FPartyMember> PartyMembers;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FString Nickname;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FName LastVisitedVillage = NAME_None;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FTransform LastVillageSpawn;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FName LastVisitedField = NAME_None;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FString GetNickname() const { return Nickname; }
	void SetNickname(const FString& InNickname) { Nickname = InNickname; }

	int32 GetLevel() const;

	void SetPartyLeader(APlayerState* Leader);
	APlayerState* GetPartyLeader() const;

	// 현재 파티 리더인지 여부 확인
	bool IsPartyLeader() const { return PartyLeader == this; }

	// 현재 파티에 소속되어 있는지 여부 확인
	bool IsInParty() const { return PartyLeader != nullptr; }

	UFUNCTION()
	void OnRep_PartyLeader();

	UFUNCTION()
	void OnRep_PartyMembers();

	UFUNCTION()
	void SetLastVillage(FName Village, const FTransform& Spawn);

	UFUNCTION()
	void SetLastField(FName Field);

	const TArray<FPartyMember>& GetReplicatedPartyMembers() const { return PartyMembers; }
	void SetReplicatedPartyMembers(const TArray<FPartyMember>& Members);

	const FName& GetLastVillage() const { return LastVisitedVillage; }
	const FName& GetLastField() const { return LastVisitedField; }
	const FTransform& GetLastVillageSpawn() const { return LastVillageSpawn; }
};
