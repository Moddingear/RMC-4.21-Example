// Fill out your copyright notice in the Description page of Project Settings.

#include "CppRMCTester.h"

// Sets default values
ACppRMCTester::ACppRMCTester()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Mesh section Tester"));
}

// Called when the game starts or when spawned
void ACppRMCTester::BeginPlay()
{
	Super::BeginPlay();

	URuntimeMeshComponent* root = Cast<URuntimeMeshComponent>(RootComponent);
	FRuntimeMeshDataRef rootdata = root->GetOrCreateRuntimeMesh()->GetRuntimeMeshData(); //Get the mesh data from the root mesh, technically superfluous and bypasses IsInGameThread check
	for(int i = 0; i < 6; i++)
	{
		TArray<FRuntimeMeshVertexSimple> verts; TArray<int32> tris; //Use packed vertices
		URuntimeMeshShapeGenerator::CreateBoxMesh(FVector(100, 100, 100), verts, tris); //Get a box mesh
		for (int j = 0; j < verts.Num(); j++)
		{
			verts[j].Position += FVector(0,300*i,0); //Move the verts slightly for each new section so they aren't stacked on top of each other
		}
		root->CreateMeshSection(i, verts, tris, true, EUpdateFrequency::Average, ESectionUpdateFlags::None);
	}
	
	URuntimeMeshComponent* ConvexComp = NewObject<URuntimeMeshComponent>(root); //Create the component at runtime because why not ? It's a realistic test imo.
	ConvexComp->RegisterComponent();
	ConvexComp->AttachTo(root);
	ConvexComp->SetRelativeTransform(FTransform(FVector(300, 0, 0)));
	TArray<FVector> verts = {FVector(-100,-100,-100), //Front bottom left
		FVector(100,100,-100), //Back right bottom
		FVector(100,-100,100), //Front right top
		FVector(-100,100,100)}; //Back left top
	//Verts for a tetrahedron, technically there should be 12 verts for a proper hard shading but that's ok i guess
	TArray<int32> tris = {0,2,1,
		3,0,1,
		0,3,2,
		1,2,3};
	TArray<FVector2D> uv = { FVector2D(0,0),
		FVector2D(0,1),
		FVector2D(1,0),
		FVector2D(1,1) };
	//The number corresponds to the index of the vertex
	//To invert the normal of a face, just flip two of it's vertices
	ConvexComp->CreateMeshSection(0, verts, tris, TArray<FVector>(), uv, TArray<FColor>(), TArray<FRuntimeMeshTangent>(), true, EUpdateFrequency::Average, ESectionUpdateFlags::CalculateNormalTangentHard);
	ConvexComp->AddConvexCollisionSection(verts);
	ConvexComp->SetCollisionUseAsyncCooking(false);
	ConvexComp->SetCollisionUseComplexAsSimple(false);
	ConvexComp->SetMassOverrideInKg(NAME_None, 100.0f, true); //Make the spawned object lighter
	ConvexComp->SetSimulatePhysics(true);
}

// Called every frame
void ACppRMCTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

