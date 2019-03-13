/*
 * Copyright 2018 SequoiaDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

package com.sequoiadb.base;

import com.sequoiadb.exception.BaseException;
import com.sequoiadb.exception.SDBError;
import com.sequoiadb.message.request.AdminRequest;
import com.sequoiadb.message.response.SdbReply;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;
import org.bson.types.BasicBSONList;
import java.util.ArrayList;
import java.util.List;

/**
 * Collection space of SequoiaDB.
 */
public class CollectionSpace {
    private String name;
    private Sequoiadb sequoiadb;

    /**
     * Return the name of current collection space.
     *
     * @return The collection space name
     */
    public String getName() {
        return name;
    }

    /**
     * Get the Sequoiadb instance of current collection space belongs to.
     *
     * @return Sequoiadb object
     */
    public Sequoiadb getSequoiadb() {
        return sequoiadb;
    }

    /**
     * @param sequoiadb Sequoiadb instance
     * @param name      Collection space name
     */
    CollectionSpace(Sequoiadb sequoiadb, String name) {
        this.name = name;
        this.sequoiadb = sequoiadb;
    }

    /**
     * Get the named collection.
     *
     * @param collectionName The collection name
     * @return the object of the specified collection, or an exception when the collection does not exist.
     * @throws BaseException If error happens.
     */
    public DBCollection getCollection(String collectionName) throws BaseException {
        // get cl from cache
        String collectionFullName = name + "." + collectionName;
        if (sequoiadb.fetchCache(collectionFullName)) {
            return new DBCollection(sequoiadb, this, collectionName);
        }
        // create cl and then update cache
        BSONObject obj = new BasicBSONObject();
        obj.put(SdbConstants.FIELD_NAME_NAME, collectionFullName);
        AdminRequest request = new AdminRequest(AdminCommand.TEST_CL, obj);
        SdbReply response = sequoiadb.requestAndResponse(request);
        sequoiadb.throwIfError(response);
        sequoiadb.upsertCache(collectionFullName);
        return new DBCollection(sequoiadb, this, collectionName);
    }

    /**
     * Verify the existence of collection in current collection space.
     *
     * @param collectionName The collection name
     * @return True if collection existed or False if not existed
     * @throws BaseException If error happens.
     */
    public boolean isCollectionExist(String collectionName) throws BaseException {
        String collectionFullName = name + "." + collectionName;

        BSONObject obj = new BasicBSONObject();
        obj.put(SdbConstants.FIELD_NAME_NAME, collectionFullName);

        AdminRequest request = new AdminRequest(AdminCommand.TEST_CL, obj);
        SdbReply response = sequoiadb.requestAndResponse(request);

        int flag = response.getFlag();
        if (flag == 0) {
            sequoiadb.upsertCache(collectionFullName);
            return true;
        } else if (flag == SDBError.SDB_DMS_NOTEXIST.getErrorCode()) {
            sequoiadb.removeCache(collectionFullName);
            return false;
        } else {
            sequoiadb.throwIfError(response);
            return false; // make compiler happy
        }
    }

    /**
     * Get all the collection names of current collection space.
     *
     * @return A list of collection names
     * @throws BaseException If error happens.
     */
    public List<String> getCollectionNames() throws BaseException {
        List<String> collectionNames = new ArrayList<String>();
        List<String> colNames = sequoiadb.getCollectionNames();
        if ((colNames != null) && (colNames.size() != 0)) {
            for (String col : colNames) {
                if (col.startsWith(name + ".")) {
                    collectionNames.add(col);
                }
            }
        }
        return collectionNames;
    }

    /**
     * Create the named collection in current collection space.
     *
     * @param collectionName The collection name
     * @return the newly created object of collection
     * @throws BaseException If error happens.
     */
    public DBCollection createCollection(String collectionName)
            throws BaseException {
        return createCollection(collectionName, null);
    }

