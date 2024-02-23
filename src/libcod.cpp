#include "gsc.hpp"
#include "libcod.hpp"

#include <signal.h>

// Stock cvars
cvar_t *cl_paused;
cvar_t *com_dedicated;
cvar_t *com_logfile;
cvar_t *com_sv_running;
cvar_t *sv_allowDownload;
cvar_t *sv_pure;
cvar_t *sv_serverid;

// Custom cvars
cvar_t *fs_callbacks;
cvar_t *g_debugCallbacks;
cvar_t *sv_downloadMessage;

cHook *hook_sys_loaddll;
cHook *hook_com_initdvars;
cHook *hook_gametype_scripts;
cHook *hook_g_localizedstringindex;
cHook *hook_sv_begindownload_f;

// Stock callbacks
int codecallback_startgametype = 0;
int codecallback_playerconnect = 0;
int codecallback_playerdisconnect = 0;
int codecallback_playerdamage = 0;
int codecallback_playerkilled = 0;

// Custom callbacks
int codecallback_playercommand = 0;

callback_t callbacks[] =
{
    { &codecallback_startgametype, "CodeCallback_StartGameType" }, // g_scr_data.gametype.startupgametype
    { &codecallback_playerconnect, "CodeCallback_PlayerConnect" }, // g_scr_data.gametype.playerconnect
    { &codecallback_playerdisconnect, "CodeCallback_PlayerDisconnect" }, // g_scr_data.gametype.playerdisconnect
    { &codecallback_playerdamage, "CodeCallback_PlayerDamage" }, // g_scr_data.gametype.playerdamage
    { &codecallback_playerkilled, "CodeCallback_PlayerKilled" }, // g_scr_data.gametype.playerkilled

    { &codecallback_playercommand, "CodeCallback_PlayerCommand"},
};

// Game lib variables
gentity_t *g_entities;

// Game lib functions
Scr_GetFunctionHandle_t Scr_GetFunctionHandle;
Scr_GetNumParam_t Scr_GetNumParam;
SV_Cmd_ArgvBuffer_t SV_Cmd_ArgvBuffer;
ClientCommand_t ClientCommand;
Scr_GetFunction_t Scr_GetFunction;
Scr_GetMethod_t Scr_GetMethod;
SV_GameSendServerCommand_t SV_GameSendServerCommand;
Scr_ExecEntThread_t Scr_ExecEntThread;
Scr_FreeThread_t Scr_FreeThread;
Scr_Error_t Scr_Error;
SV_GetConfigstringConst_t SV_GetConfigstringConst;
SV_GetConfigstring_t SV_GetConfigstring;
Scr_IsSystemActive_t Scr_IsSystemActive;
Scr_GetInt_t Scr_GetInt;
Scr_GetString_t Scr_GetString;
Scr_GetType_t Scr_GetType;
Scr_GetEntity_t Scr_GetEntity;
Scr_AddBool_t Scr_AddBool;
Scr_AddInt_t Scr_AddInt;
Scr_AddString_t Scr_AddString;
Scr_AddUndefined_t Scr_AddUndefined;
Scr_AddVector_t Scr_AddVector;
Scr_MakeArray_t Scr_MakeArray;
Scr_AddArray_t Scr_AddArray;
Scr_LoadScript_t Scr_LoadScript;
Q_strlwr_t Q_strlwr;
Q_strcat_t Q_strcat;

void custom_Com_InitCvars(void)
{
    hook_com_initdvars->unhook();
    void (*Com_InitDvars)(void);
    *(int *)&Com_InitDvars = hook_com_initdvars->from;
    Com_InitDvars();
    hook_com_initdvars->hook();

    // Get references to early loaded stock dvars
    cl_paused = Cvar_FindVar("cl_paused");
    com_dedicated = Cvar_FindVar("dedicated");
    com_logfile = Cvar_FindVar("logfile");
    com_sv_running = Cvar_FindVar("sv_running");
}

