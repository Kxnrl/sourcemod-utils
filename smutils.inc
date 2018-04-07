/******************************************************************/
/*                                                                */
/*                         Sourcemod UTILs                        */
/*                                                                */
/*                                                                */
/*  File:          smutils.inc                                    */
/*  Description:   Some helpful functions. -> go fuck sasusi.     */
/*                                                                */
/*                                                                */
/*  Copyright (C) 2018  Kyle   https://kxnrl.com                  */
/*  2018/04/04 10:17:14                                           */
/*                                                                */
/*  This code is licensed under the MIT License (MIT).            */
/*                                                                */
/******************************************************************/


#if defined __SourceMod_UTILs__
    #endinput
#endif

#define __SourceMod_UTILs__

#define SMUtils_Version 1.0
#define SMUtils_Author  "Kyle"
#define SMUtils_URL     "https://kxnrl.com/git/SMUtils"

#define CONSOLE 0


/**************************
            Global
**************************/
stock bool ClientIsKyle(int client)
{
    return (GetSteamAccountID(client) == 88166525);
}

stock int GetKyleIndex()
{
    for(int client = 1; client <= MaxClients; client++)
        if(ClientIsValid(client))
            if(GetSteamAccountID(client) == 88166525)
                return client;

    return -1;
}


/**************************
            Client
**************************/
stock bool ClientIsValid(int client, bool bot = false)
{
    if(client > MaxClients || client < CONSOLE)
		return false;

    if(!IsClientInGame(client))
        return false;

    return (bot || (!IsFakeClient(client) && !IsClientSourceTV(client)));
}

stock bool ClientIsAlive(int client, bool bot = false)
{
    return (ClientIsValid(client, bot) && IsPlayerAlive(client));
}

stock int FindClientBySteamId(AuthIdType authType, const char[] authId, bool validate = false)
{
    char steamId[32];
    for(int client = 1; client <= MaxClients; client++)
        if(ClientIsValid(client))
            if(GetClientAuthId(client, authType, steamId, 32, validate))
                if(strcmp(steamId, authId) == 0)
                    return client;

    return -1;
}


/**************************
            SteamId
**************************/
stock int ConvertSteam64ToSteam32(const char[] steamId64, char[] steamId32, int maxLen)
{
    char[] m_szBase = "76561197960265728";
    char m_szAuth[18], m_szAccount[18];
    int m_iBorrow, m_iTemp, m_iSteamId[2];

    strcopy(m_szAuth, 18, steamId64);

    if(CharToNumber(m_szAuth[16]) % 2 == 1)
    {
        m_iSteamId[0] = 1;
        m_szAuth[16] = NumberToChar(CharToNumber(m_szAuth[16]) - 1);
    }
    
    for(int k = 16; k >= 0; k--)
    {
        if(m_iBorrow > 0)
        {
            m_iTemp = CharToNumber(m_szAuth[k]) - 1;

            if(m_iTemp >= CharToNumber(m_szBase[k]))
            {
                m_iBorrow = 0;
                m_szAccount[k] = NumberToChar(m_iTemp - CharToNumber(m_szBase[k]));
            }
            else
            {
                m_iBorrow = 1;
                m_szAccount[k] = NumberToChar((m_iTemp + 10) - CharToNumber(m_szBase[k]));
            }
        }
        else
        {
            if(CharToNumber(m_szAuth[k]) >= CharToNumber(m_szBase[k]))
            {
                m_iBorrow = 0;
                m_szAccount[k] = NumberToChar(CharToNumber(m_szAuth[k]) - CharToNumber(m_szBase[k]));
            }
            else
            {
                m_iBorrow = 1;
                m_szAccount[k] = NumberToChar((CharToNumber(m_szAuth[k]) + 10) - CharToNumber(m_szBase[k]));
            }
        }
    }

    m_iSteamId[1] = StringToInt(m_szAccount);
    m_iSteamId[1] /= 2;

    return FormatEx(steamId32, maxLen, "STEAM_1:%d:%d", m_iSteamId[0], m_iSteamId[1]);
}

stock int NumberToChar(const int iNum)
{
    return '0' + ((iNum >= 0 && iNum <= 9) ? iNum : 0);
}

stock int CharToNumber(const int cNum)
{
    return (cNum >= '0' && cNum <= '9') ? (cNum - '0') : 0;
}


/**************************
            Chat
**************************/
static char ChatPrefix[32] = "[\x04SM\x01]";
static char ChatSpaces[32] = "   ";
static bool ChatConSnd     = true;
static bool StopNextChatCS = false;