    /**
     * Create collection by options.
     *
     * @param collectionName The collection name
     * @param options        The {@see <a href=http://doc.sequoiadb.com/cn/index-cat_id-1432190821-edition_id-300>options</a>}
     *                        for creating collection or null for not specified any options.
     * @return the newly created object of collection.
     * @throws BaseException If error happens.
     */
    public DBCollection createCollection(String collectionName, BSONObject options) {
        String collectionFullName = name + "." + collectionName;
        BSONObject obj = new BasicBSONObject();
        obj.put(SdbConstants.FIELD_NAME_NAME, collectionFullName);
        if (options != null) {
            obj.putAll(options);
        }

        AdminRequest request = new AdminRequest(AdminCommand.CREATE_CL, obj);
        SdbReply response = sequoiadb.requestAndResponse(request);
        String msg = "collection = " + collectionFullName + ", options = " + options;
        sequoiadb.throwIfError(response, msg);
        sequoiadb.upsertCache(collectionFullName);
        return new DBCollection(sequoiadb, this, collectionName);
    }

    /**
     * Drop current collection space.
     *
     * @throws BaseException If error happens.
     * @see com.sequoiadb.base.Sequoiadb#dropCollectionSpace(String)
     * @deprecated Use Sequoiadb.dropCollectionSpace() instead.
     */
    public void drop() throws BaseException {
        sequoiadb.dropCollectionSpace(this.name);
    }

    /**
     * Remove the named collection of current collection space.
     *
     * @param collectionName The collection name
     * @throws BaseException If error happens.
     */
    public void dropCollection(String collectionName) throws BaseException {
        String collectionFullName = name + "." + collectionName;
        BSONObject obj = new BasicBSONObject();
        obj.put(SdbConstants.FIELD_NAME_NAME, collectionFullName);

        AdminRequest request = new AdminRequest(AdminCommand.DROP_CL, obj);
        SdbReply response = sequoiadb.requestAndResponse(request);
        sequoiadb.throwIfError(response, collectionName);
        sequoiadb.removeCache(collectionFullName);
    }

    private void alterInternal(String taskName, BSONObject options, boolean allowNullArgs) throws BaseException {
        if (null == options && !allowNullArgs) {
            throw new BaseException(SDBError.SDB_INVALIDARG, "options is null");
        }
        BSONObject argumentObj = new BasicBSONObject();
        argumentObj.put(SdbConstants.FIELD_NAME_NAME, taskName);
        argumentObj.put(SdbConstants.FIELD_NAME_ARGS, options);
        BSONObject alterObject = new BasicBSONObject();
        alterObject.put(SdbConstants.FIELD_NAME_ALTER, argumentObj);
        alterCollectionSpace(alterObject);
    }

    /**
     * Alter the current collection space.
     *
     * @param options The options of the collection space:
     *                <ul>
     *                <li>Domain : the domain of collection space.
     *                <li>PageSize : page size of collection space.
     *                <li>LobPageSize : LOB page size of collection space.
     *                </ul>
     * @throws BaseException If error happens.
     */
    public void alterCollectionSpace(BSONObject options) throws BaseException {
        if (null == options) {
            throw new BaseException(SDBError.SDB_INVALIDARG, "options is null");
        }

        BSONObject newObj = new BasicBSONObject();
        if (!options.containsField(SdbConstants.FIELD_NAME_ALTER)) {
            newObj.put(SdbConstants.FIELD_NAME_NAME, name);
            newObj.put(SdbConstants.FIELD_NAME_OPTIONS, options);
        } else {
            Object tmpAlter = options.get(SdbConstants.FIELD_NAME_ALTER);
            if (tmpAlter instanceof BasicBSONObject ||
                tmpAlter instanceof BasicBSONList) {
                newObj.put(SdbConstants.FIELD_NAME_ALTER, tmpAlter);
            } else {
                throw new BaseException(SDBError.SDB_INVALIDARG, options.toString());
            }
            newObj.put(SdbConstants.FIELD_NAME_ALTER_TYPE, SdbConstants.SDB_ALTER_CS);
            newObj.put(SdbConstants.FIELD_NAME_VERSION, SdbConstants.SDB_ALTER_VERSION);
            newObj.put(SdbConstants.FIELD_NAME_NAME, name);

            if (options.containsField(SdbConstants.FIELD_NAME_OPTIONS)) {
                Object tmpOptions = options.get(SdbConstants.FIELD_NAME_OPTIONS);
                if (tmpOptions instanceof BasicBSONObject) {
                    newObj.put(SdbConstants.FIELD_NAME_OPTIONS, tmpOptions);
                } else {
                    throw new BaseException(SDBError.SDB_INVALIDARG, options.toString());
                }
            }
        }

        AdminRequest request = new AdminRequest(AdminCommand.ALTER_CS, newObj);
        SdbReply response = sequoiadb.requestAndResponse(request);
        sequoiadb.throwIfError(response);
    }

