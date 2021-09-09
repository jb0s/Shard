#pragma once

#include <vector>
#include <iostream>
#include <Windows.h>
#include "Enums.h"

#define PI (3.14159265358979323846264338327950288419716939937510)
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct { unsigned char MAKE_PAD(offset); type name;}




struct FVector2D
{
	float X;
	float Y;

	inline BOOL IsValid()
	{
		return X != NULL && Y != NULL;
	}

	inline float distance()
	{
		return sqrtf(this->X * this->X + this->Y * this->Y);
	}
};

struct FVector
{
	float X;
	float Y;
	float Z;

	FVector operator-(FVector buffer)
	{

		return FVector
		(
			{ this->X - buffer.X, this->Y - buffer.Y, this->Z - buffer.Z }
		);
	}

	FVector operator+(FVector buffer)
	{

		return FVector
		(
			{ this->X + buffer.X, this->Y + buffer.Y, this->Z + buffer.Z }
		);
	}

	inline float distance()
	{
		return sqrtf(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
	}
};
struct FText
{
	char UnknownData[0x18];
};

struct FRotator
{

	float Pitch;
	float Yaw;
	float Roll;

	FRotator ClampAngles()
	{

		if (this->Yaw < -180.0f)
			this->Yaw += 360.0f;

		if (this->Yaw > 180.0f)
			this->Yaw -= 360.0f;

		if (this->Pitch < -74.0f)
			this->Pitch = -74.0f;

		if (this->Pitch > 74.0f)
			this->Pitch = 74.0f;

		return *this;
	}

	static FRotator CalcAngle(FVector cameraLocation, FVector boneLocation)
	{

		FRotator result;

		auto delta = boneLocation - cameraLocation;
		auto distance = delta.distance();

		result.Yaw = atan2f(delta.Y, delta.X) * (180.0f / PI);
		result.Pitch = (-((acosf((delta.Z / distance)) * 180.0f / PI) - 90.0f));
		result.Roll = NULL;

		return result;
	}

};


template<class T>
struct TArray
{
	friend class FString;

public:
	inline TArray()
	{
		Data = NULL;
		Count = Max = 0;
	};

	inline INT Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline T& operator[](int i) const
	{
		return Data[i];
	};

	inline BOOL IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	INT32 Count;
	INT32 Max;
};

struct FQuat
{
	float X;
	float Y; // 0x04(0x04)
	float Z; // 0x08(0x04)
	float W; // 0x0c(0x04)
};

struct alignas(16) FTransform
{
	struct FQuat                                       Rotation;                                                 // 0x0000(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
	struct FVector                                     Translation;                                              // 0x0010(0x000C) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
	unsigned char                                      UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
	struct FVector                                     Scale3D;                                                  // 0x0020(0x000C) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
	unsigned char                                      UnknownData01[0x4];                                       // 0x002C(0x0004) MISSED OFFSET
};

struct FString : private TArray<wchar_t>
{
	inline const wchar_t* c_str() const
	{
		return Data;
	}

	FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	}

	bool IsValid() const
	{
		return Data != nullptr;
	}

	const wchar_t* ToWString() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

class UClass
{
public:
	BYTE _padding_0[0x40];
	UClass* SuperClass;
};

class UObject
{
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	BYTE _padding_0[0x8];
	UObject* Outer;

	inline void ProcessEvent(void* fn, void* parms)
	{
		auto vtable = *reinterpret_cast<void***>(this);
		auto processEventFn = static_cast<void(*)(void*, void*, void*)>(vtable[0x44]);
		processEventFn(this, fn, parms);
	}

	inline BOOLEAN IsA(PVOID parentClass)
	{
		for (auto super = this->Class; super; super = super->SuperClass) {
			if (super == parentClass) {
				return TRUE;
			}
		}

		return FALSE;
	}
};

class FUObjectItem
{
public:
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
	DWORD SerialNumber2;
};

class TUObjectArray
{
public:
	FUObjectItem* Objects[9];
};

class GObjects
{
public:

