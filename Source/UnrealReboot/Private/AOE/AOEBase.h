// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../DamageSystem/DamageSystem.h"
#include "AOEBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAOEOverlapActor,AActor*, DamageCauser);


UCLASS()
class AAOEBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAOEBase();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "OnAOEOver")
	FOnAOEOverlapActor OnAOEOverlapActor;

	UFUNCTION()
	void SpawnAOESphere();

	UFUNCTION()
	void Trigger();

	UFUNCTION()
	void SetRadius(float Rad);



private:
	float radius;
	bool bDrawDebugSphere;
	bool ignoreInstigator;
	bool triggerOnBeginPlay;





public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetOptionOfAOE(bool DrawDebugsphere,bool IgnoreInstigator, bool triggerOnBeginPlay);

	

//DamageInfo ÀúÀå¿ë
private:
	FDamageInfo StoredDamageInfo;

public:
	void SetDamageInfo(FDamageInfo& NewDamageInfo) { StoredDamageInfo = NewDamageInfo; }
	FDamageInfo& GetDamageInfo()  { return StoredDamageInfo; }


};