void common_init_complete_print(const char *format, ...)
{
    Com_Printf("--- Common Initialization Complete ---\n");

    // Get references to stock cvars
    sv_allowDownload = Cvar_FindVar("sv_allowDownload");
    sv_pure = Cvar_FindVar("sv_pure");
    sv_serverid = Cvar_FindVar("sv_serverid");

    // Register custom cvars
    Cvar_Get("libcod", "1", CVAR_SERVERINFO);
    Cvar_Get("sv_cracked", "0", CVAR_ARCHIVE);

    fs_callbacks = Cvar_Get("fs_callbacks", "", CVAR_ARCHIVE);
    g_debugCallbacks = Cvar_Get("g_debugCallbacks", "0", CVAR_ARCHIVE);
    sv_downloadMessage = Cvar_Get("sv_downloadMessage", "", CVAR_ARCHIVE);
}

int custom_GScr_LoadGameTypeScript()
{
    unsigned int i;
    char path_for_cb[512] = "maps/mp/gametypes/_callbacksetup";
    char path_for_cb_custom[512] = "callback"; //TODO: maybe don't use another hardcoded path for custom callbacks

    hook_gametype_scripts->unhook();
    int (*GScr_LoadGameTypeScript)();
    *(int *)&GScr_LoadGameTypeScript = hook_gametype_scripts->from;
    int ret = GScr_LoadGameTypeScript();
    hook_gametype_scripts->hook();

    if(!Scr_LoadScript(path_for_cb_custom))
        printf("####### custom_GScr_LoadGameTypeScript: Scr_LoadScript(path_for_cb_custom) FAILED \n");

    if ( strlen(fs_callbacks->string) )
        strncpy(path_for_cb, fs_callbacks->string, sizeof(path_for_cb));
        
    for ( i = 0; i < sizeof(callbacks)/sizeof(callbacks[0]); i++ )
    {
        if(!strcmp(callbacks[i].name, "CodeCallback_PlayerCommand")) // Custom callback
            *callbacks[i].pos = Scr_GetFunctionHandle(path_for_cb_custom, callbacks[i].name);
        else
            *callbacks[i].pos = Scr_GetFunctionHandle(path_for_cb, callbacks[i].name);
        
        /*if ( *callbacks[i].pos && g_debugCallbacks->integer )
            Com_Printf("%s found @ %p\n", callbacks[i].name, scrVarPub.programBuffer + *callbacks[i].pos);*/ //TODO: access scrVarPub
    }

    return ret;
}

static int localized_string_index = 128;
int custom_G_LocalizedStringIndex(const char *string)
{
    // See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/script.c#L944
    
    int i;
    char s[MAX_STRINGLENGTH];

    if(localized_string_index >= 256)
        localized_string_index = 128;

    if(!string || !*string)
        return 0;
    
    int start = 1244;

    for(i = 1; i < 256; i++)
    {
        SV_GetConfigstring(start + i, s, sizeof(s));
        if(!*s)
            break;
        if (!strcmp(s, string))
            return i;
    }
    if(i == 256)
        i = localized_string_index;

    SV_SetConfigstring(i + 1244, string);
    ++localized_string_index;
    
    return i;
}

void hook_ClientCommand(int clientNum)
{
    if ( !Scr_IsSystemActive() )
        return;

    #if COD_VERSION == COD1_1_1
    char* cmd = Cmd_Argv(0);
    if(!strcmp(cmd, "gc"))
        return; // Prevent server crash
    #elif COD_VERSION == COD1_1_5
    //TODO: verify no need to ignore gc command
    #endif
      
    if ( !codecallback_playercommand )
    {	ClientCommand(clientNum);
        return;
    }

    stackPushArray();
    int args = Cmd_Argc();
    for ( int i = 0; i < args; i++ )
    {
        char tmp[MAX_STRINGLENGTH];
        SV_Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
        if( i == 1 && tmp[0] >= 20 && tmp[0] <= 22 )
        {
            char *part = strtok(tmp + 1, " ");
            while( part != NULL )
            {
                stackPushString(part);
                stackPushArrayLast();
                part = strtok(NULL, " ");
            }
        }
        else
        {
            stackPushString(tmp);
            stackPushArrayLast();
        }
    }

    short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_playercommand, 1);
    Scr_FreeThread(ret);
}

