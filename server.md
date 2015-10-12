Often your server.cfg is incomplete, so at the end of the match the server configuration is not correctly restored

Here is a complete server.cfg:

```
// Server infos
hostname "Server name"
sv_contact "Admin e-mail"
sv_lan 0 // (0) LAN (1) Web
sv_region 3 // (0) USA East Cost, (1) USA West Cot, (2) South America, (3) Europe, (4) Asia, (5) Australia, (6) Orient, (7) Africa, (255) the World
// sv_visiblemaxplayers 17


// Security
rcon_password "Your RCON password"
sv_password "" // server password ("" <=> no password)
sv_rcon_banpenalty 0
sv_rcon_minfailures 5
sv_rcon_minfailuretime 30
sv_rcon_maxfailures 10


// Downloads
// sv_downloadurl "your-mirror/cstrike"


// Logs
log on
sv_logbans 1
sv_logecho 1
sv_logfile 1
sv_log_onefile 1


// Configuration files
exec banned_user.cfg
exec banned_ip.cfg
exec mani_server.cfg


// Connectivity (tickrate 66)
sv_allowupload 1
sv_minupdaterate 66
sv_maxupdaterate 66
sv_minrate 30000
sv_maxrate 35000
sv_mincmdrate 66
sv_maxcmdrate 66
decalfrequency 10
fps_max 0


// Game configuration
mp_allowspectators 1
mp_autocrosshair 0
mp_autokick 0
mp_autoteambalance 1
mp_buytime 1.5
mp_c4timer 35
mp_chattime 6
mp_fadetoblack 0
mp_flashlight 1
mp_footsteps 1
mp_forcecamera 0
mp_fraglimit 0
mp_freezetime 0
mp_friendlyfire 1
mp_hostagepenalty 0
mp_limitteams 0
mp_maxrounds 0
mp_playerid 0
mp_roundtime 3
mp_spawnprotectiontime 0
mp_startmoney 800
mp_timelimit 20
mp_tkpunish 1
mp_weaponstay 1
mp_winlimit 0
sv_airaccelerate 10
sv_airmove 1
sv_alltalk 1
sv_cheats 0
sv_clienttrace 1
sv_consistency 1
sv_gravity 800
sv_maxspeed 320
sv_pausable 0
sv_timeout 120
sv_voiceenable 1
phys_pushscale 1
phys_timescale 1

ma_war 0

mattie_eventscripts 1
```