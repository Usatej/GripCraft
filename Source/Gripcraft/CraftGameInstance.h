#pragma once

#include "CoreMinimal.h"
#include "Utils/CraftSaveUtils.h"
#include "CraftGameInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GRIPCRAFT_API UCraftGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCraftGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable, Category="Craft")
	bool CreateNewWorld(const FString& WorldName, const int32& WorldSeed, FString& WorldPath);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft")
	TSoftObjectPtr<UWorld> GetGameLevel() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft")
	FString GetGameLevelName() const;

	UFUNCTION(BlueprintCallable, Category="Craft")
	void SelectWorldToPlayIn(const FString& WorldPath);
	
	const FString& GetSelectedWorldPath() const;
private:
	mutable FString SelectedWorldPath;
	
	UPROPERTY(EditDefaultsOnly, Category="Craft")
	TSoftObjectPtr<UWorld> GameLevel;
	FDelegateHandle WorldTearDownHandle;
	//TODO: Would be best to move it to some EditorModule (custom debug menu) with own config.
private:
	/*
	 *	Used (only in PIE) when game is started right from GameLevel and not from MainMenu.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Craft|Debug")
	int32 PIESeed;
	/*
	 *	Determines whether save files should be deleted after leaving the world.
	 *	Used (only in PIE) when game is started right from GameLevel and not from MainMenu.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Craft|Debug")
	bool bPIECleanSaves;
	mutable bool bPIEDummyGeneration;
};
