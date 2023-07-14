#include "SGAnimNotifyState_TimedNiagaraEffect.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UObject/UObjectGlobals.h"

USGAnimNotifyState_TimedNiagaraEffect::USGAnimNotifyState_TimedNiagaraEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


UNiagaraComponent* USGAnimNotifyState_TimedNiagaraEffect::SpawnSystemAttached(UNiagaraSystem* SystemTemplate,
	USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale,
	EAttachLocation::Type LocationType, bool bAutoDestroy, bool bAutoActivate, ENCPoolMethod PoolingMethod,
	bool bPreCullCheck)
{
	FFXSystemSpawnParameters SpawnParams;
	SpawnParams.SystemTemplate = SystemTemplate;
	SpawnParams.AttachToComponent = AttachToComponent;
	SpawnParams.AttachPointName = AttachPointName;
	SpawnParams.Location = Location;
	SpawnParams.Rotation = Rotation;
	SpawnParams.Scale = Scale;
	SpawnParams.LocationType = LocationType;
	SpawnParams.bAutoDestroy = bAutoDestroy;
	SpawnParams.bAutoActivate = bAutoActivate;
	SpawnParams.PoolingMethod = ToPSCPoolMethod(PoolingMethod);
	SpawnParams.bPreCullCheck = bPreCullCheck;
	return UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(SpawnParams);
	
	//return UNiagaraFunctionLibrary::SpawnSystemAttached(SystemTemplate, AttachToComponent, AttachPointName, Location, Rotation, Scale, LocationType, bAutoDestroy, PoolingMethod, bAutoActivate, bPreCullCheck);
}

UFXSystemComponent* USGAnimNotifyState_TimedNiagaraEffect::SpawnEffect(USkeletalMeshComponent* MeshComp,
                                                                      UAnimSequenceBase* Animation) const
{
	// Only spawn if we've got valid params
	if (ValidateParameters(MeshComp))
	{
		return SpawnSystemAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset, SpawnScale, EAttachLocation::KeepRelativeOffset, !bDestroyAtEnd);
	}
	return nullptr;

	// Only spawn if we've got valid params
	/*
	if (ValidateParameters(MeshComp))
	{
		return UNiagaraFunctionLibrary::SpawnSystemAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset, SpawnScale, EAttachLocation::KeepRelativeOffset, !bDestroyAtEnd);
	}
	return nullptr;
	*/
}