const char* hook_AuthorizeState(int arg)
{
    const char* s = Cmd_Argv(arg);
    cvar_t* sv_cracked = Cvar_FindVar("sv_cracked");

    if (sv_cracked->integer && strcmp(s, "deny") == 0)
        return "accept";

    return s;
}

qboolean FS_svrPak(const char *base)
{
    if(strstr(base, "_svr_"))
        return qtrue;
    return qfalse;
}

const char* custom_FS_ReferencedPakNames(void)
{
    static char info[BIG_INFO_STRING];
    searchpath_t *search;

    info[0] = 0;
    
    for ( search = fs_searchpaths ; search ; search = search->next )
    {
        if (!search->pak)
            continue;

        if(FS_svrPak(search->pak->pakBasename))
        {
            search->pak->referenced = 0;
            continue;
        }

        if(*info)
            Q_strcat(info, sizeof( info ), " ");
        Q_strcat(info, sizeof( info ), search->pak->pakGamename);
        Q_strcat(info, sizeof( info ), "/");
        Q_strcat(info, sizeof( info ), search->pak->pakBasename);
    }

    return info;
}

const char* custom_FS_ReferencedPakChecksums(void)
{
    static char info[BIG_INFO_STRING];
    searchpath_t *search;
    
    info[0] = 0;

    for ( search = fs_searchpaths ; search ; search = search->next )
    {
        if (!search->pak)
            continue;
        
        if(FS_svrPak(search->pak->pakBasename))
        {
            search->pak->referenced = 0;
            continue;
        }
        
        Q_strcat( info, sizeof( info ), custom_va( "%i ", search->pak->checksum ) );
    }

    return info;
}

qboolean IsReferencedPak(const char *requestedFilePath)
{
    static char localFilePath[BIG_INFO_STRING];
    searchpath_t *search;

    localFilePath[0] = 0;

    for ( search = fs_searchpaths ; search ; search = search->next )
    {
        if(search->pak)
        {
            sprintf(localFilePath, "%s/%s.pk3", search->pak->pakGamename, search->pak->pakBasename);
            if (!strcmp(localFilePath, requestedFilePath))
                if (search->pak->referenced)
                    return qtrue;
        }
    }
    return qfalse;
}

void custom_SV_BeginDownload_f(client_t *cl)
{
    // Patch q3dirtrav
    int args = Cmd_Argc();
    if(args > 1)
    {
        const char* arg1 = Cmd_Argv(1);
        if(!IsReferencedPak(arg1))
            return;
    }

    hook_sv_begindownload_f->unhook();
    void (*SV_BeginDownload_f)(client_t *cl);
    *(int *)&SV_BeginDownload_f = hook_sv_begindownload_f->from;
    SV_BeginDownload_f(cl);
    hook_sv_begindownload_f->hook();
}

char *custom_va(const char *format, ...)
{
    // See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/shared.c#L632

    #define MAX_VA_STRING 32000
    va_list argptr;
    static char temp_buffer[MAX_VA_STRING];
    static char string[MAX_VA_STRING];
    static int index = 0;
    char *buf;
    int len;

    va_start( argptr, format );
    vsprintf( temp_buffer, format, argptr );
    va_end( argptr );

    if ( ( len = strlen( temp_buffer ) ) >= MAX_VA_STRING )
        Com_Error( ERR_DROP, "Attempted to overrun string in call to va() \n" );

    if ( len + index >= MAX_VA_STRING - 1 )
        index = 0;

    buf = &string[index];
    memcpy( buf, temp_buffer, len + 1 );

    index += len + 1;

    return buf;
}

