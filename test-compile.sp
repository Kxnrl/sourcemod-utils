#include "smutils.inc"

public void OnPluginStart()
{
    char buffer[32];
    Handle hndl;
    float pos[3];
    
    // Global
    ClientIsKyle(0);
    GetKyleIndex();
    
    // Client
    ClientIsValid(0);
    ClientIsAlive(0);
    FindClientBySteamId(AuthId_Steam2, "steamid");
    SetClientFrags(0, 0);
    SetClientDeaths(0, 0);
    SetClientMoney(0, 0);
    SetClientKevlar(0, 0);
    SetClientHelmet(0, false);
    SetClientHud(0, 0);
    SetClientWallHack(0, 0.0);
    GetClientAimTargetPos(0, pos);
    GetRandomClient(true);
    HasClientUserFlags(0, ADMFLAG_ROOT);
    HasClientAdminFlags(0, Admin_Root);
    
    
    // SteamId
    ConvertSteam64ToSteam32("sasusi nm$l", buffer, 32);

    // Chat
    SMUtils_SetChatPrefix("nm$l");
    SMUtils_SetChatSpaces("    ");
    SMUtils_SetChatConSnd(true);
    SMUtils_StopNextChatCS();
    Chat(0, "sasusi nm$l -> %d", 19990420);
    ChatAll("sasusi nm$l -> %d", 19990420);
    tChat(0, "sasusi nm$l -> %d", 19990420);
    tChatAll("sasusi nm$l -> %d", 19990420);
    
    // Hint
    Hint(0, "sasusi nm$l -> %d", 19990420);
    HintAll("sasusi nm$l -> %d", 19990420);
    tHintAll("sasusi nm$l -> %d", 19990420);
    
    // TextMsg
    SMUtils_SetTextDest(HUD_PRINTCENTER);
    Text(0, "sasusi nm$l -> %d", 19990420);
    TextAll("sasusi nm$l -> %d", 19990420);
    tTextAll("sasusi nm$l -> %d", 19990420);
    
    // Instructor Hud
    InstructorHud hud = new InstructorHud("sasusi nm$l, fuck you all");
    hud.EasyInit();
    hud.CaptionColor(59, 197, 187);
    hud.Positioning(false);
    hud.AllowInvisible(true);
    hud.ForceCaption(true);
    hud.OnscreenIcon(Icon_skull);
    hud.OffscreenIcon(Icon_run);
    hud.ShowOffscreen(true);
    hud.DisplayRange(9999.99);
    hud.TimeOut(5.0);
    hud.IconHeight(0.5);
    hud.SizePulsing(0);
    hud.AlphaPulsing(0);
    hud.Shaking(0);
    hud.Display(0, "sasusi nm$l, go fuck urself -> %d", 19990420);
    hud.DisplayAll("sasusi nm$l, go fuck urself -> %d", 19990420);
    hud.tDisplay(0, "sasusi nm$l, go fuck urself -> %d", 19990420);
    hud.tDisplayAll("sasusi nm$l, go fuck urself -> %d", 19990420);
    hud.Destroy();

    EasyMissionHint(0, "sasusi nm$l, fuck you all", 10.0, Icon_skull, 255, 0, 0);
    EasyMissionHintAll("sasusi nm$l, fuck you all", 10.0, Icon_skull, 255, 0, 0);
    
    // Sound
    PrepareSound("sanshengshi/nimasile.mp3");
    int[] clients = new int [MaxClients];
    EmitSoundEx(clients, MaxClients, "sanshengshi/nimasile.mp3");
    EmitSoundOne(0, "sanshengshi/nimasile.mp3");
    EmitSoundAll("sanshengshi/nimasile.mp3");
    
    // Weapon
    GetWeaponClassname(0, -1, buffer, 32);
    GetPlayerWeaponEntity(0, "sasusi_nm$l");
    RemoveWeaponByClassname(0, "sasusi_nm$l");
    RemoveWeapon(0, 1);
    SetWeaponClip(0, 1);
    SetWeaponAmmo(0, 1);
    SetWeaponAmmoEx(1, 2, 3);
    
    // Overlay
    PrepareOverlay("sasusi/nmsl");
    Overlay(0, "sasusi/nmsl");
    OverlayAll("sasusi/nmsl", 5.0);

    // Misc
    FreeHandle(hndl);
    StopTimer(hndl);
    RemoveEntityEx(0);
    RemoveEdictEx(0);
    RandomInt(1);
    RandomFloat();
    RandomString(buffer, 32);
    RandomStringEx("sasusi_nm$l", buffer, 32);
}