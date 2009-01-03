/* 
 * Copyright 2007, 2008 Nicolas Maingot
 * 
 * This file is part of CSSMatch.
 * 
 * CSSMatch is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSSMatch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSSMatch; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Portions of this code are also Copyright � 1996-2005 Valve Corporation, All rights reserved
 */

#include "SimplePlugin.h"
#include "../convars/I18nConVar.h"
#include "../commands/I18nConCommand.h"
#include "../match/WarmupMatchState.h"

#include <algorithm>
#include <sstream>

namespace cssmatch
{
	ValveInterfaces::ValveInterfaces() :
		engine(NULL),
		filesystem(NULL),
		gameeventmanager2(NULL),
		helpers(NULL),
		gpGlobals(NULL),
		convars(NULL)
	{
	}

	SimplePlugin::SimplePlugin() : clientCommandIndex(0), match(NULL), i18n(NULL)
	{
	}

	SimplePlugin::~SimplePlugin()
	{
		if (interfaces.convars != NULL)
			delete interfaces.convars;

		std::for_each(playerlist.begin(),playerlist.end(),PlayerToRemove());

		if (match != NULL)
			delete match;

		if (i18n != NULL)
			delete i18n;
	}

	bool SimplePlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
	{
		bool success = true;

		try
		{
			interfaces.convars = new ConvarsAccessor();
			interfaces.convars->initializeInterface(interfaceFactory);
		}
		catch(const BaseConvarsAccessorException & e)
		{
			success = false;
			Msg(PLUGIN_NAME " : %s\n",e.what()); // Do not use printException here !
		}

		success &= 
			getInterface<IPlayerInfoManager>(gameServerFactory,interfaces.playerinfomanager,INTERFACEVERSION_PLAYERINFOMANAGER) &&
			getInterface<IVEngineServer>(interfaceFactory,interfaces.engine,INTERFACEVERSION_VENGINESERVER) &&
			getInterface<IGameEventManager2>(interfaceFactory,interfaces.gameeventmanager2,INTERFACEVERSION_GAMEEVENTSMANAGER2) &&
			getInterface<IFileSystem>(interfaceFactory,interfaces.filesystem,FILESYSTEM_INTERFACE_VERSION) &&
			getInterface<IServerPluginHelpers>(interfaceFactory,interfaces.helpers,INTERFACEVERSION_ISERVERPLUGINHELPERS);

		if (success)
		{
			interfaces.gpGlobals = interfaces.playerinfomanager->GetGlobalVars();

			match = new MatchManager(this);
		}

		MathLib_Init(2.2f,2.2f,0.0f,2.0f);

		// Create the plugin's convars
		ConVar * cssmatch_language = new ConVar("cssmatch_language",
												"english",
												FCVAR_PLUGIN,PLUGIN_NAME " : Default language of CSSMatch (e.g. : \"english\" \
																	   will use the file  cfg/cssmatch/languages/english.txt)");
		//	Initialize the translations tools
		i18n = new I18nManager(cssmatch_language);

		addPluginConVar(cssmatch_language);
		addPluginConVar(new I18nConVar(i18n,"cssmatch_version",PLUGIN_VERSION_LIGHT,FCVAR_NONE,"cssmatch_version"));

		return success;
	}

	ValveInterfaces * SimplePlugin::getInterfaces()
	{
		return &interfaces;
	}

	int SimplePlugin::GetCommandIndex() const
	{
		return clientCommandIndex; 
	}

	void SimplePlugin::SetCommandClient(int index)
	{
		clientCommandIndex = index;
	}

	std::list<ClanMember *> * SimplePlugin::getPlayerlist()
	{
		return &playerlist;
	}

	void SimplePlugin::addPluginConVar(ConVar * variable)
	{
		pluginConVars.push_back(variable);
	}

	const std::list<ConVar *> * SimplePlugin::getPluginConVars() const
	{
		return &pluginConVars;
	}

	I18nManager * SimplePlugin::get18nManager()
	{
		return i18n;
	}

	void SimplePlugin::addTimer(BaseTimer * timer)
	{
		timers.push_back(timer);
	}