stock int SMUtils_SetChatPrefix(const char[] prefix)
{
    return strcopy(ChatPrefix, 32, prefix);
}

stock int SMUtils_SetChatSpaces(const char[] spaces)
{
    return strcopy(ChatSpaces, 32, spaces);
}

// Set Chat string log in console and chat sound?
stock void SMUtils_SetChatConSnd(bool consnd)
{
    ChatConSnd = consnd;
}

// set next chat stop sound and log
stock void SMUtils_StopNextChatCS()
{
    StopNextChatCS = true;
}

stock void Chat(int client, const char[] buffer, any ...)
{
    Protobuf SayText2 = view_as<Protobuf>(StartMessageOne("SayText2", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(SayText2 == null)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "StartMessageOne -> SayText2 is null");
        return;
    }

    char msg[256];
    VFormat(msg, 256, buffer, 3);
    Format(msg, 256, "%s%s%s", ChatPrefix, ChatSpaces, msg);
    ProcessColorString(msg, 256);

    SayText2.SetInt("ent_idx", 0);
    SayText2.SetBool("chat", StopNextChatCS ? false : ChatConSnd);
    SayText2.SetString("msg_name", msg);
    SayText2.AddString("params", "");
    SayText2.AddString("params", "");
    SayText2.AddString("params", "");
    SayText2.AddString("params", "");
    EndMessage();
    
    StopNextChatCS = false;
}

stock void ChatAll(const char[] buffer, any ...)
{
    Protobuf SayText2 = view_as<Protobuf>(StartMessageAll("SayText2", USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(SayText2 == null)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "StartMessageAll -> SayText2 is null");
        return;
    }
    
    char msg[256];
    VFormat(msg, 256, buffer, 2);
    Format(msg, 256, "%s%s%s", ChatPrefix, ChatSpaces, msg);
    ProcessColorString(msg, 256);

    SayText2.SetInt("ent_idx", 0);
    SayText2.SetBool("chat", StopNextChatCS ? false : ChatConSnd);
    SayText2.SetString("msg_name", msg);
    SayText2.AddString("params", "");
    SayText2.AddString("params", "");
    SayText2.AddString("params", "");
    SayText2.AddString("params", "");
    EndMessage();

    StopNextChatCS = false;
}

stock void tChat(int client, const char[] buffer, any ...)
{
    Protobuf SayText2 = view_as<Protobuf>(StartMessageOne("SayText2", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(SayText2 == null)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "StartMessageOne -> SayText2 is null");
        return;
    }
    
    SetGlobalTransTarget(client);

    char msg[256];
    VFormat(msg, 256, buffer, 3);
    Format(msg, 256, "%s%s%s", ChatPrefix, ChatSpaces, msg);
    ProcessColorString(msg, 256);

    SayText2.SetInt("ent_idx", 0);
    SayText2.SetBool("chat", StopNextChatCS ? false : ChatConSnd);
    SayText2.SetString("msg_name", msg);
    SayText2.AddString("params", "");
    SayText2.AddString("params", "");
    SayText2.AddString("params", "");
    SayText2.AddString("params", "");
    EndMessage();
    
    StopNextChatCS = false;
    SetGlobalTransTarget(LANG_SERVER);
}

