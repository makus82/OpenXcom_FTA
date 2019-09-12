/*
 * Copyright 2010-2019 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "RuleEventScript.h"

namespace OpenXcom
{

/**
 * RuleEventScript: the (optional) rules for generating custom Geoscape events.
 * Each script element is independent, and the saved game will probe the list of these each month to determine what's going to happen.
 * Event scripts are executed just after the mission scripts.
 */
RuleEventScript::RuleEventScript(const std::string &type) :
	_type(type), _firstMonth(0), _lastMonth(-1), _executionOdds(100), _minDifficulty(0), _maxDifficulty(4)
{
}

/**
 * Cleans up the event script ruleset.
 */
RuleEventScript::~RuleEventScript()
{
	for (std::vector<std::pair<size_t, WeightedOptions*> >::iterator i = _eventWeights.begin(); i != _eventWeights.end(); ++i)
	{
		delete i->second;
	}
}

/**
 * Loads an event script from YAML.
 * @param node YAML node.
 */
void RuleEventScript::load(const YAML::Node &node)
{
	if (const YAML::Node &parent = node["refNode"])
	{
		load(parent);
	}
	_type = node["type"].as<std::string>(_type);
	if (const YAML::Node &weights = node["eventWeights"])
	{
		for (YAML::const_iterator nn = weights.begin(); nn != weights.end(); ++nn)
		{
			WeightedOptions *nw = new WeightedOptions();
			nw->load(nn->second);
			_eventWeights.push_back(std::make_pair(nn->first.as<size_t>(0), nw));
		}
	}
	_firstMonth = node["firstMonth"].as<int>(_firstMonth);
	_lastMonth = node["lastMonth"].as<int>(_lastMonth);
	_executionOdds = node["executionOdds"].as<int>(_executionOdds);
	_minDifficulty = node["minDifficulty"].as<int>(_minDifficulty);
	_maxDifficulty = node["maxDifficulty"].as<int>(_maxDifficulty);
	_researchTriggers = node["researchTriggers"].as<std::map<std::string, bool> >(_researchTriggers);
}

/**
 * Chooses one of the available events for this command.
 * @param monthsPassed The number of months that have passed in the game world.
 * @return The string id of the event.
 */
std::string RuleEventScript::generate(const size_t monthsPassed) const
{
	std::vector<std::pair<size_t, WeightedOptions*> >::const_reverse_iterator rw;
	rw = _eventWeights.rbegin();
	while (monthsPassed < rw->first)
		++rw;
	return rw->second->choose();
}

}