	inline void NumChunks(int* start, int* end) const
	{
		int cStart = 0, cEnd = 0;

		if (!cEnd)
		{
			while (1)
			{
				if (ObjectArray->Objects[cStart] == 0)
				{
					cStart++;
				}
				else
				{
					break;
				}
			}

			cEnd = cStart;
			while (1)
			{
				if (ObjectArray->Objects[cEnd] == 0)
				{
					break;
				}
				else
				{
					cEnd++;
				}
			}
		}

		*start = cStart;
		*end = cEnd;
	}

	inline UObject* GetByIndex(int32_t index) const
	{
		int cStart = 0, cEnd = 0;
		int chunkIndex = 0, chunkSize = 0xFFFF, chunkPos;
		FUObjectItem* Object;

		NumChunks(&cStart, &cEnd);

		chunkIndex = index / chunkSize;
		if (chunkSize * chunkIndex != 0 &&
			chunkSize * chunkIndex == index)
		{
			chunkIndex--;
		}

		chunkPos = cStart + chunkIndex;
		if (chunkPos < cEnd)
		{
#pragma warning( push )
#pragma warning( disable : 26451 )
			Object = ObjectArray->Objects[chunkPos] + (index - chunkSize * chunkIndex);
#pragma warning( pop )
			if (!Object) { return NULL; }

			return Object->Object;
		}

		return nullptr;
	}

	inline FUObjectItem* GetItemByIndex(int32_t index) const
	{
		int cStart = 0, cEnd = 0;
		int chunkIndex = 0, chunkSize = 0xFFFF, chunkPos;
		FUObjectItem* Object;

		NumChunks(&cStart, &cEnd);

		chunkIndex = index / chunkSize;
		if (chunkSize * chunkIndex != 0 &&
			chunkSize * chunkIndex == index)
		{
			chunkIndex--;
		}

		chunkPos = cStart + chunkIndex;
		if (chunkPos < cEnd)
		{
#pragma warning( push )
#pragma warning( disable : 26451 )
			Object = ObjectArray->Objects[chunkPos] + (index - chunkSize * chunkIndex);
#pragma warning( pop )
			if (!Object) { return NULL; }

			return Object;
		}

		return nullptr;
	}

