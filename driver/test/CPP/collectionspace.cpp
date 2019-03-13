#include <stdio.h>
#include <gtest/gtest.h>
#include "client.hpp"
#include "testcommon.hpp"
#include <string>
#include <iostream>

using namespace std ;
using namespace sdbclient ;

//#define USER                  "SequoiaDB"
//#define PASSWD                "SequoiaDB"

#define NUM_RECORD             5

TEST(collectionspace, not_connect)
{
   sdbCollectionSpace cs ;
   sdbCollection cl ;
   INT32 rc = SDB_OK ;
   rc = cs.getCollection( COLLECTION_NAME, cl ) ;
   ASSERT_EQ( SDB_NOT_CONNECTED, rc ) ;
}

TEST(collectionspace,getCollection)
{
   sdb connection ;
   sdbCollectionSpace cs ;
   sdbCollection cl ;
   // initialize local variables
   const CHAR *pHostName                    = HOST ;
   const CHAR *pPort                        = SERVER ;
   const CHAR *pUsr                         = USER ;
   const CHAR *pPasswd                      = PASSWD ;
   INT32 rc                                 = SDB_OK ;
   const CHAR *clName                       = NULL ;
   // initialize the work environment
   rc = initEnv() ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // connect to database
   rc = connection.connect( pHostName, pPort, pUsr, pPasswd ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get cs
   rc = getCollectionSpace( connection, COLLECTION_SPACE_NAME, cs ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get collection
   rc = cs.getCollection( COLLECTION_NAME, cl ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get the cl name
   clName = cl.getCollectionName() ;
   cout<<"The cl we got is : "<<clName<<endl ;
   // disconnect the connection
   connection.disconnect() ;
}

TEST(collectionspace,createCollection_without_Sharding_and_replSize)
{
   sdb connection ;
   sdbCollectionSpace cs ;
   sdbCollection cl ;
   // initialize local variables
   const CHAR *pHostName                    = HOST ;
   const CHAR *pPort                        = SERVER ;
   const CHAR *pUsr                         = USER ;
   const CHAR *pPasswd                      = PASSWD ;
   INT32 rc                                 = SDB_OK ;
   const CHAR *clName                       = NULL ;
   // initialize the work environment
   rc = initEnv() ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // connect to database
   rc = connection.connect( pHostName, pPort, pUsr, pPasswd ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get cs
   rc = getCollectionSpace( connection, COLLECTION_SPACE_NAME, cs ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // delete the cl named "testbar", first
   rc = cs.dropCollection( COLLECTION_NAME ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get collection
   rc = cs.createCollection( COLLECTION_NAME, cl ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get the cl name
   clName = cl.getCollectionName() ;
   cout<<"The cl we created is : "<<clName<<endl ;
   // disconnect the connection
   connection.disconnect() ;
}

TEST(collectionspace,dropCollection)
{
   sdb connection ;
   sdbCollectionSpace cs ;
   sdbCollection cl ;
   sdbCollection cl1 ;

   // initialize local variables
   const CHAR *pHostName                    = HOST ;
   const CHAR *pPort                        = SERVER ;
   const CHAR *pUsr                         = USER ;
   const CHAR *pPasswd                      = PASSWD ;
   INT32 rc                                 = SDB_OK ;
   const CHAR *clName                       = NULL ;
   // initialize the work environment
   rc = initEnv() ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // connect to database
   rc = connection.connect( pHostName, pPort, pUsr, pPasswd ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get cs
   rc = getCollectionSpace( connection, COLLECTION_SPACE_NAME, cs ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get cl
   rc = cs.getCollection( COLLECTION_NAME, cl ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get the cl name
   clName = cl.getCollectionName() ;
   cout<<"Before drop, the exist cl is : "<<clName<<endl ;
   // delete the cl named "testbar", first
   rc = cs.dropCollection( COLLECTION_NAME ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get the cl name again
   rc = cs.getCollection( COLLECTION_NAME, cl1 ) ;
   ASSERT_EQ( SDB_DMS_NOTEXIST, rc ) ;
   cout<<"After drop, the cl named \"testbar\" does not exist."<<endl ;

   // disconnect the connection
   connection.disconnect() ;
}
/*
TEST(collectionspace,create)
{
   sdb connection ;
   sdbCollectionSpace cs ;
   sdbCollection cl ;
   sdbCollection cl1 ;

   // initialize local variables
   const CHAR *pHostName                    = HOST ;
   const CHAR *pPort                        = SERVER ;
   const CHAR *pUsr                         = USER ;
   const CHAR *pPasswd                      = PASSWD ;
   INT32 rc                                 = SDB_OK ;
   const CHAR *clName                       = NULL ;
   // initialize the work environment
   initEnv() ;
   // connect to database
   rc = connection.connect( pHostName, pPort, pUsr, pPasswd ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get cs
   rc = getCollectionSpace( connection, COLLECTION_SPACE_NAME, cs ) ;
   ASSERT_EQ( SDB_OK, rc ) ;

   ASSERT_TRUE( 1==0 ) ;
   // disconnect the connection
   connection.disconnect() ;
}
*/

/*
TEST(collectionspace,drop)
{
   sdb connection ;
   sdbCollectionSpace cs ;
   sdbCollection cl ;
   sdbCollection cl1 ;

   // initialize local variables
   const CHAR *pHostName                    = HOST ;
   const CHAR *pPort                        = SERVER ;
   const CHAR *pUsr                         = USER ;
   const CHAR *pPasswd                      = PASSWD ;
   INT32 rc                                 = SDB_OK ;
   const CHAR *clName                       = NULL ;
   // initialize the work environment
   initEnv() ;
   // connect to database
   rc = connection.connect( pHostName, pPort, pUsr, pPasswd ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get cs
   rc = getCollectionSpace( connection, COLLECTION_SPACE_NAME, cs ) ;
   ASSERT_EQ( SDB_OK, rc ) ;

   ASSERT_TRUE( 1==0 ) ;
   // disconnect the connection
   connection.disconnect() ;
}
*/

/*
TEST(collectionspace,createCollection_with_Sharding_and_replSize)
{
   sdb connection ;
   sdbCollectionSpace cs ;
   sdbCollection cl ;
   // initialize local variables
   const CHAR *pHostName                    = HOST ;
   const CHAR *pPort                        = SERVER ;
   const CHAR *pUsr                         = USER ;
   const CHAR *pPasswd                      = PASSWD ;
   INT32 rc                                 = SDB_OK ;
   const CHAR *clName                       = NULL ;
   // initialize the work environment
   initEnv() ;
   // connect to database
   rc = connection.connect( pHostName, pPort, pUsr, pPasswd ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get cs
   rc = getCollectionSpace( connection, COLLECTION_SPACE_NAME, cs ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get the cl name
//   clName = cl.getCollectionName() ;
//   cout<<"The cl we got is : "<<clName<<endl ;
   ASSERT_TRUE( 0==1 ) ;
   // disconnect the connection
   connection.disconnect() ;
}

*/


// TODO:

/*
create //deprecated
drop   //deprecated
getCSName


*/
