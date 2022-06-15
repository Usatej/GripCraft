// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/CraftInventoryComponent.h"

// Sets default values
ACraftCharacter::ACraftCharacter(): BuildingDistance(4)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(45.f, 96.0f);

	// Create a camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-40.f, 1.75f, 75.f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	BuildingViewComponent = CreateDefaultSubobject<UCraftBuildingViewComponent>(TEXT("BuildingComponent"));
	MiningComponent = CreateDefaultSubobject<UCraftMiningComponent>(TEXT("MiningComponent"));
	InventoryComponent = CreateDefaultSubobject<UCraftInventoryComponent>(TEXT("InventoryComponent"));

	// Create a selected item mesh component
	SelectedItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectedItemMesh"));
	SelectedItemMeshComponent->SetOnlyOwnerSee(true);
	SelectedItemMeshComponent->SetupAttachment(CameraComponent);
	SelectedItemMeshComponent->bCastDynamicShadow = false;
	SelectedItemMeshComponent->CastShadow = false;
	SelectedItemMeshComponent->SetRelativeLocation(FVector(30.f, 22.f, -37.f));
	SelectedItemMeshComponent->SetRelativeRotation(FRotator(19.f, 8.f, -90.f));	
}

int32 ACraftCharacter::GetBuildingDistanceInWorldUnits() const
{
	return BuildingDistance * CraftWorldConstants::WORLD_BLOCK_SIZE;
}

const UCraftBuildingViewComponent* ACraftCharacter::GetBuildingViewComponent() const
{
	return BuildingViewComponent;
}

UCraftInventoryComponent* ACraftCharacter::GetInventoryComponent() const
{
	return InventoryComponent;
}

// Called when the game starts or when spawned
void ACraftCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACraftCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind actions events
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ACraftCharacter::OnPrimaryAction);
	PlayerInputComponent->BindAction("SecondaryAction", IE_Pressed, this, &ACraftCharacter::OnSecondaryAction);
	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACraftCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACraftCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRight", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void ACraftCharacter::OnPrimaryAction()
{
}

void ACraftCharacter::OnSecondaryAction()
{
}

void ACraftCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACraftCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}