	inline int32_t Num() const
	{
		return ObjectCount;
	}

public:
	TUObjectArray* ObjectArray;
	BYTE _padding_0[0xC];
	DWORD ObjectCount;
};

class cWeapon
{
public:
	union
	{
		DEFINE_MEMBER_N(float, LastFireTime, 0x9BC);
		DEFINE_MEMBER_N(float, LastFireTimeVerified, 0x9C0);
	};
};

class cActor
{
public:
	union
	{// TrackedNativeGlowFlags 0x01D8
		DEFINE_MEMBER_N(cWeapon*, CurrentWeapon, 0x600);
	};
};


class cPlayerController
{
public:
	union
	{
		DEFINE_MEMBER_N(cActor*, AcknowledgedPawn, 0x02A0);
		DEFINE_MEMBER_N(UObject*, CheatManager, 0x0338);
	};
};

class cConsole;

class cGameViewportClient
{
public:
	union
	{
		DEFINE_MEMBER_N(UObject*, ViewportConsole, 0x0040);
	};
};

class cEngine
{
public:
	union
	{
		DEFINE_MEMBER_N(UClass*, ConsoleClass, 0x00F8);
		DEFINE_MEMBER_N(cGameViewportClient*, GameViewportClient, 0x0788);
	};
};

class cLocalPlayer
{
public:
	union
	{
		DEFINE_MEMBER_N(cPlayerController*, PlayerController, 0x0030);
	};
};

class cGameInstance
{
public:
	union
	{
		DEFINE_MEMBER_N(cLocalPlayer**, LocalPlayers, 0x0038);
	};
};

class cLevel
{
public:
	union
	{
		DEFINE_MEMBER_N(cActor**, Actors, 0x0098);
		DEFINE_MEMBER_N(int32_t, ActorsCount, 0x00A0);
	};
};

class cUWorld
{
public:
	union
	{
		DEFINE_MEMBER_N(cLevel**, Levels, 0x0148);
		DEFINE_MEMBER_N(int32_t, LevelCount, 0x150);
		DEFINE_MEMBER_N(cGameInstance*, GameInstance, 0x180);
	};
};



struct SpawnObjectParams
{
	UClass* ObjectClass;
	UObject* Outer;
	UObject* ReturnValue;
};

struct addtoviewport {
	int ZOrder;
};

struct CheatScriptParams
{
	struct FString ScriptName;
	UObject* ReturnValue;
};

struct SayParmas
{
	struct FString Msg;
	UObject* ReturnValue;
};

struct JonLHack_GetAllObjectsOfClassFromPathParams
{
	struct FString Path;
	class UClass* Class;
	TArray<class UObject*> ReturnValue;
};

struct ApplyCharacterCosmeticsParams
{
	UObject* WorldContextObject;
	TArray<UObject*> CharacterParts;
	UObject* PlayerState;
	bool bSuccess;
};

struct GetPlayerControllerParams
{
	class UObject* WorldContextObject;
	int PlayerIndex;
	class APlayerController* ReturnValue;
};

struct UGameplayStatics_GetGameState_Params
{
	class UObject* WorldContextObject;
	class AGameStateBase* ReturnValue;
};
struct FPlaylistPropertyArray
{
	char padding[0x0120];
	UObject* BasePlaylist;
	UObject* OverridePlaylist;
};

struct UGameplayStatics_GetPlayerPawn_Params
{
	class UObject* WorldContextObject;
	int PlayerIndex;
	class APawn* ReturnValue;
};

struct PlayIntroAnim_Params
{
	EAthenaGamePhaseStep Step;
};
struct FActorSpawnParameters
{
	unsigned char Unk00[0x40];
};
UObject* (*SpawnActorLong)(UObject* World, UClass* Class, FVector* Location, FRotator* Rotation, FActorSpawnParameters& SpawnParameters);

struct bools
{
	unsigned char pad[0x888];
	char bHasInitiallySpawned : 1; // 0x888(0x01)
	char bAssignedStartSpawn : 1; // 0x888(0x01)
	char bReadyToStartMatch : 1; // 0x888(0x01)
	char bClientPawnIsLoaded : 1; // 0x888(0x01)
};
struct FCustomCharacterData {
	char WasPartReplicatedFlags; // 0x00(0x01)
	char pad_1[0x3]; // 0x01(0x03)
	int32_t RequiredVariantPartFlags; // 0x04(0x04)
	UObject* Parts[0x7]; // 0x08(0x38)
	 UObject* Charms[0x4]; // 0x40(0x20)
	TArray<struct UObject*> VariantRequiredCharacterParts; // 0x60(0x10)
	bool bReplicationFailed; // 0x70(0x01)
	char pad_71[0x7]; // 0x71(0x07)
};

struct switchlevel_params {
	FString URL;
};
struct FGuid
{
	int A;
	int B;
	int C;
	int D;
};

struct UFortKismetLibrary_ApplyCharacterCosmetics_Params
{
	UObject* WorldContextObject;
	TArray<UObject*> CharacterParts;
	UObject* PlayerState;
	bool bSuccess;
};
struct UFortKismetLibrary_UpdatePlayerCustomCharacterPartsVisualization_Params
{
	UObject* PlayerState;
};
struct AFortPlayerPawnAthena_TeleportToSkyDive_Params
{
	float HeightAboveGround;
};
struct LoadLevelinstance {
		UObject* WorldContextObject;
		FString LevelName;
		FVector Location;
		struct FRotator Rotation;
		bool bOutSuccess;
		FString OptionalLevelNameOverride;
};

template <class TEnum>
class TEnumAsByte
{
public:
	TEnumAsByte()
	{
	}

	TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit TEnumAsByte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit TEnumAsByte(uint8_t _value)
		: value(_value)
	{
	}

	operator TEnum() const
	{
		return static_cast<TEnum>(value);
	}

