/**
 * Copyright (c) 2020-2021 Kareem Olim (Kemo)
 * All Rights Reserved. Licensed under the Neo License
 * https://neonite.dev/LICENSE.html
 */

#pragma once
#include "ue4.h"
#include "neoroyale.h"
#include "hwid.h"
#include "kismet.h"

#ifndef PROD
//#define LOGGING
#endif

using namespace NeoRoyale;

inline bool bIsDebugCamera;
inline bool bIsFlying;

inline void* ProcessEventDetour(UObject* pObj, UFunction* pFunc, void* pParams)
{
	auto nObj = pObj->GetName();
	auto nFunc = pFunc->GetName();

	if (!ProdMode)
	{
		/*if (gUrl.find(XOR("matchmakingservice")) != std::string::npos)
		{
			printf(XOR("\n\n[NeoRoyale] Start!"));

			//TODO: clean this mess;
			std::string url = gUrl;
			gUrl.clear();
			std::string query = url.erase(0, url.find(XOR("%3A")) + 1);
			query.erase(url.find("&"), url.size());
			query.erase(0, url.find(XOR("playlist")));
			std::string PlaylistName = query + "." + query;
			const std::wstring PlaylistNameW(PlaylistName.begin(), PlaylistName.end());

			auto Playlist = UE4::FindObject<UObject*>(PlaylistNameW.c_str(), true, true);
			auto Map = APOLLO_TERRAIN;

			if (PlaylistNameW.find(XOR(L"papaya")) != std::string::npos && !gPlaylist)
			{
				Map = APOLLO_PAPAYA;
			}

			if (PlaylistNameW.find(XOR(L"yogurt")) != std::string::npos && !gPlaylist)
			{
				Map = APOLLO_TERRAIN_YOGURT;
				gPlaylist = UE4::FindObject<UObject*>(XOR(L"FortPlaylistAthena /Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab"));
			}

			if (Playlist && !gPlaylist)
			{
				gPlaylist = Playlist;
			}
			else if (!Playlist && !gPlaylist)
			{
				gPlaylist = UE4::FindObject<UObject*>(XOR(L"FortPlaylistAthena /Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab"));
			}

#ifndef PROD
			if (HWID::Validate())
			{
				Start(Map);
			}
			else
			{
				MessageBoxA(nullptr, XOR("Your pc isn't activated, please dm kemo#1337 on discord."), XOR("Cranium HWID System"), MB_OK);
			}
#else
			Start(Map);
#endif
		}*/

		if (wcsstr(nFunc.c_str(), XOR(L"ReadyToStartMatch")) && bIsStarted && !bIsInit)
		{
			printf(XOR("\n[NeoRoyale] Init!\n"));
			Init();
		}

		if (wcsstr(nFunc.c_str(), XOR(L"DynamicHandleLoadingScreenVisibilityChanged")) && wcsstr(
			nObj.c_str(), XOR(L"AthenaLobby")))
		{
			if (bIsDebugCamera) bIsDebugCamera = !bIsDebugCamera;
			UFunctions::RegionCheck();
		}

		if (wcsstr(nFunc.c_str(), XOR(L"ServerLoadingScreenDropped")) && bIsInit && bIsStarted)
		{
			if (gVersion > 14.30)
			{
				//UFunctions::SetupCustomInventory();
			}

			//UFunctions::PlayCustomPlayPhaseAlert();
			//LoadMoreClasses();
		}


		// NOTE: (irma) This is better.
		if (wcsstr(nFunc.c_str(), XOR(L"ServerAttemptAircraftJump")))
		{
			//Console::ExecuteConsoleCommand(XOR(L"PAUSESAFEZONE"));
			NeoPlayer.Respawn();
			NeoPlayer.TeleportTo(NeoPlayer.GetLocation());
		}

		if (bIsInit)
		{
			if (bWantsToJump)
			{
				NeoPlayer.Jump();
				bWantsToJump = false;
			}

			else if (bWantsToOpenGlider)
			{
				NeoPlayer.ForceOpenParachute();
				bWantsToOpenGlider = false;
			}

			else if (bWantsToSkydive)
			{
				NeoPlayer.Skydive();
				bWantsToSkydive = false;
			}

			else if (bWantsToShowPickaxe)
			{
				NeoPlayer.ShowPickaxe();
				bWantsToShowPickaxe = false;
			}
		}

		if (wcsstr(nFunc.c_str(), XOR(L"OnWeaponEquipped")))
		{
			auto params = static_cast<AFortPawn_OnWeaponEquipped_Params*>(pParams);

			auto OldWeapon = params->PrevWeapon;

			if (OldWeapon && !Util::IsBadReadPtr(OldWeapon))
			{
				UFunctions::DestroyActor(OldWeapon);
				OldWeapon = nullptr;
			}
		}

		if (wcsstr(nFunc.c_str(), XOR(L"BP_OnDeactivated")) && wcsstr(nObj.c_str(), XOR(L"PickerOverlay_EmoteWheel")))
		{
			if (NeoPlayer.Pawn)
			{
				ObjectFinder PlayerControllerFinder = ObjectFinder::EntryPoint(uintptr_t(NeoPlayer.Controller));

				ObjectFinder LastEmotePlayedFinder = PlayerControllerFinder.Find(XOR(L"LastEmotePlayed"));

				auto LastEmotePlayed = LastEmotePlayedFinder.GetObj();

				if (LastEmotePlayed && !Util::IsBadReadPtr(LastEmotePlayed))
				{
					NeoPlayer.Emote(LastEmotePlayed);
					for (auto i = 0; i < Bots.size(); i++)
					{
						auto Bot = Bots[i];
						if (Bot.Pawn)
						{
							Bot.Emote(LastEmotePlayed);
						}
					}
				}
			}
		}

		if (wcsstr(nFunc.c_str(), XOR(L"ReceiveHit")) && nObj.starts_with(XOR(L"Prj_Athena_FrenchYedoc_JWFriendly_C")))
		{
			Player Bot;
			const auto Params = static_cast<AActor_ReceiveHit_Params*>(pParams);
			auto HitLocation = Params->HitLocation;

			HitLocation.Z = HitLocation.Z + 200;

			Bot.Pawn = UE4::SpawnActorEasy(UE4::FindObject<UClass*>(XOR(
				                               L"BlueprintGeneratedClass /Game/Athena/AI/Phoebe/BP_PlayerPawn_Athena_Phoebe.BP_PlayerPawn_Athena_Phoebe_C")),
			                               HitLocation);

			if (Bot.Pawn)
			{
				Bot.SetSkeletalMesh(XOR(L"SK_M_MALE_Base"));
				Bot.Emote(UE4::FindObject<UObject*>(XOR(L"EID_HightowerSquash.EID_HightowerSquash"), true));

				Bots.push_back(Bot);
			}
		}

		if (wcsstr(nFunc.c_str(), XOR(L"BlueprintOnInteract")) && nObj.starts_with(
			XOR(L"BGA_FireExtinguisher_Pickup_C_")))
		{
			NeoPlayer.EquipWeapon(XOR(L"WID_FireExtinguisher_Spray"));
		}


		if (wcsstr(nFunc.c_str(), XOR(L"ExecuteConsoleCommand")))
		{
			FString ScriptNameF = static_cast<UKismetSystemLibrary_ExecuteConsoleCommand_Params*>(pParams)->Command;

			if (ScriptNameF.IsValid())
			{
				std::wstring ScriptNameW = ScriptNameF.ToWString();

				std::wstring arg;

				if (ScriptNameW.find(L" ") != std::wstring::npos)
				{
					arg = ScriptNameW.substr(ScriptNameW.find(L" ") + 1);
				}

				auto CMD = str2enum(ScriptNameW.c_str());

				switch (CMD)
				{
				case TEST:
					{
						break;
					}

				case DUMP:
					{
						UE4::DumpGObjects();
						break;
					}

				case DUMPBPS:
					{
						UE4::DumpBPs();
						break;
					}
#ifndef PROD
				case ACTIVATE:
					{
						if (!arg.empty())
						{
							if (!HWID::WriteKeyToReg(const_cast<wchar_t*>(arg.c_str())))
							{
								UFunctions::ConsoleLog(XOR(L"Couldn't process your activation code!."));
							}
						}
						else
						{
							UFunctions::ConsoleLog(XOR(L"Please input your activation key!."));
						}
						break;
					}
#endif
				case EVENT:
					{
						if (gVersion == 14.60f)
						{
							UFunctions::Play(GALACTUS_EVENT_PLAYER);
						}
						else if (gVersion == 12.41f)
						{
							UFunctions::Play(JERKY_EVENT_PLAYER);
						}
						else if (gVersion == 12.61f)
						{
							UFunctions::Play(DEVICE_EVENT_PLAYER);
						}
						else if (gVersion == 16.00f)
						{
							UFunctions::Play(YOUGURT_EVENT_PLAYER);
						}
						else
						{
							UFunctions::ConsoleLog(
								XOR(L"Sorry the version you are using doesn't have any event we support."));
						}
						break;
					}

				case DEBUG_CAMERA:
					{
						//bIsDebugCamera = !bIsDebugCamera;
						break;
					}

				case FLY:
					{
						NeoPlayer.Fly(bIsFlying);
						bIsFlying = !bIsFlying;
						break;
					}

				case EQUIP:
					{
						if (!arg.empty())
						{
							NeoPlayer.EquipWeapon(arg.c_str());
						}
						else
						{
							UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
						}
						break;
					}

				case SET_MAX_HEALTH:
					{
						if (!arg.empty())
						{
							auto n = std::stof(arg);
							NeoPlayer.SetMaxHealth(n);
						}
						else
						{
							UFunctions::ConsoleLog(
								XOR(L"This command requires an argument, e.g: (cheatscript setmaxhealth 1000)"));
						}
						break;
					}

				case SET_MAX_SHIELD:
					{
						if (!arg.empty())
						{
							auto n = std::stof(arg);
							NeoPlayer.SetMaxShield(n);
						}
						else
						{
							UFunctions::ConsoleLog(
								XOR(L"This command requires an argument e.g: (cheatscript setmaxshield 1000)"));
						}
						break;
					}

				case SET_HEALTH:
					{
						if (!arg.empty())
						{
							auto n = std::stof(arg);
							NeoPlayer.SetHealth(n);
						}
						else
						{
							UFunctions::ConsoleLog(
								XOR(L"This command requires an argument e.g: (cheatscript sethealth 1000)"));
						}
						break;
					}

				case SET_SHIELD:
					{
						if (!arg.empty())
						{
							auto n = std::stof(arg);
							NeoPlayer.SetShield(n);
						}
						else
						{
							UFunctions::ConsoleLog(
								XOR(L"This command requires an argument e.g: (cheatscript setshiled 1000)"));
						}
						break;
					}

				case SET_SPEED:
					{
						if (!arg.empty())
						{
							auto n = std::stof(arg);
							NeoPlayer.SetMovementSpeed(n);
						}
						else
						{
							UFunctions::ConsoleLog(
								XOR(L"This command requires an argument e.g: (cheatscript setspeed 1000)"));
						}
						break;
					}

				case SET_GRAVITY:
					{
						if (!arg.empty())
						{
							auto n = std::stof(arg);
							NeoPlayer.SetPawnGravityScale(n);
						}
						else
						{
							UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
						}
						break;
					}

				case SET_PLAYLIST:
					{
						if (!arg.empty())
						{
							auto Playlist = UE4::FindObject<UObject*>(ScriptNameW.c_str());
							if (Playlist)
							{
								gPlaylist = Playlist;
							}
							else
							{
								UFunctions::ConsoleLog(XOR(L"Couldn't find the requested playlist!."));
							}
						}
						else
						{
							UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
						}
						break;
					}

				case SKYDIVE:
					{
						NeoPlayer.StartSkydiving(0);
						NeoPlayer.StartSkydiving(0);
						NeoPlayer.StartSkydiving(0);
						NeoPlayer.StartSkydiving(1500.0f);
						break;
					}

				case RESPAWN:
					{
						NeoPlayer.Respawn();
					}

				case LOADBPC:
					{
						if (!arg.empty())
						{
							const auto BPGClass = UE4::FindObject<UClass*>(
								XOR(L"Class /Script/Engine.BlueprintGeneratedClass"));

							UE4::StaticLoadObjectEasy(BPGClass, arg.c_str());
						}
						else
						{
							UFunctions::ConsoleLog(XOR(L"This command requires an argument"));
						}
						break;
					}

				default:
					break;
				}
			}
		}
	}

	//Server functions
	if (wcsstr(nFunc.c_str(), XOR(L"SendClientHello")) ||
		wcsstr(nFunc.c_str(), XOR(L"SendPacketToServer")) ||
		wcsstr(nFunc.c_str(), XOR(L"SendPacketToClient")) ||
		//Disable change window mode or size to prevent crashes
		wcsstr(nFunc.c_str(), XOR(L"SetFullscreenMode")) ||
		wcsstr(nFunc.c_str(), XOR(L"SetRenderingAPI")))
	{
		return nullptr;
	}

	if (wcsstr(nFunc.c_str(), XOR(L"OnRep_ToSpawnList")))
	{
	}

#ifdef LOGGING
	//Logging
	if (!wcsstr(nFunc.c_str(), L"EvaluateGraphExposedInputs") &&
		!wcsstr(nFunc.c_str(), L"Tick") &&
		!wcsstr(nFunc.c_str(), L"OnSubmixEnvelope") &&
		!wcsstr(nFunc.c_str(), L"OnSubmixSpectralAnalysis") &&
		!wcsstr(nFunc.c_str(), L"OnMouse") &&
		!wcsstr(nFunc.c_str(), L"Pulse") &&
		!wcsstr(nFunc.c_str(), L"IsAcceptablePositionForPlacement") &&
		!wcsstr(nFunc.c_str(), L"OnContextualReticleChanged") &&
		!wcsstr(nFunc.c_str(), L"OnUpdateVisuals") &&
		!wcsstr(nFunc.c_str(), L"OnUpdateScale") &&
		!wcsstr(nFunc.c_str(), L"SetScalarParameterValueOnAllPreviewMIDs") &&
		!wcsstr(nFunc.c_str(), L"BlueprintUpdateAnimation") &&
		!wcsstr(nFunc.c_str(), L"BlueprintPostEvaluateAnimation") &&
		!wcsstr(nFunc.c_str(), L"BlueprintModifyCamera") &&
		!wcsstr(nFunc.c_str(), L"BlueprintModifyPostProcess") &&
		!wcsstr(nFunc.c_str(), L"Loop Animation Curve") &&
		!wcsstr(nFunc.c_str(), L"UpdateTime") &&
		!wcsstr(nFunc.c_str(), L"GetMutatorByClass") &&
		!wcsstr(nFunc.c_str(), L"UpdatePreviousPositionAndVelocity") &&
		!wcsstr(nFunc.c_str(), L"IsCachedIsProjectileWeapon") &&
		!wcsstr(nFunc.c_str(), L"LockOn") &&
		!wcsstr(nFunc.c_str(), L"GetAbilityTargetingLevel") &&
		!wcsstr(nFunc.c_str(), L"ReadyToEndMatch"))
	{
		printf(XOR("[Object]: %ws [Function]: %ws\n"), nObj.c_str(), nFunc.c_str());
	}
#endif

out:
	return ProcessEvent(pObj, pFunc, pParams);
}

