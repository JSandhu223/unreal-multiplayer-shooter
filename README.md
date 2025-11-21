# Multiplayer Shooter

A third person multiplayer shooter created in Unreal Engine 5.6, utilizing Unreal's
multiplayer features and integrating with Steam servers to allow online play.

## Unreal Multiplayer Features

### Replication Overview

Replication allows the server to propagate an actor's variable changes from the server to
clients. Class variables can be set to be replicated as follows:

1. In the actor's constructor, set `bReplicates = true`.
2. Set the variable's UPROPERTY to `Replicated` or `ReplicatedUsing` (if using rep notifies).
3. Override `AActor::GetLifetimeReplicatedProps` and register the variable to be replicated
via the `DOREPLIFETIME` macro.

Note that replication only works one way, which means that variables cannot be replicated
from a client to the server.

### RepNotify

Rep notifies are functions that get called when a variable gets replicated. A replicating variable
can be tied to a rep notify with `UPROPERTY(ReplicatedUsing = OnRep_VARIABLENAME)`.

### Remote Procedural Call (RPC)

RPCs are functions that get called locally and executed on another machine. There are different
types of RPCs.

- **Server RPC**: called on the client and executed on the server
- **Client RPC**: called on the server and executed on the client
- **Remote RPC**: executed on the *remote* side of the connection, which can be either
the server or client. Never executed on the local side
- **NetMulticast RPC**: executed on the server and all connected clients of the relevant actor