	TEnum GetValue() const
	{
		return static_cast<TEnum>(value);
	}

private:
	uint8_t value;
};


enum class EFortCustomGender : uint8_t {
	Invalid,
	Male,
	Female,
	Both,
	EFortCustomGender_MAX,
};

enum class EFortCustomBodyType : uint8_t {
	NONE,
	Small,
	Medium,
	MediumAndSmall,
	Large,
	LargeAndSmall,
	LargeAndMedium,
	All,
	Deprecated,
	EFortCustomBodyType_MAX,
};

enum class EMontagePlayReturnType : uint8_t {
	MontageLength,
	Duration,
	EMontagePlayReturnType_MAX,
};

struct UFortMontageItemDefinitionBase_GetAnimationHardReference_Params
{
	TEnumAsByte<EFortCustomBodyType> BodyType;
	TEnumAsByte<EFortCustomGender> Gender;
	UObject* PawnContext;
	UObject* ReturnValue;
};

struct UAnimInstance_Montage_Play_Params
{
	UObject* MontageToPlay;
	float InPlayRate;
	EMontagePlayReturnType ReturnValueType;
	float InTimeToStartMontageAt;
	bool bStopAllMontages;
	float ReturnValue;
};

struct USkeletalMeshComponent_GetAnimInstance_Params
{
	UObject* ReturnValue;
};

struct UAnimInstance_GetCurrentActiveMontage_Params
{
	UObject* ReturnValue;
};


struct UAnimInstance_Montage_Stop_Params
{
	float InBlendOutTime;
	UObject* Montage;
};

struct RBitField
{
	unsigned char A : 1;
	unsigned char B : 1;
	unsigned char C : 1;
	unsigned char D : 1;
	unsigned char E : 1;
	unsigned char F : 1;
	unsigned char G : 1;
	unsigned char H : 1;
};

struct UCharacterMovementComponent_SetMovementMode_Params
{
	TEnumAsByte<EMovementMode> NewMovementMode;
	unsigned char NewCustomMode;
};

struct FGameplayTag {
	char pad_59[0x8]; // 0x59(0x07)
};

struct ACharacter_IsInAircraft_Params
{
	bool ReturnValue;
};

struct FCosmeticVariantInfo {
	 FGameplayTag VariantChannelTag; // 0x00(0x08)
	 FGameplayTag ActiveVariantTag; // 0x08(0x08)
};


struct FMcpVariantChannelInfo : FCosmeticVariantInfo {
	char pad_59[0x20]; // 0x59(0x07)
	char pad_30[0x8]; // 0x20(0x07)
	char pad_38[0x10]; // 0x38(0x10)
};

struct FFortAthenaLoadout {
	FString BannerIconId; // 0x00(0x10)
	FString BannerColorId; // 0x10(0x10)
	UObject* SkyDiveContrail; // 0x20(0x08)
	 UObject* Glider; // 0x28(0x08)
	UObject* Pickaxe; // 0x30(0x08)
	bool bIsDefaultCharacter; // 0x38(0x01)
	char pad_39[0x7]; // 0x39(0x07)
	 UObject* Character; // 0x40(0x08)
	 TArray<struct FMcpVariantChannelInfo> CharacterVariantChannels; // 0x48(0x10)
	bool bForceUpdateVariants; // 0x58(0x01)
	char pad_59[0x7]; // 0x59(0x07)
	 UObject* Hat; // 0x60(0x08)
	UObject* Backpack; // 0x68(0x08)
	UObject* LoadingScreen; // 0x70(0x08)
	 UObject* BattleBus; // 0x78(0x08)
	UObject* VehicleDecoration; // 0x80(0x08)
	UObject* CallingCard; // 0x88(0x08)
	 UObject* MapMarker; // 0x90(0x08)
	 TArray<struct UObject*> Dances; // 0x98(0x10)
	 UObject* VictoryPose; // 0xa8(0x08)
	 UObject* MusicPack; // 0xb0(0x08)
	 UObject* ItemWrapOverride; // 0xb8(0x08)
	 TArray<struct UObject*> ItemWraps; // 0xc0(0x10)
	 UObject* CharmOverride; // 0xd0(0x08)
	 TArray<struct UObject*> Charms; // 0xd8(0x10)
	 UObject* PetSkin; // 0xe8(0x08)
	char pad_F0[0x50]; // 0xf0(0x50)
};
