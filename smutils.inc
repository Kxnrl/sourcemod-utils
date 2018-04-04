/******************************************************************/
/*                                                                */
/*                         Sourcemod UTILs                        */
/*                                                                */
/*                                                                */
/*  File:          Kstock.inc                                     */
/*  Description:   Some helpful functions.                        */
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

stock bool GetKyleIndex()
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

stock bool FindClientBySteamId(AuthIdType authType, const char[] authId, bool validate = false)
{
    char steamId[32];
    for(int client = 1; client <= MaxClients; client++)
        if(ClientIsValid(client))
            if(GetClientAuthId(client, authType, steamid, 32, validate))
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
static bool UserM

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

            Protobuf SayText2 = view_as<Protobuf>(StartMessageOne(StartMessageOne("SayText2", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
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
            Protobuf HintText = view_as<Protobuf>(StartMessageOne(StartMessageOne("HintText", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
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
static int TextDest = HUD_PRINTCENTER;

stock void SMUtils_SetTextDest(int val)
{
    TextDest = val;
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

    TextMsg.PbSetInt("msg_dst", TextMsgDest); 
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

    TextMsg.PbSetInt("msg_dst", TextMsgDest); 
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
            Protobuf TextMsg = view_as<Protobuf>(StartMessageOne(StartMessageOne("TextMsg", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
            if(TextMsg == null)
            {
                LogError("StartMessageOne -> TextMsg is null");
                continue;
            }

            SetGlobalTransTarget(client);

            VFormat(msg, 256, buffer, 3);

            TextMsg.PbSetInt("msg_dst", TextMsgDest); 
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
enum MissionHint_Icon
{
    Icon_tip = 0,
    Icon_info,
    Icon_shield,
    Icon_alert,
    Icon_alert_red,
    Icon_tip_again,
    Icon_skull,
    Icon_no,
    Icon_icon_interact,
    Icon_button,
    Icon_door,
    Icon_arrow_plain,
    Icon_arrow_plain_white_dn,
    Icon_arrow_plain_white_up,
    Icon_arrow_up,
    Icon_arrow_right
};

static char MissionHint_Icon_String[MissionHint_Icon][32] =
{
    "icon_tip",
    "icon_info",
    "icon_shield",
    "icon_alert",
    "icon_alert_red",
    "icon_tip (again)",
    "icon_skull",
    "icon_no",
    "icon_interact",
    "icon_button",
    "icon_door",
    "icon_arrow_plain",
    "icon_arrow_plain_white_dn",
    "icon_arrow_plain_white_up",
    "icon_arrow_up",
    "icon_arrow_right"
};

//https://developer.valvesoftware.com/wiki/Env_instructor_hint

static char  MissionHint_Targetname[128]  = "mission_hint_entity";
static char  MissionHint_TargetEntity[64] = "";
static bool  MissionHint_Positioning      = false;
static bool  MissionHint_AllowInvisible   = true;
static int   MissionHint_Color[3]         = {0, 255, 0};
static bool  MissionHint_ForceCaption     = true;
static bool  MissionHint_OnScreenIcon     = true;
static bool  MissionHint_OffScreenIcon    = true;
static bool  MissionHint_ShowOffScreen    = false;
static float MissionHint_IconHeightOffset = 5.0;
static int   MissionHint_SizePulsing      = 0;
static int   MissionHint_AlphaPulsing     = 0;
static int   MissionHint_Shaking          = 0;
static float MissionHint_DisplayRange     = 9999.99;
static bool  MissionHint_ShowOnFirstSight = true;
static int   MissionHint_DisplayLimit     = 0;
static bool  MissionHint_MovingSuppress   = false;

Cdaption, timeout
stock int SMUtils_SetMissionHint_Targetname(const char[] buffer, any ...)
{
    return VFormat(MissionHint_Targetname, 128, buffer, 2);
}

stock int SMUtils_SetMissionHint_TargetEntity(const char[] buffer, any ...)
{
    return VFormat(MissionHint_TargetEntity, 64, buffer, 2);
}

stock void SMUtils_SetMissionHint_Positioning(bool val)
{
    MissionHint_Positioning = val;
}

stock void SMUtils_SetMissionHint_AllowInvisible(bool val)
{
    MissionHint_AllowInvisible = val;
}

stock void SMUtils_SetMissionHint_Color(int r, int g, int b)
{
    MissionHint_Color[0] = r;
    MissionHint_Color[1] = g;
    MissionHint_Color[2] = b;
}

stock void SMUtils_SetMissionHint_ForceCaption(bool val)
{
    MissionHint_ForceCaption = val;
}

stock void SMUtils_SetMissionHint_OnScreenIcon(bool val)
{
    MissionHint_OnScreenIcon = val;
}

stock void SMUtils_SetMissionHint_OffScreenIcon(bool val)
{
    MissionHint_OffScreenIcon = val;
}
stock void SMUtils_SetMissionHint_ShowOffScreen(bool val)
{
    MissionHint_ShowOffScreen = val;
}

stock void SMUtils_SetMissionHint_IconHeightOffset(float dist)
{
    MissionHint_IconHeightOffset = dist;
}

stock void SMUtils_SetMissionHint_SizePulsing(int val)
{
    MissionHint_SizePulsing = val;
}

stock void SMUtils_SetMissionHint_AlphaPulsing(int val)
{
    MissionHint_AlphaPulsing = val;
}

stock void SMUtils_SetMissionHint_Shaking(int val)
{
    MissionHint_Shaking = val;
}

stock void SMUtils_SetMissionHint_DisplayRange(float val)
{
    MissionHint_DisplayRange = val;
}

stock void SMUtils_SetMissionHint_ShowOnFirstSight(bool val)
{
    MissionHint_ShowOnFirstSight = val;
}

stock void SMUtils_SetMissionHint_DisplayLimit(int val)
{
    MissionHint_DisplayLimit = val;
}

stock void SMUtils_SetMissionHint_MovingSuppress(bool val)
{
    MissionHint_MovingSuppress = val;
}

stock void DisplayMissionHintAll(const char[] caption, float timeout)
{
    int entity = CreateEntityByName("env_instructor_hint");
    if(entity == -1)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "MissionHint_Immed -> env_instructor_hint is not valid entity.");
        return;
    }
    
    
}

stock void MissionHint_Immed(const char[] caption, const char[] color, const char[] icon)
{
    int entity = CreateEntityByName("env_instructor_hint");
    if(entity == -1)
    {
        ThrowNativeError(SP_ERROR_ABORTED, "MissionHint_Immed -> env_instructor_hint is not valid entity.");
        return;
    }

    DispatchKeyValue(entity, "hint_range", "999999");
    DispatchKeyValue(entity, "hint_color", color);
    DispatchKeyValue(entity, "hint_caption", caption);
    DispatchKeyValue(entity, "hint_icon_onscreen", icon);
    DispatchKeyValue(entity, "hint_timeout", "5.0");

    DispatchSpawn(entity);

    AcceptEntityInput(entity, "ShowHint");

    AcceptEntityInput(entity, "kill");
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