void ServerCrash(int sig)
{
    int fd;
    FILE *fp;
    void *array[20];
    size_t size = backtrace(array, 20);

    // Write to crash log ...
    fp = fopen("./crash.log", "a");
    if ( fp )
    {
        fd = fileno(fp);
        fseek(fp, 0, SEEK_END);
        fprintf(fp, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
        fflush(fp);
        backtrace_symbols_fd(array, size, fd);
    }
    // ... and stderr
    fprintf(stderr, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

void *custom_Sys_LoadDll(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...))
{
    hook_sys_loaddll->unhook();
    void *(*Sys_LoadDll)(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...));
    *(int *)&Sys_LoadDll = hook_sys_loaddll->from;
    void *ret = Sys_LoadDll(name, fqpath, entryPoint, systemcalls);
    hook_sys_loaddll->hook();

    // Unprotect the game lib
    char libPath[512];
    cvar_t* fs_game = Cvar_FindVar("fs_game");
    if (*fs_game->string)
        sprintf(libPath, "%s/game.mp.i386.so", fs_game->string);
    else
        sprintf(libPath, "main/game.mp.i386.so");

    char buf[512];
    char flags[4];
    void *low, *high;
    FILE *fp;
    fp = fopen("/proc/self/maps", "r");
    if (!fp)
        return 0;
    while (fgets(buf, sizeof(buf), fp))
    {
        if (!strstr(buf, libPath))
            continue;
        if (sscanf (buf, "%p-%p %4c", &low, &high, flags) != 3)
            continue;
        mprotect((void *)low, (int)high-(int)low, PROT_READ | PROT_WRITE | PROT_EXEC);
    }
    fclose(fp);

    // Game lib variables
    g_entities = (gentity_t*)dlsym(ret, "g_entities");

    // Game lib functions
    Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)dlsym(ret, "Scr_GetFunctionHandle");
    Scr_GetNumParam = (Scr_GetNumParam_t)dlsym(ret, "Scr_GetNumParam");
    SV_Cmd_ArgvBuffer = (SV_Cmd_ArgvBuffer_t)dlsym(ret, "trap_Argv");
    ClientCommand = (ClientCommand_t)dlsym(ret, "ClientCommand");
    Scr_GetFunction = (Scr_GetFunction_t)dlsym(ret, "Scr_GetFunction");
    Scr_GetMethod = (Scr_GetMethod_t)dlsym(ret, "Scr_GetMethod");
    SV_GameSendServerCommand = (SV_GameSendServerCommand_t)dlsym(ret, "trap_SendServerCommand");
    Scr_ExecEntThread = (Scr_ExecEntThread_t)dlsym(ret, "Scr_ExecEntThread");
    Scr_FreeThread = (Scr_FreeThread_t)dlsym(ret, "Scr_FreeThread");
    Scr_Error = (Scr_Error_t)dlsym(ret, "Scr_Error");
    SV_GetConfigstringConst = (SV_GetConfigstringConst_t)dlsym(ret, "trap_GetConfigstringConst");
    SV_GetConfigstring = (SV_GetConfigstring_t)dlsym(ret, "trap_GetConfigstring");
    Scr_IsSystemActive = (Scr_IsSystemActive_t)dlsym(ret, "Scr_IsSystemActive");
    Scr_GetInt = (Scr_GetInt_t)dlsym(ret, "Scr_GetInt");
    Scr_GetString = (Scr_GetString_t)dlsym(ret, "Scr_GetString");
    Scr_GetType = (Scr_GetType_t)dlsym(ret, "Scr_GetType");
    Scr_GetEntity = (Scr_GetEntity_t)dlsym(ret, "Scr_GetEntity");
    Scr_AddBool = (Scr_AddBool_t)dlsym(ret, "Scr_AddBool");
    Scr_AddInt = (Scr_AddInt_t)dlsym(ret, "Scr_AddInt");
    Scr_AddString = (Scr_AddString_t)dlsym(ret, "Scr_AddString");
    Scr_AddUndefined = (Scr_AddUndefined_t)dlsym(ret, "Scr_AddUndefined");
    Scr_AddVector = (Scr_AddVector_t)dlsym(ret, "Scr_AddVector");
    Scr_MakeArray = (Scr_MakeArray_t)dlsym(ret, "Scr_MakeArray");
    Scr_AddArray = (Scr_AddArray_t)dlsym(ret, "Scr_AddArray");
    Scr_LoadScript = (Scr_LoadScript_t)dlsym(ret, "Scr_LoadScript");
    Q_strlwr = (Q_strlwr_t)dlsym(ret, "Q_strlwr");
    Q_strcat = (Q_strcat_t)dlsym(ret, "Q_strcat");

    // Game lib calls instructions redirections
    cracking_hook_call((int)dlsym(ret, "vmMain") + 0xB0, (int)hook_ClientCommand);

    hook_gametype_scripts = new cHook((int)dlsym(ret, "GScr_LoadGameTypeScript"), (int)custom_GScr_LoadGameTypeScript);
    hook_gametype_scripts->hook();
    
    // Game lib functions replacements
    cracking_hook_function((int)dlsym(ret, "G_LocalizedStringIndex"), (int)custom_G_LocalizedStringIndex);
    
    // Patch codmsgboom
    /* See:
    - https://aluigi.altervista.org/adv/codmsgboom-adv.txt
    - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/librarymodule.c#L146
    */
    cracking_write_hex((int)dlsym(ret, "G_Say") + 0x50e, (char *)"0x37f");
    cracking_write_hex((int)dlsym(ret, "G_Say") + 0x5ca, (char *)"0x37f");

    return ret;
}

