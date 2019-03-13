/*    Copyright 2012 SequoiaDB Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*    Copyright (C) 2011-2014 SequoiaDB Ltd.
 *    This program is free software: you can redistribute it and/or modify
 *    it under the term of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warrenty of
 *    MARCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program. If not, see <http://www.gnu.org/license/>.
 */


// This Header File is automatically generated, you MUST NOT modify this file anyway!
// On the contrary, you can modify the xml file "sequoiadb/misc/rcgen/rclist.xml" if necessary!

#include "ossErr.h"

const CHAR* getErrDesp ( INT32 errCode )
{
   INT32 code = -errCode;
   const static CHAR* errDesp[] =
   {
      "Succeed",
      "IO Exception",
      "Out of Memory",
      "Permission Error",
      "File Not Exist",
      "File Exist",
      "Invalid Argument",
      "Invalid size",
      "Interrupt",
      "Hit end of file",
      "System error",
      "No space is left on disk",
      "EDU status is not valid",
      "Timeout error",
      "Database is quiesced",
      "Network error",
      "Network is closed from remote",
      "Database is in shutdown status",
      "Application is forced",
      "Given path is not valid",
      "Unexpected file type specified",
      "There's no space for DMS",
      "Collection already exists",
      "Collection does not exist",
      "User record is too large",
      "Record does not exist",
      "Remote overflow record exists",
      "Invalid record",
      "Storage unit need reorg",
      "End of collection",
      "Context is already opened",
      "Context is closed",
      "Option is not supported yet",
      "Collection space already exists",
      "Collection space does not exist",
      "Storage unit file is invalid",
      "Context does not exist",
      "More than one fields are array type",
      "Duplicate key exist",
      "Index key is too large",
      "No space can be found for index extent",
      "Index key does not exist",
      "Hit max number of index",
      "Failed to initialize index",
      "Collection is dropped",
      "Two records get same key and rid",
      "Duplicate index name",
      "Index name does not exist",
      "Unexpected index flag",
      "Hit end of index",
      "Hit the max of dedup buffer",
      "Invalid predicates",
      "Index does not exist",
      "Invalid hint",
      "No more temp collections are avaliable",
      "Exceed max number of storage unit",
      "$id index can't be dropped",
      "Log was not found in log buf",
      "Log was not found in log file",
      "Replication group does not exist",
      "Replication group exists",
      "Invalid request id is received",
      "Session ID does not exist",
      "System EDU cannot be forced",
      "Database is not connected",
      "Unexpected result received",
      "Corrupted record",
      "Backup has already been started",
      "Backup is not completed",
      "Backup is in progress",
      "Backup is corrupted",
      "No primary node was found",
      "Reserved",
      "Engine help argument is specified",
      "Invalid connection state",
      "Invalid handle",
      "Object does not exist",
      "Listening port is already occupied",
      "Unable to listen the specified address",
      "Unable to connect to the specified address",
      "Connection does not exist",
      "Failed to send",
      "Timer does not exist",
      "Route info does not exist",
      "Broken msg",
      "Invalid net handle",
      "Invalid reorg file",
      "Reorg file is in read only mode",
      "Collection status is not valid",
      "Collection is not in reorg state",
      "Replication group is not activated",
      "Node does not belong to the group",
      "Collection status is not compatible",
      "Incompatible version for storage unit",
      "Version is expired for local group",
      "Invalid page size",
      "Version is expired for remote group",
      "Failed to vote for primary",
      "Log record is corrupted",
      "LSN is out of boundary",
      "Unknown mesage is received",
      "Updated information is same as old one",
      "Unknown message",
      "Empty heap",
      "Node is not primary",
      "Not enough number of data nodes",
      "Catalog information does not exist on data node",
      "Catalog version is expired on data node",
      "Catalog version is expired on coordinator node",
      "Exceeds the max group size",
      "Failed to sync log",
      "Failed to replay log",
      "Invalid HTTP header",
      "Failed to negotiate",
      "Failed to change DPS metadata",
      "SME is corrupted",
      "Application is interrupted",
      "Application is disconnected",
      "Character encoding errors",
      "Failed to query on coord node",
      "Buffer array is full",
      "Sub context is conflict",
      "EOC message is received by coordinator node",
      "Size of DPS files are not the same",
      "Invalid DPS log file",
      "No resource is avaliable",
      "Invalid LSN",
      "Pipe buffer size is too small",
      "Catalog authentication failed",
      "Full sync is in progress",
      "Failed to assign data node from coordinator node",
      "PHP driver internal error",
      "Failed to send the message",
      "No activated group information on catalog",
      "Remote-node is disconnected",
      "Unable to find the matched catalog information",
      "Failed to update catalog",
      "Unknown request operation code",
      "Group information cannot be found on coordinator node",
      "DMS extent is corrupted",
      "Remote cluster manager failed",
      "Remote database services have been stopped",
      "Service is starting",
      "Service has been started",
      "Service is restarting",
      "Node already exists",
      "Node does not exist",
      "Unable to lock",
      "DMS state is not compatible with current command",
      "Database rebuild is already started",
      "Database rebuild is in progress",
      "Cache is empty on coordinator node",
      "Evalution failed with error",
      "Group already exist",
      "Group does not exist",
      "Node does not exist",
      "Failed to start the node",
      "Invalid node configuration",
      "Group is empty",
      "The operation is for coord node only",
      "Failed to operate on node",
      "The mutex job already exist",
      "The specified job does not exist",
      "The catalog information is corrupted",
      "$shard index can't be dropped",
      "The command can't be run in the node",
      "The command can't be run in the service plane",
      "The group info not exist",
      "Group name is conflict",
      "The collection is not sharded",
      "The record does not contains valid sharding key",
      "A task that already exists does not compatible with the new task",
      "The collection does not exists on the specified group",
      "The specified task does not exist",
      "The record contains more than one sharding key",
      "The mutex task already exist",
      "The split key is not valid or not in the source group",
      "The unique index must include all fields in sharding key",
      "Sharding key cannot be updated",
      "Authority is forbidden",
      "There is no catalog address specified by user",
      "Current record has been removed",
      "No records can be matched for the search condition",
      "Index page is reorged and the pos got different lchild",
      "Duplicate field name exists in the record",
      "Too many records to be inserted at once",
      "Sort-Merge Join only supports equal predicates",
      "Trace is already started",
      "Trace buffer does not exist",
      "Trace file is not valid",
      "Incompatible lock",
      "Rollback operation is in progress",
      "Invalid record is found during import",
      "Repeated variable name",
      "Column name is ambiguous",
      "SQL syntax error",
      "Invalid transactional operation",
      "Append to lock-wait-queue",
      "Record is deleted",
      "Index is dropped or invalid",
      "Unable to create new catalog when there's already one exists",
      "Failed to parse JSON file",
      "Failed to parse CSV file",
      "Log file size is too large",
      "Unable to remove the last node or primary in a group",
      "Unable to clean up catalog, manual cleanup may be required",
      "Unable to remove primary catalog or catalog group for non-empty database",
      "Reserved",
      "Unable to remove non-empty group",
      "End of queue",
      "Unable to split because of no sharding index exists",
      "The parameter field does not exist",
      "Too many break points are specified",
      "All prefetchers are busy",
      "Domain does not exist",
      "Domain already exists",
      "Group is not in domain",
      "Sharding type is not hash",
      "split percentage is lower then expected",
      "Task is already finished",
      "Collection is in loading status",
      "Rolling back load operation",
      "RouteID is different from the local",
      "Service already exists",
      "Field is not found",
      "csv field line end",
      "Unknown file type",
      "Exporting configuration does not complete in all nodes",
      "Empty non-primary node",
      "Secret value for index file does not match with data file",
      "Engine version argument is specified",
      "Help argument is specified",
      "Version argument is specified",
      "Stored procedure does not exist",
      "Unable to remove collection partition",
      "Duplicated attach collection partition",
      "Invalid partitioned-collection",
      "New boundary is conflict with the existing boundary",
      "Invalid boundary for the shard",
      "Hit the high water mark",
      "Backup already exists",
      "Backup does not exist",
      "Invalid collection partition",
      "Task is canceled",
      "Sharding type must be ranged partition for partitioned-collection",
      "There is no valid sharding-key defined",
      "Operation is not supported on partitioned-collection",
      "Redefine index",
      "Dropping the collection space is in progress",
      "Hit the limit of maximum number of nodes in the cluster",
      "The node is not in normal status",
      "Node information is expired",
      "Failed to wait for the sync operation from secondary nodes",
      "Transaction is disabled",
      "Data source is running out of connection pool",
      "Too many opened file description",
      "Domain is not empty",
      "The data received by REST is larger than the max size",
      "Failed to build bson object",
      "Stored procedure arguments are out of bound",
      "Unknown REST command",
      "Failed to execute command on data node",
      "The domain is empty",
      "Changing password is required",
      "One or more nodes did not complete successfully",
      "There is another OM Agent running with different version",
      "Task does not exist",
      "Task is rolling back",
      "LOB sequence does not exist",
      "LOB is not useable",
      "Data is not in UTF-8 format",
      "Task failed",
      "Lob does not open",
      "Lob has been open",
      "Node is in restoring",
      "There are some collections in the collection space",
      "'localhost' and '127.0.0.1' cannot be used mixed with other hostname and IP address",
      "If use 'localhost' and '127.0.0.1' for hostname, coord and catalog must in the same host ",
      "The special group is not data group",
      "can not update/delete records when $id index does not exist",
      "can not step up when primary node exists or LSN is not the biggest",
      "Image address is conflict with the self cluster",
      "The data group does not have image group",
      "The data group has image group",
      "The image is in enabled status",
      "The cluster's image does not configured",
      "This cluster and image cluster is both writable",
      "This cluster is readonly",
      "Sorting of 'query and modify' must use index",
      "'query and modify' can't use skip and limit in multiple nodes or sub-collections",
      "Given path is not empty",
      "Exist one index which can cover this scene",
      "The cluster's image has already configured",
      "The command is in local mode",
      "The object is not a special object in sdb shell",
      "The specified user already exist",
      "The collection is empty",
      "LOB sequence exists",
      "cluster do not exist",
      "business do not exist",
      "user specified is not exist or password is invalid",
      "Compression initialization failed",
      "Compression failed",
      "Decompression failed",
      "Compression abort",
      "Buffer for compression is too small",
      "Buffer for decompression is too small",
      "Up to the limit",
      "data source is not enabled yet",
      "No reachable coord notes",
      "the record which exclusive ruleID is not exist",
      "Task name conflict",
      "The task is not existed",
      "Replica log is not archived",
      "Data source has not been initialized",
      "Operation is incompatible with the object",
      "This cluster is deactived",
      "LOB is in use",
      "Data operation is overflowed",
      "LOB's pieces info is overflowed"
   };
   if ( code < 0 || (UINT32)code >= (sizeof ( errDesp ) / sizeof ( CHAR* )) )
   {
      return "unknown error";
   }
   return errDesp[code];
}