    /**
     * Alter the current collection space to set domain
     *
     * @param options The options of the collection space:
     *                <ul>
     *                <li>Domain : the domain of collection space.
     *                </ul>
     * @throws BaseException If error happens.
     */
    public void setDomain(BSONObject options) throws BaseException {
        alterInternal(SdbConstants.SDB_ALTER_SET_DOMAIN, options, false);
    }

    /**
     * Alter the current collection space to remove domain
     *
     * @throws BaseException If error happens.
     */
    public void removeDomain() throws BaseException {
        alterInternal(SdbConstants.SDB_ALTER_REMOVE_DOMAIN, null, true);
    }

    /**
     * Alter the current collection space to enable capped
     *
     * @throws BaseException If error happens.
     */
    public void enableCapped() throws BaseException {
        alterInternal(SdbConstants.SDB_ALTER_ENABLE_CAPPED, null, true);
    }

    /**
     * Alter the current collection space to disable capped
     *
     * @throws BaseException If error happens.
     */
    public void disableCapped() throws BaseException {
        alterInternal(SdbConstants.SDB_ALTER_DISABLE_CAPPED, null, true);
    }

    /**
     * Alter the current collection space to set attributes.
     *
     * @param options The options of the collection space:
     *                <ul>
     *                <li>Domain : the domain of collection space.
     *                <li>PageSize : page size of collection space.
     *                <li>LobPageSize : LOB page size of collection space.
     *                </ul>
     * @throws BaseException If error happens.
     */
    public void setAttributes(BSONObject options) throws BaseException {
        alterInternal(SdbConstants.SDB_ALTER_SET_ATTRIBUTES, options, false);
    }

   /**
     * @param oldName The old collection name
     * @param newName The new collection name
     * @throws BaseException If error happens.
     * 
     */
    public void renameCollection(String oldName, String newName) throws BaseException {
        if (oldName == null || oldName.length() == 0) {
            throw new BaseException(SDBError.SDB_INVALIDARG, "The old name of collection is null or empty");
        }
        if (newName == null || newName.length() == 0) {
            throw new BaseException(SDBError.SDB_INVALIDARG, "The new name of collection is null or empty");
        }

        BSONObject matcher = new BasicBSONObject();
        matcher.put(SdbConstants.FIELD_NAME_CELLECTIONSPACE, name);
        matcher.put(SdbConstants.FIELD_NAME_OLDNAME, oldName);
        matcher.put(SdbConstants.FIELD_NAME_NEWNAME, newName);

        AdminRequest request = new AdminRequest(AdminCommand.RENAME_CL, matcher);
        SdbReply response = sequoiadb.requestAndResponse(request);
        sequoiadb.throwIfError(response);
        sequoiadb.removeCache(name+"."+oldName);
        sequoiadb.upsertCache(name+"."+newName);
    }
}
