#include "smutils.inc"

public void OnPluginStart()
{
    char buffer[32];
    Handle hndl;
    
    // Global
    ClientIsKyle(0);
    GetKyleIndex();
    
    // Client
    ClientIsValid(0);
    ClientIsAlive(0);
    FindClientBySteamId(AuthId_Steam2, "steamid");
    
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
    hud.TargetEntity("die die die");
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
    hud.Destroy(10.0);
    
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

    // Misc
    FreeHandle(hndl);
    StopTimer(hndl);
    RemoveEntityEx(0);
    RemoveEdictEx(0);
}