	void SimplePlugin::setConvarsAccessor(BaseConvarsAccessor * convarsAccessor)
	{
		if (interfaces.convars != NULL)
			delete interfaces.convars;

		interfaces.convars = convarsAccessor;
	}

	void SimplePlugin::Unload()
	{
	}

	void SimplePlugin::Pause()
	{
	}

	void SimplePlugin::UnPause()
	{
	}

	const char * SimplePlugin::GetPluginDescription()
	{
		return PLUGIN_VERSION;
	}

	void SimplePlugin::LevelInit(char const * pMapName)
	{
	}

	void SimplePlugin::ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
	{
	}

	void SimplePlugin::GameFrame(bool simulating)
	{
		// Execute and remove the timers out of date
		timers.remove_if(TimerOutOfDate(interfaces.gpGlobals->curtime));
	}

	void SimplePlugin::LevelShutdown() // !!!!this can get called multiple times per map change
	{
	}

	void SimplePlugin::ClientActive(edict_t * pEntity)
	{
	}

	void SimplePlugin::ClientDisconnect(edict_t * pEntity)
	{
		std::list<ClanMember *>::iterator endPlayer = playerlist.end();
		std::list<ClanMember *>::iterator itPlayer = std::find_if(playerlist.begin(),endPlayer,PlayerHavingPEntity(pEntity));
		if (itPlayer != endPlayer)
		{
			ClanMember * toRemove = *itPlayer;
			playerlist.remove(toRemove);
			PlayerToRemove()(toRemove);
		}
	}

	void SimplePlugin::ClientPutInServer(edict_t * pEntity, char const * playername)
	{
		int index = interfaces.engine->IndexOfEdict(pEntity);
        if (isValidPlayerIndex(index,interfaces.gpGlobals->maxClients))
        {
                try
                {
                        playerlist.push_back(new ClanMember(interfaces.engine,interfaces.playerinfomanager,index));
                }
                catch(const PlayerException & e)
                {
                        printException(e,__FILE__,__LINE__);
                }
        }

		//match->setMatchState(new WarmupMatchState(match,interfaces.gameeventmanager2)); // test

	}

	void SimplePlugin::ClientSettingsChanged(edict_t * pEdict)
	{
	}

	PLUGIN_RESULT SimplePlugin::ClientConnect(bool * bAllowConnect,
											edict_t * pEntity,
											const char * pszName,
											const char * pszAddress,
											char * reject,
											int maxrejectlen)
	{
		return PLUGIN_CONTINUE;
	}

	PLUGIN_RESULT SimplePlugin::ClientCommand(edict_t * pEntity)
	{
		return PLUGIN_CONTINUE;
	}

	PLUGIN_RESULT SimplePlugin::NetworkIDValidated(const char * pszUserName, const char * pszNetworkID)
	{
		return PLUGIN_CONTINUE;
	}

	void SimplePlugin::log(const std::string & message) const
	{
		std::ostringstream buffer;
		buffer << PLUGIN_NAME << " : " << message << "\n";
		interfaces.engine->LogPrint(buffer.str().c_str());
	}

	void SimplePlugin::printException(const BaseException & e, const std::string & fileName, int line) const
	{
		std::ostringstream buffer;
		buffer << e.what() << " (" << fileName << ", l." << line << ")";
		log(buffer.str());
	}

	void SimplePlugin::queueCommand(const std::string & command) const
	{
		interfaces.engine->ServerCommand(command.c_str());
	}

	bool SimplePlugin::hltvConnected() const
	{
		std::list<ClanMember *>::const_iterator firstPlayer = playerlist.begin();
		std::list<ClanMember *>::const_iterator lastPlayer = playerlist.end();

		return std::find_if(firstPlayer,lastPlayer,PlayerIsHltv()) != lastPlayer;
	}

	int SimplePlugin::getPlayerCount(TeamCode team) const
	{
		int playerCount = 0;

		if (team == INVALID_TEAM)
				playerCount = (int)playerlist.size();
		else
				playerCount = std::count_if(playerlist.begin(),playerlist.end(),PlayerHavingTeam(team));
		return playerCount;
	}
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(SimplePlugin, 
								  IServerPluginCallbacks,
								  INTERFACEVERSION_ISERVERPLUGINCALLBACKS,
								  *cssmatch::SimplePlugin::getInstance());