stock void tChatAll(const char[] buffer, any ...)
{
    char msg[256];
    for(int client = 1; client <= MaxClients; ++client)
        if(ClientIsValid(client))
        {
            SetGlobalTransTarget(client);

            VFormat(msg, 256, buffer, 2);
            Format(msg, 256, "%s%s%s", ChatPrefix, ChatSpaces, msg);

            ProcessColorString(msg, 256);

            Protobuf SayText2 = view_as<Protobuf>(StartMessageOne("SayText2", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
            if(SayText2 == null)
            {
                LogError("StartMessageOne -> SayText2 is null");
                continue;
            }

            SayText2.SetInt("ent_idx", 0);
            SayText2.SetBool("chat", StopNextChatCS ? false : ChatConSnd);
            SayText2.SetString("msg_name", msg);
            SayText2.AddString("params", "");
            SayText2.AddString("params", "");
            SayText2.AddString("params", "");
            SayText2.AddString("params", "");
            EndMessage();
        }

    StopNextChatCS = false;
    SetGlobalTransTarget(LANG_SERVER);
}

stock void ProcessColorString(char[] message, int maxLen)
{
    ReplaceString(message, maxLen, "{normal}",      "\x01", false);
    ReplaceString(message, maxLen, "{default}",     "\x01", false);
    ReplaceString(message, maxLen, "{white}",       "\x01", false);
    ReplaceString(message, maxLen, "{darkred}",     "\x02", false);
    ReplaceString(message, maxLen, "{pink}",        "\x03", false);
    ReplaceString(message, maxLen, "{green}",       "\x04", false);
    ReplaceString(message, maxLen, "{lime}",        "\x05", false);
    ReplaceString(message, maxLen, "{yellow}",      "\x05", false);
    ReplaceString(message, maxLen, "{lightgreen}",  "\x06", false);
    ReplaceString(message, maxLen, "{lightred}",    "\x07", false);
    ReplaceString(message, maxLen, "{red}",         "\x07", false);
    ReplaceString(message, maxLen, "{gray}",        "\x08", false);
    ReplaceString(message, maxLen, "{grey}",        "\x08", false);
    ReplaceString(message, maxLen, "{olive}",       "\x09", false);
    ReplaceString(message, maxLen, "{orange}",      "\x10", false);
    ReplaceString(message, maxLen, "{silver}",      "\x0A", false);
    ReplaceString(message, maxLen, "{lightblue}",   "\x0B", false);
    ReplaceString(message, maxLen, "{blue}",        "\x0C", false);
    ReplaceString(message, maxLen, "{purple}",      "\x0E", false);
    ReplaceString(message, maxLen, "{darkorange}",  "\x0F", false);
}


/**************************
            Hint
**************************/
stock void Hint(int client, const char[] buffer, any ...)
{
    Protobuf HintText = view_as<Protobuf>(StartMessageOne("HintText", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(HintText == null)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "StartMessageOne -> HintText is null");
        return;
    }

    SetGlobalTransTarget(client);

    char msg[256];
    VFormat(msg, 256, buffer, 3);

    HintText.SetString("text", msg);

    EndMessage();

    SetGlobalTransTarget(LANG_SERVER);
}

stock void HintAll(const char[] buffer, any ...)
{
    Protobuf HintText = view_as<Protobuf>(StartMessageAll("HintText", USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(HintText == null)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "StartMessageAll -> HintText is null");
        return;
    }
    
    char msg[256];
    VFormat(msg, 256, buffer, 2);
    HintText.SetString("text", msg);
    EndMessage();
}

stock void tHintAll(const char[] buffer, any ...)
{
    char msg[256];
    for(int client = 1; client <= MaxClients; ++client)
        if(ClientIsValid(client))
        {
            Protobuf HintText = view_as<Protobuf>(StartMessageOne("HintText", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
            if(HintText == null)
            {
                LogError("StartMessageOne -> HintText is null");
                continue;
            }
            
            SetGlobalTransTarget(client);

            VFormat(msg, 256, buffer, 2);

            HintText.SetString("text", msg);

            EndMessage();
        }

    SetGlobalTransTarget(LANG_SERVER);
}


/**************************
            TextMsg
**************************/
#define HUD_PRINTNOTIFY   1 
#define HUD_PRINTCONSOLE  2 
#define HUD_PRINTTALK     3 
#define HUD_PRINTCENTER   4 
static int TextMsgDest = HUD_PRINTCENTER;

stock void SMUtils_SetTextDest(int val)
{
    TextMsgDest = val;
}

stock void Text(int client, const char[] buffer, any ...)
{
    Protobuf TextMsg = view_as<Protobuf>(StartMessageOne("TextMsg", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(TextMsg == null)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "StartMessageOne -> TextMsg is null");
        return;
    }

    SetGlobalTransTarget(client);

    char msg[256];
    VFormat(msg, 256, buffer, 3);

    TextMsg.SetInt("msg_dst", TextMsgDest); 
    TextMsg.AddString("params", msg);
    TextMsg.AddString("params", "");
    TextMsg.AddString("params", "");
    TextMsg.AddString("params", "");
    TextMsg.AddString("params", "");

    EndMessage();

    SetGlobalTransTarget(LANG_SERVER);
}

stock void TextAll(const char[] buffer, any ...)
{
    Protobuf TextMsg = view_as<Protobuf>(StartMessageAll("TextMsg", USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(TextMsg == null)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "StartMessageAll -> TextMsg is null");
        return;
    }
    
    char msg[256];
    VFormat(msg, 256, buffer, 3);

    TextMsg.SetInt("msg_dst", TextMsgDest); 
    TextMsg.AddString("params", msg);
    TextMsg.AddString("params", "");
    TextMsg.AddString("params", "");
    TextMsg.AddString("params", "");
    TextMsg.AddString("params", "");

    EndMessage();
}

stock void tTextAll(const char[] buffer, any ...)
{
    char msg[256];
    for(int client = 1; client <= MaxClients; ++client)
        if(ClientIsValid(client))
        {
            Protobuf TextMsg = view_as<Protobuf>(StartMessageOne("TextMsg", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
            if(TextMsg == null)
            {
                LogError("StartMessageOne -> TextMsg is null");
                continue;
            }

            SetGlobalTransTarget(client);

            VFormat(msg, 256, buffer, 3);

            TextMsg.SetInt("msg_dst", TextMsgDest); 
            TextMsg.AddString("params", msg);
            TextMsg.AddString("params", "");
            TextMsg.AddString("params", "");
            TextMsg.AddString("params", "");
            TextMsg.AddString("params", "");

            EndMessage();
        }

    SetGlobalTransTarget(LANG_SERVER);
}


/**************************
        MissionHint
**************************/
#include <sdktools_functions>
#include <sdktools_entinput>

enum MissionHint_Icon
{
    Icon_bulb = 0,
    Icon_caution,
    Icon_alert,
    Icon_alert_red,
    Icon_tip,
    Icon_skull,
    Icon_no,
    Icon_run,
    Icon_interact,
    Icon_button,
    Icon_door,
    Icon_arrow_plain,
    Icon_arrow_plain_white_dn,
    Icon_arrow_plain_white_up,
    Icon_arrow_up,
    Icon_arrow_right,
    Icon_fire,
    Icon_present
};

static char MissionHint_Icon_String[MissionHint_Icon][32] =
{
    "icon_bulb",
    "icon_caution",
    "icon_alert",
    "icon_alert_red",
    "icon_tip",
    "icon_skull",
    "icon_no",
    "icon_run",
    "icon_interact",
    "icon_button",
    "icon_door",
    "icon_arrow_plain",
    "icon_arrow_plain_white_dn",
    "icon_arrow_plain_white_up",
    "icon_arrow_up",
    "icon_arrow_right",
    "icon_fire",
    "icon_present"
};

enum InstructorHud
{
    Invalid_Hud = -1
};

//https://developer.valvesoftware.com/wiki/Env_instructor_hint
methodmap InstructorHud __nullable__
{
    property int entity
    {
        public get(){ return view_as<int>(this); }
    }

    public InstructorHud(const char[] targetname)
    {
        int entity = CreateEntityByName ("env_instructor_hint");
        if(entity <= MaxClients)
            ThrowNativeError(SP_ERROR_ABORTED, "Create env_instructor_hint failed -> entity is invalid.");

        if(targetname[0])
            DispatchKeyValue(entity, "targetname", targetname);
        else
        {
            char buffer[32];
            FormatEx(buffer, 32, "env_instructor_hint_%d", entity);
            DispatchKeyValue(entity, "targetname", buffer);
        }
        
        DispatchSpawn(entity);

        return view_as<InstructorHud>(entity);
    }
    
    public bool EnforceValue(const char[] key, const char[] val)
    {
        return DispatchKeyValue(this.entity, key, val);
    }
    
    public bool EnforceFloat(const char[] key, const float val)
    {
        return DispatchKeyValueFloat(this.entity, key, val);
    }
    
    public void Destroy(float delay = 0.0)
    {
        RemoveEdictEx(this.entity, delay);
    }

    public void Display(int client, const char[] buffer, any ...)
    {
        char caption[256];
        VFormat(caption, 256, buffer, 3);
        this.EnforceValue("hint_caption", caption);
        AcceptEntityInput(this.entity, "ShowHint", client, this.entity);
    }
    
    public void DisplayAll(const char[] buffer, any ...)
    {
        char caption[256];
        VFormat(caption, 256, buffer, 2);
        this.EnforceValue("hint_caption", caption);
        AcceptEntityInput(this.entity, "ShowHint");
    }
    
    public void tDisplay(int client, const char[] buffer, any ...)
    {
        SetGlobalTransTarget(client);
        char caption[256];
        VFormat(caption, 256, buffer, 3);
        this.EnforceValue("hint_caption", caption);
        AcceptEntityInput(this.entity, "ShowHint", client, this.entity);
        SetGlobalTransTarget(LANG_SERVER);
    }
    
    public void tDisplayAll(const char[] buffer, any ...)
    {
        char caption[256];
        for(int client = 1; client <= MaxClients; ++client)
            if(ClientIsValid(client))
            {
                SetGlobalTransTarget(client);
                VFormat(caption, 256, buffer, 2);
                this.EnforceValue("hint_caption", caption);
                AcceptEntityInput(this.entity, "ShowHint", client, this.entity);
            }
        SetGlobalTransTarget(LANG_SERVER);
    }

    public bool TargetEntity(const char[] targetname)
    {
        return this.EnforceValue("hint_target", targetname);
    }
    
    public bool CaptionColor(int r, int g, int b)
    {
        char color[32];
        FormatEx(color, 32, "%d %d %d", r, g, b);
        return this.EnforceValue("hint_color", color);
    }
    
    public bool Positioning(bool positioning) // 0 = follow, 1 = hold on hud
    {
        return this.EnforceValue("hint_static", positioning ? "1" : "0");
    }

    public bool AllowInvisible(bool allow) // 0 = end immed on nodraw, 1 = yes
    {
        return this.EnforceValue("hint_allow_nodraw_target", allow ? "1" : "0");
    }
    
    public bool ForceCaption(bool show) // 0 = no, 1 = show when occluded
    {
        return this.EnforceValue("hint_forcecaption", show ? "1" : "0");
    }
    
    public bool OnscreenIcon(MissionHint_Icon icon)
    {
        return this.EnforceValue("hint_icon_onscreen", MissionHint_Icon_String[icon]);
    }
    
    public bool OffscreenIcon(MissionHint_Icon icon)
    {
        return this.EnforceValue("hint_icon_offscreen", MissionHint_Icon_String[icon]);
    }
    
    public bool ShowOffscreen(bool show)
    {
        return this.EnforceValue("hint_nooffscreen", show ? "0" : "1");
    }

    public bool DisplayRange(float range)
    {
        return this.EnforceFloat("hint_range", range);
    }
    
    public bool TimeOut(float time)
    {
        return this.EnforceFloat("hint_timeout", time);
    }
    
    public bool IconHeight(float height)
    {
        return this.EnforceFloat("hint_icon_offset", height);
    }
    
    public bool SizePulsing(int val)
    {
        char buffer[16];
        IntToString(val, buffer, 16);
        return this.EnforceValue("hint_pulseoption", buffer);
    }
    
    public bool AlphaPulsing(int val)
    {
        char buffer[16];
        IntToString(val, buffer, 16);
        return this.EnforceValue("hint_alphaoption", buffer);
    }
    
    public bool Shaking(int val) // 0 = no, 1 = Narrow, 2 = Wide
    {
        char buffer[16];
        IntToString(val, buffer, 16);
        return this.EnforceValue("hint_shakeoption", buffer);
    }

    public void EasyInit()
    {
        this.CaptionColor(57, 197, 187);
        this.Positioning(false);
        this.AllowInvisible(true);
        this.ForceCaption(true);
        this.ShowOffscreen(true);
        this.DisplayRange(1000.0);
        this.IconHeight(1.0);
        this.SizePulsing(0);
        this.AlphaPulsing(0);
        this.Shaking(0);
    }
}

stock void EasyMissionHint(int client, const char[] caption, float holdtime, MissionHint_Icon icon, int r, int g, int b)
{
    InstructorHud hint = new InstructorHud("");
    hint.EasyInit();
    hint.CaptionColor(r, g, b);
    hint.TimeOut(holdtime);
    hint.OnscreenIcon(icon);
    hint.Display(client, caption);
    hint.Destroy();
}

stock void EasyMissionHintAll(const char[] caption, float holdtime, MissionHint_Icon icon, int r, int g, int b)
{
    InstructorHud hint = new InstructorHud("");
    hint.EasyInit();
    hint.CaptionColor(r, g, b);
    hint.TimeOut(holdtime);
    hint.OnscreenIcon(icon);
    hint.DisplayAll(caption);
    hint.Destroy();
}


/**************************
            Sound
**************************/
stock void PrepareSound(const char[] sound)
{
    static int table_download = INVALID_STRING_TABLE;
    static int table_soundpre = INVALID_STRING_TABLE;

    if(table_download == INVALID_STRING_TABLE)
		table_download = FindStringTable("downloadables");

    if(table_soundpre == INVALID_STRING_TABLE)
		table_soundpre = FindStringTable("soundprecache");

    char buffer[256];
    bool prevls;  //Previous lock state.

    // precache
    FormatEx(buffer, 256, "*%s", sound);
    prevls = LockStringTables(false);
	AddToStringTable(table_soundpre, buffer);
	LockStringTables(prevls);
    
    // downloader
    FormatEx(buffer, 256, "sound/%s", sound);
    prevls = LockStringTables(false);
	AddToStringTable(table_download, buffer);
	LockStringTables(prevls);
}

stock void EmitSoundEx(const int[] clients, int numClients, const char[] sample, int entity = SOUND_FROM_PLAYER, int channel = SNDCHAN_AUTO, int level = SNDLEVEL_NORMAL, int flags = SND_NOFLAGS, float volume = SNDVOL_NORMAL, int pitch = SNDPITCH_NORMAL, int speakerentity = -1, const float origin[3] = NULL_VECTOR, const float dir[3] = NULL_VECTOR, bool updatePos = true, float soundtime = 0.0)
{
    char sound[256];
    FormatEx(sound, 256, "*%s", sample);
    EmitSoundEx(clients, numClients, sound, int entity, channel, level, flags, volume, pitch, speakerentity, origin, dir, updatePos, soundtime);
}

stock void EmitSoundOne(int client, const char[] sample, int entity = SOUND_FROM_PLAYER, int channel = SNDCHAN_AUTO, int level = SNDLEVEL_NORMAL, int flags = SND_NOFLAGS, float volume = SNDVOL_NORMAL, int pitch = SNDPITCH_NORMAL, int speakerentity = -1, const float origin[3] = NULL_VECTOR, const float dir[3] = NULL_VECTOR, bool updatePos = true, float soundtime = 0.0)
{
    char sound[256];
    FormatEx(sound, 256, "*%s", sample);
    EmitSoundToClient(client, sound, int entity, channel, level, flags, volume, pitch, speakerentity, origin, dir, updatePos, soundtime)
}

stock void EmitSoundAll(const char[] sample, int entity = SOUND_FROM_PLAYER, int channel = SNDCHAN_AUTO, int level = SNDLEVEL_NORMAL, int flags = SND_NOFLAGS, float volume = SNDVOL_NORMAL, int pitch = SNDPITCH_NORMAL, int speakerentity = -1, const float origin[3] = NULL_VECTOR, const float dir[3] = NULL_VECTOR, bool updatePos = true, float soundtime = 0.0)
{
    char sound[256];
    FormatEx(sound, 256, "*%s", sample);
    EmitSoundToAll(sound, int entity, channel, level, flags, volume, pitch, speakerentity, origin, dir, updatePos, soundtime)
}

/**************************
            Misc
**************************/
stock void FreeHandle(Handle &hndl)
{
    if(hndl != INVALID_HANDLE)
	{
		CloseHandle(hndl);
		hndl = INVALID_HANDLE;
	}
}

stock void StopTimer(Handle &timer)
{
    if(timer != INVALID_HANDLE)
	{
		KillTimer(timer);
		timer = INVALID_HANDLE;
	}
}

stock void RemoveEntityEx(int entity, float delay = 0.0) // 0.0 = 1 frame delay
{
    if(!IsValidEntity(entity))
        return;

    if(delay < 0.0)
    {
        AcceptEntityInput(entity, "kill");
        return;
    }

    CreateTimer(delay, Timer_DestroyEntity, EntIndexToEntRef(entity), TIMER_FLAG_NO_MAPCHANGE);
}

stock void RemoveEdictEx(int edict, float delay = 0.0)
{
    if(!IsValidEdict(edict))
        return;

    if(delay < 0.0)
    {
        RemoveEdict(edict);
        AcceptEntityInput(edict, "kill");
        return;
    }

    CreateTimer(delay, Timer_DestroyEdict, EntIndexToEntRef(edict), TIMER_FLAG_NO_MAPCHANGE);
}

public Action Timer_DestroyEntity(Handle timer, int entRef)
{
    int entity = EntRefToEntIndex(entRef);
    if(IsValidEntity(entity))
    {
        if(!AcceptEntityInput(entity, "Kill"))
            LogError("can not kill entity %d", entity);
    }

    return Plugin_Stop;
}

public Action Timer_DestroyEdict(Handle timer, int edictRef)
{
    int edict = EntRefToEntIndex(edictRef);
    if(IsValidEdict(edict))
    {
        RemoveEdict(edict);
        if(!AcceptEntityInput(edict, "Kill"))
            LogError("can not kill edict %d", edict);
    }

    return Plugin_Stop;
}