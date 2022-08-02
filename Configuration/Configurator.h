/* © Copyright CERN, 2015. All rights not expressly granted are reserved. */
/*
 * Configurator.h
 *
 *  Created on: Jun 6, 2014
 *      Author: Piotr Nikiel
 *
 *  This file is part of Quasar.
 *
 *  Quasar is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public Licence as published by
 *  the Free Software Foundation, either version 3 of the Licence.
 *
 *  Quasar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public Licence for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Quasar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_

#include <string>
#include <set>
#include <numeric>
#include <functional>

#include <ASNodeManager.h>

// forward-decls
namespace Configuration{ class Configuration; }

typedef std::function<bool (Configuration::Configuration&)> ConfigXmlDecoratorFunction;
bool configure (std::string fileName,
        AddressSpace::ASNodeManager *nm, ConfigXmlDecoratorFunction
        configXmlDecoratorFunction = ConfigXmlDecoratorFunction()); // 'empty' function by default.

/*
* Implementation where children is an optional (0/1)
*/
template <typename TParent, typename TChildren, typename TChildTypeId>
auto validateContentOrderImpl(const TParent& parent, const TChildren& children, const TChildTypeId childTypeId)
  -> decltype(children.present(), void()) // children type needs method present()
{
  if(!children.present()) return; // nothing to check

  bool isChildRegistered = false;
  for(const auto& orderedIter : parent.content_order())
  {
    if(orderedIter.id == childTypeId)
    {
      isChildRegistered = true;
      break;
    }
  }

  if(!isChildRegistered)
  {
    std::ostringstream msg;
    msg<<__FUNCTION__<<" ERROR optional child type id ["<<childTypeId<<"] not registered in parent content order";
    throw std::range_error(msg.str());
  }
}

/*
* Implementation where children is an array (0/N)
*/
template<typename TParent, typename TChildren, typename TChildTypeId>
auto validateContentOrderImpl(const TParent& parent, const TChildren& children, const TChildTypeId childTypeId)
  -> decltype(children.size(), void()) // children type needs method size()
{
  std::set<size_t> validChildIndices;
  for(size_t i=0; i<children.size(); validChildIndices.insert(i++));

  for(const auto& orderedIter : parent.content_order())
  {
    const auto xmlIndex = orderedIter.index;
    if(orderedIter.id == childTypeId)
    {
      auto pos = std::find(std::begin(validChildIndices), std::end(validChildIndices), xmlIndex);
      if(pos == std::end(validChildIndices))
      {
        std::ostringstream msg;
        msg<<__FUNCTION__<<" ERROR content child type id ["<<childTypeId<<"] order index ["<<xmlIndex<<"] invalid. Valid={0.."<<children.size()<<"}";
        throw std::range_error(msg.str());
      }
      validChildIndices.erase(pos);
    }
  }
  if(!validChildIndices.empty())
  {
    std::ostringstream msg;
    msg<<__FUNCTION__<<" ERROR parent has ["<<validChildIndices.size()<<"] child objects unregistered in content order";
    throw std::range_error(msg.str());
  }
}

/*
* Use SFINAE (it's a thing) to resolve to the appropriate validateContentOrderImpl above, depending on children type.
* Chidlren can be one of
* - multiple (array - 0..N): children type has function size()
* - optional (optional - 0/1): children type has function present()
*/
template<typename TParent, typename TChildren, typename TChildTypeId>
auto validateContentOrder(const TParent& parent, const TChildren& children, const TChildTypeId childTypeId)
  -> decltype(validateContentOrderImpl(parent, children, childTypeId), void())
{
  validateContentOrderImpl(parent, children, childTypeId);
}

void unlinkAllDevices (AddressSpace::ASNodeManager *nm);

/* The body for that one is generated in ConfigValidator.cpp */
bool validateDeviceTree ();


#endif /* CONFIGURATOR_H_ */
