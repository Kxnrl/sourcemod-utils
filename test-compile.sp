#include "smutils.inc"

public void OnPluginStart()
{
    char chars[32];
    bool boolean;
    Handle handle;
    float decimal;
    float float3[3];
    int integer;
    int[] integers = new int [65];

    /**************************
                Global
    **************************/
    boolean = ClientIsKyle(0);
    integer = GetKyleIndex();

    /**************************
                Client
    **************************/
    boolean = ClientIsValid(0);
    boolean = ClientIsAlive(0);
    integer = FindClientByAccount(0);
    integer = FindClientBySteamId(AuthId_SteamID64, "76561198048432253");
    integer = GetRandomClient(false, 0);
    boolean = HasClientUserFlags(0, ADMFLAG_ROOT);
    boolean = HasClientAdminFlags(0, Admin_Root);

    SetClientFrags(0, 0);
    SetClientDeaths(0, 0);
    SetClientMoney(0, 0);
    SetClientKevlar(0, 0);
    SetClientHelmet(0, false);
    SetClientHud(0, 0);
    SetClientWallHack(0, 0.0);
    GetClientAimTargetPos(0, float3);

    integer = GetTeamPlayers(0, false);
    integer = GetPlayers(false, false);
    integer = GetPlayersEx(true, integer, integer);

    integer = ConvertSteam64ToSteam32("76561198048432253", chars, 32);
    integer = NumberToChar(0);
    integer = CharToNumber(0);

    /**************************
                Date
    **************************/
    integer = GetToday();
    integer = GetTodayHours();

    GetDate(0, "%Y/%m/%d %H:%M:%S", chars, 32);

    /**************************
                Chat
    **************************/
    SMUtils_SetChatPrefix("[\x04SM\x01]");
    SMUtils_SetChatSpaces("            ");
    SMUtils_SetChatConSnd(true);

    SMUtils_SkipNextChatCS();
    SMUtils_SkipNextPrefix();

    Chat(0, "SMUtils");
    ChatEx(integers, 0, "SMUtils: %s", SMUtils_Version);
    ChatTeam(0, false, "SMUtils: %s", SMUtils_Version);
    ChatAlive(true, true, "SMUtils: %s", SMUtils_Version);
    ChatAll("SMUtils: %s", SMUtils_Version);
    tChat(0, "SMUtils: %s", SMUtils_Version);
    tChatAlive(true, true, "SMUtils: %s", SMUtils_Version);
    tChatTeam(0, false, "SMUtils: %s", SMUtils_Version);
    tChatAll("SMUtils: %s", SMUtils_Version);
    ProcessColorString(chars, 32);
    RemoveAllColors(chars, 32);

    /**************************
                Hint
    **************************/
    Hint(0, "SMUtils: %s", SMUtils_Version);
    HintAll("SMUtils: %s", SMUtils_Version);
    HintEx(integers, 0, "SMUtils: %s", SMUtils_Version);
    tHintAll("SMUtils: %s", SMUtils_Version);

    /**************************
                TextMsg
    **************************/
    SMUtils_SetTextDest(HUD_PRINTCENTER);
    Text(0, "SMUtils: %s", SMUtils_Version);
    TextAll("SMUtils: %s", SMUtils_Version);
    TextEx(integers, 0, "SMUtils: %s", SMUtils_Version);
    tTextAll("SMUtils: %s", SMUtils_Version);

    /**************************
            MissionHint
    **************************/
    InstructorHud hint = new InstructorHud();
    hint.target = 0;
    hint.flags = 0;
    hint.activator_userid = 0;
    hint.positioning = false;
    hint.invisible = false;
    hint.forcecaption = false;
    hint.icon_onscreen = Icon_bulb;
    hint.icon_offscreen = Icon_bulb;
    hint.show_offscreen = false;
    hint.range = 0.0;
    hint.timeout = 0.0;
    hint.offset = 0.0;
    hint.pulseoption = 0;
    hint.alphaoption = 0;
    hint.shakeoption = 0;
    hint.color(0, 0, 0);
    hint.EasyInit();
    hint.Display(0, "SMUtils: %s", SMUtils_Version);
    hint.DisplayAll("SMUtils: %s", SMUtils_Version);
    hint.tDisplayAll("SMUtils: %s", SMUtils_Version);
    hint.Destroy();

    EasyMissionHint(0, 0.0, Icon_bulb, 255, 255, 255, "SMUtils: %s", SMUtils_Version);
    EasyMissionHintAll(0.0, Icon_bulb, 255, 255, 255, "SMUtils: %s", SMUtils_Version);

    /**************************
                Sound
    **************************/
    PrepareSound("kyle/tekken7/voice/kazumi.mp3");
    EmitSoundEx(integers, 0, "kyle/tekken7/voice/kazumi.mp3");
    EmitSoundOne(0, "kyle/tekken7/voice/kazumi.mp3");
    EmitSoundAll("kyle/tekken7/voice/kazumi.mp3");

    /**************************
                Weapon
    **************************/
    integer = GetWeaponClassname(0, -1, chars, 32);
    integer = GetPlayerWeaponEntity(0, "weapon_slash");
    boolean = RemoveWeaponByClassname(0, "weapon_slash");
    boolean = RemoveWeapon(0, 0);
    integer = GetClientActiveWeapon(0);

    SetWeaponClip(0, 0);
    SetWeaponAmmo(0, 0);
    SetWeaponAmmoEx(0, 0, 0);
    StripWeapon(0, true);
    StripWeaponAll();

    /**************************
                Overlay
    **************************/
    PrepareOverlay("kyle/overlay/tekken7/win");
    Overlay(0, "kyle/overlay/tekken7/win", 5.0);
    OverlayAll("kyle/overlay/tekken7/win", 5.0);

    /**************************
                Entity
    **************************/
    boolean = SetEntityParent(0, 0);
    boolean = SetEntityParentEx(0, 0, "facemask", float3);
    boolean = SelfKillEntity(0, 0.0);
    boolean = SelfKillEntityEx(0, 0.0);
    boolean = SetTextureIndex(0, 0, 0.0);
    integer = GetEntityHammerID(0);
    integer = GetEntityParentName(0);
    integer = GetEntityTargetName(0, chars, 32);

    RemoveEntityEx(0, 0.0);
    RemoveEdictEx(0, 0.0);

    /**************************
                Misc
    **************************/
    FreeHandle(handle);
    StopTimer(handle);

    integer = RandomInt();
    decimal = RandomFloat();

    RandomString(chars, 32);
    RandomStringEx("hwlifeauhjflk.wrygajkikr,ybajwfkewaotghawr", chars, 32);

    StringToLower("StringToLower", chars, 32);
    StringToUpper("StringToUpper", chars, 32);
    
    assert(decimal);
    assert(boolean);
    assert(integer);
}