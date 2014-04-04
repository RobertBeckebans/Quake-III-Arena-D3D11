#pragma pack(push, 1)
    case UI_ERROR:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Com_Error( ERR_DROP, "%s", VM_ArgPtr( p->a1 ) );
        return 0;
    }

    case UI_PRINT:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Com_Printf( "%s", VM_ArgPtr( p->a1 ) );
        return 0;
    }

    case UI_MILLISECONDS:
    {
        return Sys_Milliseconds();
    }

    case UI_CVAR_REGISTER:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
            size_t a3;
            int a4;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cvar_Register( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ), VM_ArgPtr( p->a3 ), p->a4 ); 
        return 0;
    }

    case UI_CVAR_UPDATE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cvar_Update( VM_ArgPtr( p->a1 ) );
        return 0;
    }

    case UI_CVAR_SET:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cvar_Set( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ) );
        return 0;
    }

    case UI_CVAR_VARIABLEVALUE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FloatAsInt( Cvar_VariableValue( VM_ArgPtr( p->a1 ) ) );
    }

    case UI_CVAR_VARIABLESTRINGBUFFER:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cvar_VariableStringBuffer( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ), p->a3 );
        return 0;
    }

    case UI_CVAR_SETVALUE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            float a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cvar_SetValue( VM_ArgPtr( p->a1 ), p->a2 );
        return 0;
    }

    case UI_CVAR_RESET:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cvar_Reset( VM_ArgPtr( p->a1 ) );
        return 0;
    }

    case UI_CVAR_CREATE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cvar_Get( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ), p->a3 );
        return 0;
    }

    case UI_CVAR_INFOSTRINGBUFFER:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cvar_InfoStringBuffer( p->a1, VM_ArgPtr( p->a2 ), p->a3 );
        return 0;
    }

    case UI_ARGC:
    {
        return Cmd_Argc();
    }

    case UI_ARGV:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cmd_ArgvBuffer( p->a1, VM_ArgPtr( p->a2 ), p->a3 );
        return 0;
    }

    case UI_CMD_EXECUTETEXT:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Cbuf_ExecuteText( p->a1, VM_ArgPtr( p->a2 ) );
        return 0;
    }

    case UI_FS_FOPENFILE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FS_FOpenFileByMode( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ), p->a3 );
    }

    case UI_FS_READ:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            int a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        FS_Read2( VM_ArgPtr( p->a1 ), p->a2, p->a3 );
        return 0;
    }

    case UI_FS_WRITE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            int a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        FS_Write( VM_ArgPtr( p->a1 ), p->a2, p->a3 );
        return 0;
    }

    case UI_FS_FCLOSEFILE:
    {
        struct __tmp {
            size_t a0;
            int a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        FS_FCloseFile( p->a1 );
        return 0;
    }

    case UI_FS_GETFILELIST:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
            size_t a3;
            int a4;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FS_GetFileList( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ), VM_ArgPtr( p->a3 ), p->a4 );
    }

    case UI_FS_SEEK:
    {
        struct __tmp {
            size_t a0;
            int a1;
            int a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FS_Seek( p->a1, p->a2, p->a3 );
    }
    
    case UI_R_REGISTERMODEL:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return re.RegisterModel( VM_ArgPtr( p->a1 ) );
    }

    case UI_R_REGISTERSKIN:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return re.RegisterSkin( VM_ArgPtr( p->a1 ) );
    }

    case UI_R_REGISTERSHADERNOMIP:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return re.RegisterShaderNoMip( VM_ArgPtr( p->a1 ) );
    }

    case UI_R_CLEARSCENE:
    {
        re.ClearScene();
        return 0;
    }

    case UI_R_ADDREFENTITYTOSCENE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.AddRefEntityToScene( VM_ArgPtr( p->a1 ) );
        return 0;
    }

    case UI_R_ADDPOLYTOSCENE:
    {
        struct __tmp {
            size_t a0;
            int a1;
            int a2;
            size_t a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.AddPolyToScene( p->a1, p->a2, VM_ArgPtr( p->a3 ), 1 );
        return 0;
    }

    case UI_R_ADDLIGHTTOSCENE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            float a2;
            float a3;
            float a4;
            float a5;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.AddLightToScene( VM_ArgPtr( p->a1 ), p->a2, p->a3, p->a4, p->a5 );
        return 0;
    }

    case UI_R_RENDERSCENE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.RenderScene( VM_ArgPtr( p->a1 ) );
        return 0;
    }

    case UI_R_SETCOLOR:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.SetColor( VM_ArgPtr( p->a1 ) );
        return 0;
    }

    case UI_R_DRAWSTRETCHPIC:
    {
        struct __tmp {
            size_t a0;
            float a1;
            float a2;
            float a3;
            float a4;
            float a5;
            float a6;
            float a7;
            float a8;
            int a9;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.DrawStretchPic( p->a1, p->a2, p->a3, p->a4, p->a5, p->a6, p->a7, p->a8, p->a9 );
        return 0;
    }

  case UI_R_MODELBOUNDS:
  {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            size_t a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.ModelBounds( p->a1, VM_ArgPtr( p->a2 ), VM_ArgPtr( p->a3 ) );
        return 0;
    }

    case UI_UPDATESCREEN:
    {
        SCR_UpdateScreen();
        return 0;
    }

    case UI_CM_LERPTAG:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            int a2;
            int a3;
            int a4;
            float a5;
            size_t a6;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.LerpTag( VM_ArgPtr( p->a1 ), p->a2, p->a3, p->a4, p->a5, VM_ArgPtr( p->a6 ) );
        return 0;
    }

    case UI_S_REGISTERSOUND:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            int a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return S_RegisterSound( VM_ArgPtr( p->a1 ), p->a2 );
    }

    case UI_S_STARTLOCALSOUND:
    {
        struct __tmp {
            size_t a0;
            int a1;
            int a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        S_StartLocalSound( p->a1, p->a2 );
        return 0;
    }

    case UI_KEY_KEYNUMTOSTRINGBUF:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Key_KeynumToStringBuf( p->a1, VM_ArgPtr( p->a2 ), p->a3 );
        return 0;
    }

    case UI_KEY_GETBINDINGBUF:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Key_GetBindingBuf( p->a1, VM_ArgPtr( p->a2 ), p->a3 );
        return 0;
    }

    case UI_KEY_SETBINDING:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Key_SetBinding( p->a1, VM_ArgPtr( p->a2 ) );
        return 0;
    }

    case UI_KEY_ISDOWN:
    {
        struct __tmp {
            size_t a0;
            int a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return Key_IsDown( p->a1 );
    }

    case UI_KEY_GETOVERSTRIKEMODE:
    {
        return Key_GetOverstrikeMode();
    }

    case UI_KEY_SETOVERSTRIKEMODE:
    {
        struct __tmp {
            size_t a0;
            int a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Key_SetOverstrikeMode( p->a1 );
        return 0;
    }

    case UI_KEY_CLEARSTATES:
    {
        Key_ClearStates();
        return 0;
    }

    case UI_KEY_GETCATCHER:
    {
        return Key_GetCatcher();
    }

    case UI_KEY_SETCATCHER:
    {
        struct __tmp {
            size_t a0;
            int a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Key_SetCatcher( p->a1 );
        return 0;
    }

    case UI_GETCLIPBOARDDATA:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            int a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        GetClipboardData( VM_ArgPtr( p->a1 ), p->a2 );
        return 0;
    }

    case UI_GETCLIENTSTATE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        GetClientState( VM_ArgPtr( p->a1 ) );
        return 0;        
    }

    case UI_GETGLCONFIG:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        CL_GetGlconfig( VM_ArgPtr( p->a1 ) );
        return 0;
    }

    case UI_GETCONFIGSTRING:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return GetConfigString( p->a1, VM_ArgPtr( p->a2 ), p->a3 );
    }

    case UI_LAN_LOADCACHEDSERVERS:
    {
        LAN_LoadCachedServers();
        return 0;
    }

    case UI_LAN_SAVECACHEDSERVERS:
    {
        LAN_SaveServersToCache();
        return 0;
    }

    case UI_LAN_ADDSERVER:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            size_t a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return LAN_AddServer(p->a1, VM_ArgPtr( p->a2 ), VM_ArgPtr( p->a3 ));
    }

    case UI_LAN_REMOVESERVER:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        LAN_RemoveServer(p->a1, VM_ArgPtr( p->a2 ));
        return 0;
    }

    case UI_LAN_GETPINGQUEUECOUNT:
    {
        return LAN_GetPingQueueCount();
    }

    case UI_LAN_CLEARPING:
    {
        struct __tmp {
            size_t a0;
            int a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        LAN_ClearPing( p->a1 );
        return 0;
    }

    case UI_LAN_GETPING:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            int a3;
            size_t a4;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        LAN_GetPing( p->a1, VM_ArgPtr( p->a2 ), p->a3, VM_ArgPtr( p->a4 ) );
        return 0;
    }

    case UI_LAN_GETPINGINFO:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        LAN_GetPingInfo( p->a1, VM_ArgPtr( p->a2 ), p->a3 );
        return 0;
    }

    case UI_LAN_GETSERVERCOUNT:
    {
        struct __tmp {
            size_t a0;
            int a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return LAN_GetServerCount(p->a1);
    }

    case UI_LAN_GETSERVERADDRESSSTRING:
    {
        struct __tmp {
            size_t a0;
            int a1;
            int a2;
            size_t a3;
            int a4;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        LAN_GetServerAddressString( p->a1, p->a2, VM_ArgPtr( p->a3 ), p->a4 );
        return 0;
    }

    case UI_LAN_GETSERVERINFO:
    {
        struct __tmp {
            size_t a0;
            int a1;
            int a2;
            size_t a3;
            int a4;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        LAN_GetServerInfo( p->a1, p->a2, VM_ArgPtr( p->a3 ), p->a4 );
        return 0;
    }

    case UI_LAN_GETSERVERPING:
    {
        struct __tmp {
            size_t a0;
            int a1;
            int a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return LAN_GetServerPing( p->a1, p->a2 );
    }

    case UI_LAN_MARKSERVERVISIBLE:
    {
        struct __tmp {
            size_t a0;
            int a1;
            int a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        LAN_MarkServerVisible( p->a1, p->a2, p->a3 );
        return 0;
    }

    case UI_LAN_SERVERISVISIBLE:
    {
        struct __tmp {
            size_t a0;
            int a1;
            int a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return LAN_ServerIsVisible( p->a1, p->a2 );
    }

    case UI_LAN_UPDATEVISIBLEPINGS:
    {
        struct __tmp {
            size_t a0;
            int a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return LAN_UpdateVisiblePings( p->a1 );
    }

    case UI_LAN_RESETPINGS:
    {
        struct __tmp {
            size_t a0;
            int a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        LAN_ResetPings( p->a1 );
        return 0;
    }

    case UI_LAN_SERVERSTATUS:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return LAN_GetServerStatus( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ), p->a3 );
    }

    case UI_LAN_COMPARESERVERS:
    {
        struct __tmp {
            size_t a0;
            int a1;
            int a2;
            int a3;
            int a4;
            int a5;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return LAN_CompareServers( p->a1, p->a2, p->a3, p->a4, p->a5 );
    }

    case UI_MEMORY_REMAINING:
    {
        return (int) Hunk_MemoryRemaining();
    }

    case UI_GET_CDKEY:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            int a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        CLUI_GetCDKey( VM_ArgPtr( p->a1 ), p->a2 );
        return 0;
    }

    case UI_SET_CDKEY:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        CLUI_SetCDKey( VM_ArgPtr( p->a1 ) );
        return 0;
    }
    
    case UI_SET_PBCLSTATUS:
    {
        return 0;    
    }

    case UI_R_REGISTERFONT:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            int a2;
            size_t a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.RegisterFont( VM_ArgPtr( p->a1 ), p->a2, VM_ArgPtr( p->a3 ));
        return 0;
    }

    case UI_MEMSET:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            int a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Com_Memset( VM_ArgPtr( p->a1 ), p->a2, p->a3 );
        return 0;
    }

    case UI_MEMCPY:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        Com_Memcpy( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ), p->a3 );
        return 0;
    }

    case UI_STRNCPY:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
            int a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return (int)strncpy( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ), p->a3 );
    }

    case UI_SIN:
    {
        struct __tmp {
            size_t a0;
            float a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FloatAsInt( sin( p->a1 ) );
    }

    case UI_COS:
    {
        struct __tmp {
            size_t a0;
            float a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FloatAsInt( cos( p->a1 ) );
    }

    case UI_ATAN2:
    {
        struct __tmp {
            size_t a0;
            float a1;
            float a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FloatAsInt( atan2( p->a1, p->a2 ) );
    }

    case UI_SQRT:
    {
        struct __tmp {
            size_t a0;
            float a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FloatAsInt( sqrt( p->a1 ) );
    }

    case UI_FLOOR:
    {
        struct __tmp {
            size_t a0;
            float a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FloatAsInt( floor( p->a1 ) );
    }

    case UI_CEIL:
    {
        struct __tmp {
            size_t a0;
            float a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return FloatAsInt( ceil( p->a1 ) );
    }

    case UI_PC_ADD_GLOBAL_DEFINE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return botlib_export->PC_AddGlobalDefine( VM_ArgPtr( p->a1 ) );
    }
    case UI_PC_LOAD_SOURCE:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return botlib_export->PC_LoadSourceHandle( VM_ArgPtr( p->a1 ) );
    }
    case UI_PC_FREE_SOURCE:
    {
        struct __tmp {
            size_t a0;
            int a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return botlib_export->PC_FreeSourceHandle( p->a1 );
    }
    case UI_PC_READ_TOKEN:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return botlib_export->PC_ReadTokenHandle( p->a1, VM_ArgPtr( p->a2 ) );
    }
    case UI_PC_SOURCE_FILE_AND_LINE:
    {
        struct __tmp {
            size_t a0;
            int a1;
            size_t a2;
            size_t a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return botlib_export->PC_SourceFileAndLine( p->a1, VM_ArgPtr( p->a2 ), VM_ArgPtr( p->a3 ) );
    }

    case UI_S_STOPBACKGROUNDTRACK:
    {
        S_StopBackgroundTrack();
        return 0;
    }
    case UI_S_STARTBACKGROUNDTRACK:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        S_StartBackgroundTrack( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ));
        return 0;
    }

    case UI_REAL_TIME:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return Com_RealTime( VM_ArgPtr( p->a1 ) );
    }

    case UI_CIN_PLAYCINEMATIC:
    {
      Com_DPrintf("UI_CIN_PlayCinematic\n");
      struct __tmp {
          size_t a0;
          size_t a1;
          int a2;
          int a3;
          int a4;
          int a5;
          int a6;
      };
      const struct __tmp* p = (const struct __tmp*) args;
      return CIN_PlayCinematic(VM_ArgPtr( p->a1 ), p->a2, p->a3, p->a4, p->a5, p->a6);
  }

    case UI_CIN_STOPCINEMATIC:
    {
      struct __tmp {
          size_t a0;
          int a1;
      };
      const struct __tmp* p = (const struct __tmp*) args;
      return CIN_StopCinematic(p->a1);
  }

    case UI_CIN_RUNCINEMATIC:
    {
      struct __tmp {
          size_t a0;
          int a1;
      };
      const struct __tmp* p = (const struct __tmp*) args;
      return CIN_RunCinematic(p->a1);
  }

    case UI_CIN_DRAWCINEMATIC:
    {
      struct __tmp {
          size_t a0;
          int a1;
      };
      const struct __tmp* p = (const struct __tmp*) args;
      CIN_DrawCinematic(p->a1);
      return 0;
  }

    case UI_CIN_SETEXTENTS:
    {
      struct __tmp {
          size_t a0;
          int a1;
          int a2;
          int a3;
          int a4;
          int a5;
      };
      const struct __tmp* p = (const struct __tmp*) args;
      CIN_SetExtents(p->a1, p->a2, p->a3, p->a4, p->a5);
      return 0;
  }

    case UI_R_REMAP_SHADER:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
            size_t a3;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        re.RemapShader( VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ), VM_ArgPtr( p->a3 ) );
        return 0;
    }

    case UI_VERIFY_CDKEY:
    {
        struct __tmp {
            size_t a0;
            size_t a1;
            size_t a2;
        };
        const struct __tmp* p = (const struct __tmp*) args;
        return CL_CDKeyValidate(VM_ArgPtr( p->a1 ), VM_ArgPtr( p->a2 ));
    }
#pragma pack(pop)
