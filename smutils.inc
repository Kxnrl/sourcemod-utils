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

#define CONSOLE   0
#define ClientMin 1
#define ClientMax MaxClients+1

#include <sdktools>

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
    if(client >= ClientMax || client <= ClientMin)
        return false;

    if(!IsClientInGame(client))
        return false;
    
    if(IsClientSourceTV(client) || IsClientReplay(client))
        return false;

    return (bot || !IsFakeClient(client));
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

stock int GetRandomClient(bool alive = false, int ignoreFlags = 0)
{
    int[] clients = new int [MaxClients+1];
    int total;

    clients[total++] = -1;

    for (int client = 1; client <= MaxClients; client++)
        if(ClientIsValid(client))
            if(!alive || ClientIsAlive(client))
                if((ignoreFlags != 0 && HasClientUserFlags(client, ignoreFlags)) || ignoreFlags == 0) 
                    clients[total++] = client;
    
    return clients[RandomInt(0, total)];
}

stock bool HasClientUserFlags(int client, int flags)
{
    int clientFlags = GetUserFlagBits(client);
    return (clientFlags & ADMFLAG_ROOT || clientFlags & flags);
}

stock bool HasClientAdminFlags(int client, AdminFlag flag)
{
    char steamid[32];
    
    //try all steamId type
    for(int i = 0; i < view_as<int>(AuthIdType); ++i)
    {
        GetClientAuthId(client, view_as<AuthIdType>(i), steamid, 32);
        AdminId admin = FindAdminByIdentity(steamid, AUTHMETHOD_STEAM);
        if(admin != INVALID_ADMIN_ID)
            return (admin.HasFlag(flag, Access_Real) || admin.HasFlag(flag, Access_Effective));
    }
    
    return false;
}

stock void SetClientFrags(int client, int val)
{
    SetEntProp(client, Prop_Data, "m_iFrags", val);
}

stock void SetClientDeaths(int client, int val)
{
    SetEntProp(client, Prop_Data, "m_iDeaths", val);
}

stock void SetClientMoney(int client, int val)
{
    SetEntProp(client, Prop_Send, "m_iAccount", val);
}

stock void SetClientKevlar(int client, int val)
{
    SetEntProp(client, Prop_Send, "m_ArmorValue", val);
}

stock void SetClientHelmet(int client, bool has)
{
    SetEntProp(client, Prop_Send, "m_bHasHelmet", has);
}

stock void SetClientHud(int client, int val)
{
    SetEntProp(client, Prop_Send, "m_iHideHUD", val);
}

stock void SetClientWallHack(int client, float time) // time = GetGameTime() + val
{
    SetEntPropFloat(client, Prop_Send, "m_flDetectedByEnemySensorTime", 0.0);
}

stock int GetClientAimTargetPos(int client, float pos[3])
{
    float fAgl[3];
    float fOrg[3];

    GetClientEyePosition(client,fOrg);
    GetClientEyeAngles(client, fAgl);

    Handle trace = TR_TraceRayFilterEx(fOrg, fAgl, MASK_SHOT, RayType_Infinite, TraceFilterAllEntities, client);

    TR_GetEndPosition(pos, trace);

    int entity = TR_GetEntityIndex(trace);

    CloseHandle(trace);

    return entity;
}

