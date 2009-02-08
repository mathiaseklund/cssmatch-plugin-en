/* 
 * Copyright 2008, 2009 Nicolas Maingot
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

#include "ConCommandCallbacks.h"
#include "../plugin/SimplePlugin.h"

namespace cssmatch
{
	void cssm_help()
	{
		SimplePlugin * plugin = SimplePlugin::getInstance();

		const std::list<ConCommand *> * pluginConCommands = plugin->getPluginConCommands();
		std::list<ConCommand *>::const_iterator itConCommand = pluginConCommands->begin();
		std::list<ConCommand *>::const_iterator lastConCommand = pluginConCommands->end();
		while (itConCommand != lastConCommand)
		{
			ConCommand * command = *itConCommand;
			print(std::string(command->GetName()) + " : " + command->GetHelpText());

			itConCommand++;
		}
	}

	void cssm_start()
	{
		SimplePlugin * plugin = SimplePlugin::getInstance();
		MatchManager * match = plugin->getMatchManager();
	}

	void cssm_stop()
	{
		SimplePlugin * plugin = SimplePlugin::getInstance();
		MatchManager * match = plugin->getMatchManager();
		ValveInterfaces * interfaces = plugin->getInterfaces();

		match->setMatchState(new DisableMatchState(match,interfaces->gameeventmanager2));
	}
}
