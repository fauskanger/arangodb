Active Failover Architecture
============================

An _Active Failover_ is defined as:

- One ArangoDB Single-Server instance which is read / writable by clients called **Leader**
- One or more ArangoDB Single-Server instances, which are passive and not read or writable called **Followers**, which asynchronously replicate data from the master
- At least one _Agency_ acting as a "witness" to determine which server becomes the _leader_ in a _failure_ situation

**Note:** even though it is technically possible to start more than one _followers_ only one
_follower_ is currently officially supported. This limitation may be removed in
future releases.

![ArangoDB Active Failover](leader-follower.png)

The advantage of the _Active Failover_ compared to a traditional _Master-Slave_
setup is that there is an active third party, the _Agency_ which observes and supervises
all involved server processes. _Follower_ instances can rely on the _Agency_ to
determine the correct _leader_ server.

The _Active Failover_ setup is made **resilient** by the fact that all the official
ArangoDB drivers can automatically determine the correct _leader_ server and
redirect requests appropriately. Furthermore Foxx Services do also automatically
perform a failover: should the _leader_ instance fail (which is also the _Foxxmaster_)
the newly elected _leader_ will reinstall all Foxx services and resume executing
queued [Foxx tasks](../../Foxx/Reference/Scripts.md).
[Database users](../../Administration/ManagingUsers/README.md)
which were created on the _leader_ will also be valid on the newly elected _leader_
(always depending on the condition that they were synced already).

Consider the case for two *arangod* instances. The two servers are connected via
server wide (global) asynchronous replication. One of the servers is
elected _Leader_, and the other one is made a _Follower_ automatically. At startup,
the two servers race for the leadership position. This happens through the _agency
locking mechanism_ (which means that the _Agency_ needs to be available at server start).
You can control which server will become _Leader_ by starting it earlier than
other server instances in the beginning.

The _Follower_ will automatically start replication from the _Leader_ for all
available databases, using the server-level replication introduced in v. 3.3.

When the _Leader_ goes down, this is automatically detected by the _Agency_
instance, which is also started in this mode. This instance will make the
previous follower stop its replication and make it the new _Leader_.

The _Follower_ will deny all read and write requests from client applications.
Only the replication itself is allowed to access the follower's data until the
follower becomes a new _Leader_ (should a _failover_ happen).

When sending a request to read or write data on a _Follower_, the _Follower_ will
always respond with `HTTP 503 (Service unavailable)` and provide the address of
the current _Leader_. Client applications and drivers can use this information to
then make a follow-up request to the proper _Leader_:

```
HTTP/1.1 503 Service Unavailable
X-Arango-Endpoint: http://[::1]:8531
....
```

Client applications can also detect who the current _Leader_ and the _Followers_
are by calling the `/_api/cluster/endpoints` REST API. This API is accessible
on _Leader_ and _Followers_ alike.

The tool _ArangoDB Starter_ supports starting two servers with asynchronous
replication and failover [out of the box](../../Deployment/ActiveFailover/UsingTheStarter.md).

The _arangojs_ driver for JavaScript, the Go driver, the Java driver, ArangoJS and
the PHP driver support active failover in case the currently accessed server endpoint
responds with `HTTP 503`.