/*
namespace CameraHook
{
	inline float Speed = 0.1;
	inline float FOV = 52.0;
	inline FVector Camera(52.274170, 125912.695313, 89.249969);
	inline FRotator Rotation = {0.870931, -88.071960, 0.008899};
}

inline int GetViewPointDetour(void* pPlayer, FMinimalViewInfo* pViewInfo, BYTE stereoPass)
{
	const auto CurrentViewPoint = GetViewPoint(pPlayer, pViewInfo, stereoPass);

	if (bIsDebugCamera)
	{
		//fov increase and decrease
		if (GetAsyncKeyState(VK_OEM_PLUS) == 0) CameraHook::FOV += CameraHook::Speed;

		if (GetAsyncKeyState(VK_OEM_MINUS) == 0) CameraHook::FOV -= CameraHook::Speed;

		//froward and backward left and right
		if (GetAsyncKeyState(0x57) == 0) CameraHook::Camera.Y += CameraHook::Speed;

		if (GetAsyncKeyState(0x53) == 0) CameraHook::Camera.Y -= CameraHook::Speed;

		if (GetAsyncKeyState(0x41) == 0) CameraHook::Camera.X += CameraHook::Speed;

		if (GetAsyncKeyState(0x44) == 0) CameraHook::Camera.X -= CameraHook::Speed;

		//up and down
		if (GetAsyncKeyState(0x45) == 0) CameraHook::Camera.Z += CameraHook::Speed;

		if (GetAsyncKeyState(0x51) == 0) CameraHook::Camera.Z -= CameraHook::Speed;


		//looking around
		if (GetAsyncKeyState(VK_UP) == 0) CameraHook::Rotation.Pitch -= CameraHook::Speed;

		if (GetAsyncKeyState(VK_DOWN) == 0) CameraHook::Rotation.Pitch += CameraHook::Speed;

		if (GetAsyncKeyState(VK_LEFT) == 0) CameraHook::Rotation.Yaw += CameraHook::Speed;

		if (GetAsyncKeyState(VK_RIGHT) == 0) CameraHook::Rotation.Yaw -= CameraHook::Speed;

		//assign our hooked variables
		pViewInfo->Location.X = CameraHook::Camera.X;
		pViewInfo->Location.Y = CameraHook::Camera.Y;
		pViewInfo->Location.Z = CameraHook::Camera.Z;

		pViewInfo->Rotation.Pitch = CameraHook::Rotation.Pitch;
		pViewInfo->Rotation.Yaw = CameraHook::Rotation.Yaw;
		pViewInfo->Rotation.Roll = CameraHook::Rotation.Roll;

		pViewInfo->FOV = CameraHook::FOV;
	}

	return CurrentViewPoint;
}
*/