class cCallOfDuty1Pro
{
public:
    cCallOfDuty1Pro()
    {
        // Don't inherit lib of parent
        unsetenv("LD_PRELOAD");

        // Crash handlers for debugging
        signal(SIGSEGV, ServerCrash);
        signal(SIGABRT, ServerCrash);
        
        // Otherwise the printf()'s are printed at crash/end on older os/compiler versions
        setbuf(stdout, NULL);

        #if COD_VERSION == COD1_1_1
        printf("> [LIBCOD] Compiled for: CoD1 1.1\n");
        #elif COD_VERSION == COD1_1_5
        printf("> [LIBCOD] Compiled for: CoD1 1.5\n");
        #endif

        printf("> [LIBCOD] Compiled %s %s using GCC %s\n", __DATE__, __TIME__, __VERSION__);

        // Allow to write in executable memory
        mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

        #if COD_VERSION == COD1_1_1
        cracking_hook_call(0x0806ce77, (int)common_init_complete_print);
        cracking_hook_call(0x08085213, (int)hook_AuthorizeState);
        cracking_hook_call(0x08094c54, (int)Scr_GetCustomFunction);
        cracking_hook_call(0x080951c4, (int)Scr_GetCustomMethod);

        hook_sys_loaddll = new cHook(0x080c5fe4, (int)custom_Sys_LoadDll);
        hook_sys_loaddll->hook();
        hook_com_initdvars = new cHook(0x080c6b90, (int)custom_Com_InitCvars);
        hook_com_initdvars->hook();
        hook_sv_begindownload_f = new cHook(0x08087a64, (int)custom_SV_BeginDownload_f);
        hook_sv_begindownload_f->hook();
        
        cracking_hook_function(0x080716cc, (int)custom_FS_ReferencedPakNames);
        cracking_hook_function(0x080717a4, (int)custom_FS_ReferencedPakChecksums);

        // Patch q3infoboom
        /* See:
        - https://aluigi.altervista.org/adv/q3infoboom-adv.txt
        - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/codextended.c#L295
        */
        cracking_write_hex(0x807f459, (char *)"1");

        #elif COD_VERSION == COD1_1_5
        #endif

        printf("> [PLUGIN LOADED]\n");
    }

    ~cCallOfDuty1Pro()
    {
        printf("> [PLUGIN UNLOADED]\n");
    }
};

cCallOfDuty1Pro *pro;
void __attribute__ ((constructor)) lib_load(void)
{
    pro = new cCallOfDuty1Pro;
}
void __attribute__ ((destructor)) lib_unload(void)
{
    delete pro;
}