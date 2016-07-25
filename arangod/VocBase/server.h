////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_VOC_BASE_SERVER_H
#define ARANGOD_VOC_BASE_SERVER_H 1

#include "Basics/Common.h"
#include "Basics/Mutex.h"
#include "Basics/DataProtector.h"
#include "VocBase/ticks.h"
#include "VocBase/voc-types.h"

struct TRI_vocbase_t;

namespace arangodb {
namespace aql {
class QueryRegistry;
}
namespace rest {
class ApplicationEndpointServer;
}
}

/// @brief server structure
struct TRI_server_t {
  TRI_server_t();
  ~TRI_server_t();

  std::atomic<DatabasesLists*> _databasesLists;
  // TODO: Make this again a template once everybody has gcc >= 4.9.2
  // arangodb::basics::DataProtector<64>
  arangodb::basics::DataProtector _databasesProtector;
  arangodb::Mutex _databasesMutex;

  std::atomic<arangodb::aql::QueryRegistry*> _queryRegistry;

  char* _basePath;
  char* _databasePath;

  bool _disableReplicationAppliers;
  bool _disableCompactor;
  bool _iterateMarkersOnOpen;
  bool _initialized;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief initializes all databases
////////////////////////////////////////////////////////////////////////////////

int TRI_InitDatabasesServer(TRI_server_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief get the ids of all local coordinator databases
////////////////////////////////////////////////////////////////////////////////

std::vector<TRI_voc_tick_t> TRI_GetIdsCoordinatorDatabaseServer(TRI_server_t*, 
                                                                bool includeSystem = false);

////////////////////////////////////////////////////////////////////////////////
/// @brief drops an existing coordinator database
////////////////////////////////////////////////////////////////////////////////

int TRI_DropByIdCoordinatorDatabaseServer(TRI_server_t*, TRI_voc_tick_t, bool);

////////////////////////////////////////////////////////////////////////////////
/// @brief get a coordinator database by its id
/// this will increase the reference-counter for the database
////////////////////////////////////////////////////////////////////////////////

TRI_vocbase_t* TRI_UseByIdCoordinatorDatabaseServer(TRI_server_t*,
                                                    TRI_voc_tick_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief use a coordinator database by its name
/// this will increase the reference-counter for the database
////////////////////////////////////////////////////////////////////////////////

TRI_vocbase_t* TRI_UseCoordinatorDatabaseServer(TRI_server_t*, char const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief use a database by its name
/// this will increase the reference-counter for the database
////////////////////////////////////////////////////////////////////////////////

TRI_vocbase_t* TRI_UseDatabaseServer(TRI_server_t*, char const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief lookup a database by its name
////////////////////////////////////////////////////////////////////////////////

TRI_vocbase_t* TRI_LookupDatabaseByNameServer(TRI_server_t*, char const*);

////////////////////////////////////////////////////////////////////////////////
/// @brief use a database by its id
/// this will increase the reference-counter for the database
////////////////////////////////////////////////////////////////////////////////

TRI_vocbase_t* TRI_UseDatabaseByIdServer(TRI_server_t*, TRI_voc_tick_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief release a previously used database
/// this will decrease the reference-counter for the database
////////////////////////////////////////////////////////////////////////////////

void TRI_ReleaseDatabaseServer(TRI_server_t*, TRI_vocbase_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief return the list of all databases a user can see
////////////////////////////////////////////////////////////////////////////////

int TRI_GetUserDatabasesServer(TRI_server_t*, char const*,
                               std::vector<std::string>&);

////////////////////////////////////////////////////////////////////////////////
/// @brief return the list of all database names
////////////////////////////////////////////////////////////////////////////////

int TRI_GetDatabaseNamesServer(TRI_server_t*, std::vector<std::string>&);

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the current operation mode of the server
////////////////////////////////////////////////////////////////////////////////

int TRI_ChangeOperationModeServer(TRI_vocbase_operationmode_e);

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the current operation mode of the server
////////////////////////////////////////////////////////////////////////////////

TRI_vocbase_operationmode_e TRI_GetOperationModeServer();

#endif