public bool TraceFilterAllEntities(int entity, int contentsMask, int client)
{
	if(entity == client || entity > ClientMax || entity < ClientMin)

		return false;

	return true;
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
static bool StopNextPrefix = false;

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

// set next chat ignore prefix
stock void SMUtils_StopNextPrefix()
{
    StopNextPrefix = true;
}

stock void Chat(int client, const char[] buffer, any ...)
{
    Protobuf SayText2 = view_as<Protobuf>(StartMessageOne("SayText2", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(SayText2 == null)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "StartMessageOne -> SayText2 is null");
        return;
    }

    char msg[256];
    VFormat(msg, 256, buffer, 3);
    Format(msg, 256, "%s%s%s", StopNextPrefix ? ChatPrefix : "", ChatSpaces, msg);
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
    StopNextPrefix = false;
}

stock void ChatEx(const int[] clients, int numClients, const char[] buffer, any ...)
{
    Protobuf SayText2 = view_as<Protobuf>(StartMessageOne("SayText2", clients, numClients, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(SayText2 == null)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "StartMessageOne -> SayText2 is null");
        return;
    }
    
    char msg[256];
    VFormat(msg, 256, buffer, 4);
    Format(msg, 256, "%s%s%s", StopNextPrefix ? ChatPrefix : "", ChatSpaces, msg);
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
    StopNextPrefix = false;
}

stock void ChatAll(const char[] buffer, any ...)
{
    Protobuf SayText2 = view_as<Protobuf>(StartMessageAll("SayText2", USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(SayText2 == null)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "StartMessageAll -> SayText2 is null");
        return;
    }
    
    char msg[256];
    VFormat(msg, 256, buffer, 2);
    Format(msg, 256, "%s%s%s", StopNextPrefix ? ChatPrefix : "", ChatSpaces, msg);
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
    StopNextPrefix = false;
}

stock void tChat(int client, const char[] buffer, any ...)
{
    Protobuf SayText2 = view_as<Protobuf>(StartMessageOne("SayText2", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS));
    if(SayText2 == null)
    {
        ThrowNativeError(SP_ERROR_NATIVE, "StartMessageOne -> SayText2 is null");
        return;
    }
    
    SetGlobalTransTarget(client);

    char msg[256];
    VFormat(msg, 256, buffer, 3);
    Format(msg, 256, "%s%s%s", StopNextPrefix ? ChatPrefix : "", ChatSpaces, msg);
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
    StopNextPrefix = false;
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
            Format(msg, 256, "%s%s%s", StopNextPrefix ? ChatPrefix : "", ChatSpaces, msg);

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
    StopNextPrefix = false;
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
        ThrowNativeError(SP_ERROR_NATIVE, "StartMessageOne -> HintText is null");
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
        ThrowNativeError(SP_ERROR_NATIVE, "StartMessageAll -> HintText is null");
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
        ThrowNativeError(SP_ERROR_NATIVE, "StartMessageOne -> TextMsg is null");
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
        ThrowNativeError(SP_ERROR_NATIVE, "StartMessageAll -> TextMsg is null");
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
enum InstructorHud_Icon
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

static char InstructorHud_Icon_String[InstructorHud_Icon][32] =
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

//https://developer.valvesoftware.com/wiki/Env_instructor_hint
methodmap InstructorHud < Event
{
    property int target {
        public get () { return this.GetInt("hint_target", 0); }
        public set (int val) { this.SetInt("hint_target", val); }
    }
    
    property int flags {
        public get () { return this.GetInt("hint_flags", 0); }
        public set (int val) { this.SetInt("hint_flags", val); }
    }
    
    property int activator_userid {
        public get () { return this.GetInt("hint_activator_userid", 0); }
        public set (int val) { this.SetInt("hint_activator_userid", val); }
    }

    // true = follow, false = hold on hud
    property bool positioning {
        public get () { return this.GetBool("hint_static", true); }
        public set (bool val) { this.SetBool("hint_static", val); }
    }
    
    // true = end immed on nodraw, false = yes
    property bool invisible { 
        public get () { return this.GetBool("hint_allow_nodraw_target", true); }
        public set (bool val) { this.SetBool("hint_allow_nodraw_target", val); }
    }

    // true = no, false = show when occluded
    property bool forcecaption { 
        public get () { return this.GetBool("hint_forcecaption", false); }
        public set (bool val) { this.SetBool("hint_forcecaption", val); }
    }
    
    property InstructorHud_Icon icon_onscreen { 
        public get ()
        {
            char iconname[32];
            this.GetString("hint_icon_onscreen", iconname, 32);
            for(int i = 0; i < view_as<int>(InstructorHud_Icon); ++i)
                if(strcmp(iconname, InstructorHud_Icon_String[view_as<InstructorHud_Icon>(i)]) == 0)
                    return view_as<InstructorHud_Icon>(i);
            return Icon_bulb;
        }
        public set (InstructorHud_Icon icon) { this.SetString("hint_icon_onscreen", InstructorHud_Icon_String[icon]); }
    }
    
    property InstructorHud_Icon icon_offscreen { 
        public get ()
        {
            char iconname[32];
            this.GetString("hint_icon_offscreen", iconname, 32);
            for(int i = 0; i < view_as<int>(InstructorHud_Icon); ++i)
                if(strcmp(iconname, InstructorHud_Icon_String[view_as<InstructorHud_Icon>(i)]) == 0)
                    return view_as<InstructorHud_Icon>(i);
            return Icon_bulb;
        }
        public set (InstructorHud_Icon icon) { this.SetString("hint_icon_offscreen", InstructorHud_Icon_String[icon]); }
    }
    
    // true = don`t show, false = show;
    property bool show_offscreen {
        public get () { return this.GetBool("hint_nooffscreen", false); }
        public set (bool val) { this.SetBool("hint_nooffscreen", val); }
    }
    
    property float range {
        public get () { return this.GetFloat("hint_range", 1000.0); }
        public set (float val) { this.SetFloat("hint_nooffscreen", val); }
    }
    
    property float timeout {
        public get () { return this.GetFloat("hint_timeout", 1.0); }
        public set (float val) { this.SetFloat("hint_timeout", val); }
    }
    
    property float offset {
        public get () { return this.GetFloat("hint_icon_offset", 0.0); }
        public set (float val) { this.SetFloat("hint_icon_offset", val); }
    }
    
    property int pulseoption {
        public get () { return this.GetInt("hint_pulseoption", 0); }
        public set (int val) { this.SetInt("hint_pulseoption", val); }
    }
    
    property int alphaoption {
        public get () { return this.GetInt("hint_alphaoption", 0); }
        public set (int val) { this.SetInt("hint_alphaoption", val); }
    }
    
    // 0 = no, 1 = Narrow, 2 = Wide
    property int shakeoption {
        public get () { return this.GetInt("hint_shakeoption", 0); }
        public set (int val) { this.SetInt("hint_shakeoption", val); }
    }
    
    public void color(int r, int g, int b)
    {
        char clr[32];
        FormatEx(clr, 32, "%d %d %d", r, g, b);
        this.SetString("hint_color", clr);
    }

    public InstructorHud(const char[] name, int target = 0)
    {
        Event e = CreateEvent("instructor_server_hint_create", true);
        if(e == null)
            ThrowNativeError(SP_ERROR_NATIVE, "Create instructor_server_hint_create failed -> event is null.");

        e.SetBool("hint_local_player_only", true);

        e.SetInt("hint_flags", 0);
        e.SetInt("hint_activator_userid", 0);
        e.SetInt("hint_target", target);

        if(name[0] == 0)
        {
            e.SetString("hint_name", name);
            e.SetString("hint_replace_key", name);
        }
        else
        {
            char buffer[32];
            RandomString(buffer, 32);
            e.SetString("hint_name", buffer);
            e.SetString("hint_replace_key", buffer);
        }

        return view_as<InstructorHud>(e);
    }
    
    public void EasyInit()
    {
        this.color(57, 197, 187);
        this.positioning = false;
        this.invisible = true;
        this.forcecaption = false;
        this.show_offscreen = false;
        this.range = 1000.0;
        this.offset = 1.0;
        this.pulseoption = 0;
        this.alphaoption = 0;
        this.shakeoption = 0;
    }

    public void Destroy()
    {
        delete view_as<Event>(this);
    }

    public void Display(int client, const char[] buffer, any ...)
    {
        char caption[256];
        VFormat(caption, 256, buffer, 3);
        this.activator_userid = GetClientUserId(client);
        this.SetString("hint_caption", caption);
        this.SetString("hint_activator_caption", caption);
        this.FireToClient(client);
        this.activator_userid = 0;
    }

    public void DisplayAll(const char[] buffer, any ...)
    {
        char caption[256];
        VFormat(caption, 256, buffer, 2);
        this.SetString("hint_caption", caption);
        this.SetString("hint_activator_caption", caption);
        this.Fire();
    }

    public void tDisplay(int client, const char[] buffer, any ...)
    {
        SetGlobalTransTarget(client);
        char caption[256];
        VFormat(caption, 256, buffer, 3);
        this.activator_userid = GetClientUserId(client);
        this.SetString("hint_caption", caption);
        this.SetString("hint_activator_caption", caption);
        this.FireToClient(client);
        this.activator_userid = 0;
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
                this.activator_userid = GetClientUserId(client);
                this.SetString("hint_caption", caption);
                this.SetString("hint_activator_caption", caption);
                this.FireToClient(client);
                
            }
        this.activator_userid = 0;
        SetGlobalTransTarget(LANG_SERVER);
    }
}

stock void EasyMissionHint(int client, const char[] caption, float holdtime, InstructorHud_Icon icon, int r, int g, int b)
{
    InstructorHud hint = new InstructorHud("");
    hint.EasyInit();
    hint.color(r, g, b);
    hint.timeout = holdtime;
    hint.icon_onscreen = icon;
    hint.Display(client, caption);
    hint.Destroy();
}

stock void EasyMissionHintAll(const char[] caption, float holdtime, InstructorHud_Icon icon, int r, int g, int b)
{
    InstructorHud hint = new InstructorHud("");
    hint.EasyInit();
    hint.color(r, g, b);
    hint.timeout = holdtime;
    hint.icon_onscreen = icon;
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
    EmitSound(clients, numClients, sound, entity, channel, level, flags, volume, pitch, speakerentity, origin, dir, updatePos, soundtime);
}

stock void EmitSoundOne(int client, const char[] sample, int entity = SOUND_FROM_PLAYER, int channel = SNDCHAN_AUTO, int level = SNDLEVEL_NORMAL, int flags = SND_NOFLAGS, float volume = SNDVOL_NORMAL, int pitch = SNDPITCH_NORMAL, int speakerentity = -1, const float origin[3] = NULL_VECTOR, const float dir[3] = NULL_VECTOR, bool updatePos = true, float soundtime = 0.0)
{
    char sound[256];
    FormatEx(sound, 256, "*%s", sample);
    EmitSoundToClient(client, sound, entity, channel, level, flags, volume, pitch, speakerentity, origin, dir, updatePos, soundtime)
}

stock void EmitSoundAll(const char[] sample, int entity = SOUND_FROM_PLAYER, int channel = SNDCHAN_AUTO, int level = SNDLEVEL_NORMAL, int flags = SND_NOFLAGS, float volume = SNDVOL_NORMAL, int pitch = SNDPITCH_NORMAL, int speakerentity = -1, const float origin[3] = NULL_VECTOR, const float dir[3] = NULL_VECTOR, bool updatePos = true, float soundtime = 0.0)
{
    char sound[256];
    FormatEx(sound, 256, "*%s", sample);
    EmitSoundToAll(sound, entity, channel, level, flags, volume, pitch, speakerentity, origin, dir, updatePos, soundtime)
}


/**************************
            Weapon
**************************/
stock void GetWeaponClassname(int weapon, int index = -1, char[] classname, int maxLen)
{
    GetEdictClassname(weapon, classname, maxLen);

    if(index == -1)
        index = GetEntProp(weapon, Prop_Send, "m_iItemDefinitionIndex");

    switch(index)
    {
        case 60: strcopy(classname, maxLen, "weapon_m4a1_silencer");
        case 61: strcopy(classname, maxLen, "weapon_usp_silencer");
        case 63: strcopy(classname, maxLen, "weapon_cz75a");
        case 64: strcopy(classname, maxLen, "weapon_revolver");
    }
}

stock int GetPlayerWeaponEntity(int client, const char[] weapons)
{
    int m_hMyWeapons = FindSendPropInfo("CBasePlayer", "m_hMyWeapons");
    if(m_hMyWeapons == -1)
        ThrowNativeError(SP_ERROR_NATIVE, "CBasePlayer.m_hMyWeapons offset is not valid.");

    for(int offset = 0; offset < 128; offset += 4)
    {
        int weapon = GetEntDataEnt2(client, m_hMyWeapons+offset);

        if(IsValidEdict(weapon))
        {
            char classname[32];
            GetWeaponClassname(weapon, -1, classname, 32);

            if(strcmp(weapons, classname, false) == 0)
                return weapon;
        }
    }

    return -1;
}

stock int RemoveWeaponByClassname(int client, const char[] weapons)
{
    int weapon = GetPlayerWeaponEntity(client, weapons);
    return (weapon != -1) ? view_as<int>(RemoveWeapon(client, weapon)) : weapon;
}

stock bool RemoveWeapon(int client, int weapon)
{
    if(!RemovePlayerItem(client, weapon))
        return false;

    int m_hWeaponWorldModel = GetEntPropEnt(weapon, Prop_Send, "m_hWeaponWorldModel");
    if(IsValidEdict(m_hWeaponWorldModel))
        AcceptEntityInput(m_hWeaponWorldModel, "Kill");

    if(weapon == GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon"))
        SetEntPropEnt(client, Prop_Send, "m_hActiveWeapon", -1);

    AcceptEntityInput(weapon, "Kill");

    return true;
}

stock void SetWeaponClip(int weapon, int ammo)
{
    if(!HasEntProp(weapon, Prop_Send, "m_iClip1"))
        ThrowNativeError(SP_ERROR_PARAM, "Weapon %d has not m_iClip1 Props.", weapon);

    SetEntProp(weapon, Prop_Send, "m_iClip1", ammo, 4, 0);
}

stock void SetWeaponAmmo(int weapon, int ammo)
{
    if(!HasEntProp(weapon, Prop_Send, "m_iPrimaryReserveAmmoCount"))
        ThrowNativeError(SP_ERROR_PARAM, "Weapon %d has not m_iPrimaryReserveAmmoCount Props.", weapon);

    SetEntProp(weapon, Prop_Send, "m_iPrimaryReserveAmmoCount", ammo);
}

stock void SetWeaponAmmoEx(int client, int weapon, int ammo)
{
    int amtype = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType");
    if(amtype == -1)
        ThrowNativeError(SP_ERROR_PARAM, "Weapon %d -> m_iPrimaryAmmoType is -1.", weapon);
    
    if(!HasEntProp(weapon, Prop_Send, "m_iAmmo"))
        ThrowNativeError(SP_ERROR_PARAM, "Weapon %d has not m_iAmmo Props.", weapon);

    SetEntProp(client, Prop_Send, "m_iAmmo", ammo, _, amtype);
}

/**************************
            Misc
**************************/
stock void PrepareOverlay(const char[] overlay)
{
    static int table_download = INVALID_STRING_TABLE;
    if(table_download == INVALID_STRING_TABLE)
        table_download = FindStringTable("downloadables");
    
    bool prev;
    char path[256];
    
    FormatEx(path, 256, "%s.vmt", overlay);
    PrecacheDecal(path, true);
    
    FormatEx(path, 256, "materials/%s.vmt", overlay);
    prev = LockStringTables(false);
    AddToStringTable(table_download, path);
    LockStringTables(prev);

    FormatEx(path, 256, "%s.vtf", overlay);
    PrecacheDecal(path, true);

    FormatEx(path, 256, "materials/%s.vtf", overlay);
    prev = LockStringTables(false);
    AddToStringTable(table_download, path);
    LockStringTables(prev);
}

stock void Overlay(int client, const char[] overlay, float holdtime = 0.0)
{
    ClientCommand(client, "r_screenoverlay \"%s.vtf\"", overlay);

    if(holdtime >= 0.0)
        CreateTimer(holdtime, Timer_RemoveOverlay, GetClientUserId(client), TIMER_FLAG_NO_MAPCHANGE);
}

stock void OverlayAll(const char[] overlay, float holdtime = 0.0)
{
    for(int client = 1; client <= MaxClients; ++client)
        if(ClientIsValid(client))
            ClientCommand(client, "r_screenoverlay \"%s.vtf\"", overlay);

    if(holdtime >= 0.0)
        CreateTimer(holdtime, Timer_RemoveOverlay, -1, TIMER_FLAG_NO_MAPCHANGE);
}

public Action Timer_RemoveOverlay(Handle timer, int userid)
{
    if(userid == -1)
    {
        for(int client = 1; client <= MaxClients; ++client)
            if(ClientIsValid(client))
                ClientCommand(client, "r_screenoverlay \"\"");
            
        return Plugin_Stop;
    }
    
    int client = GetClientOfUserId(userid);

    if(ClientIsValid(client))
        ClientCommand(client, "r_screenoverlay \"\"");

    return Plugin_Stop;
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

stock int RandomInt(int min = 0, int max = 2147483647)
{
    int random = GetURandomInt();

    if(random == 0)
        random++;

    return RoundToCeil(float(random) / (float(2147483647) / float(max - min + 1))) + min - 1;
}

stock float RandomFloat(float min = 0.0, float max = 5201314.0)
{
    return (GetURandomFloat() * (max  - min)) + min;
}

stock void RandomString(char[] buffer, int maxLen)
{
    // terminator
    maxLen--;

    char random[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234556789";
    int randlen = strlen(random) - 1;

    int n = 0;
    int c = 0;

    while(n < maxLen)
    {
        if(random[0] == '\0')
        {
            c = GetRandomInt(33, 126);
            buffer[n] = c;
        }
        else
        {
            c = GetRandomInt(0, randlen);
            buffer[n] = random[c];
        }

        n++;
    }

    buffer[maxLen] = '\0';
}

stock void RandomStringEx(const char[] random, char[] buffer, int maxLen)
{
    maxLen--;
    
    int randlen = strlen(random) - 1;

    int n = 0;
    int c = 0;

    while(n < maxLen)
    {
        c = GetRandomInt(0, randlen);
        buffer[n++] = random[c];
    }

    buffer[maxLen] = '\0';
}

stock void StringToLower(const char[] input, char[] output, int size)
{
    size--;
    output[0] = '\0';

    int x = 0;
    while(input[x] != '\0' && x < size)
    {
        output[x] = CharToLower(input[x]);
        x++;
    }

    output[x] = '\0';
}

stock void StringToUpper(const char[] input, char[] output, int size)
{
    size--;
    output[0] = '\0';

    int x = 0;
    while(input[x] != '\0' && x < size)
    {
        output[x] = CharToUpper(input[x]);
        x++;
    }

    output[x] = '\0';
}