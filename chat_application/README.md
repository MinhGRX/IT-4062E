# Chat Application - IT-4062E Network Programming

A multithreaded TCP-based chat application with PostgreSQL backend, featuring user authentication, friend management, and group chat capabilities.

## Project Overview

This chat application is built using C with a PostgreSQL database and provides a full-featured messaging platform with:
- User registration and authentication with PostgreSQL storage
- Real-time friend status tracking
- One-to-one messaging with offline delivery
- Group chat functionality with notifications
- Colored activity logging (ERROR/WARNING/INFO/DEBUG)
- Service layer architecture for clean code organization

## Features

### 1. System Foundation (6 points)
- ✅ Stream handling and line-framed protocol
- ✅ Socket I/O mechanism on server (multithreaded)
- ✅ User registration and account management (PostgreSQL)
- ✅ Activity logging with colored output (ERROR/WARNING/INFO/DEBUG)

### 2. User Session & Friends (6 points)
- ✅ Login and session management
- ✅ Send friend requests
- ✅ Accept/decline friend requests
- ✅ Remove friends
- ✅ Get friends list and online status

### 3. Messaging (2 points)
- ✅ Send/receive messages between two users
- ✅ Offline message delivery

### 4. Connection Management (1 point)
- ✅ Disconnect handling and notifications

### 5. Group Chat (5 points)
- ✅ Create group chats
- ✅ Add users to group chat (with friendship validation)
- ✅ Remove users from group chat (owner-only)
- ✅ Leave group chat (member-only)
- ✅ Send/receive group messages with broadcast


## Architecture

### Server
- **Multithreaded TCP Server**: Each client connection is handled in a separate thread
- **Line-based Protocol**: Commands are newline-delimited for reliable parsing
- **PostgreSQL Database**: User accounts, messages, groups, and relationships stored in PostgreSQL
- **Service Layer**: Clean separation of controllers, services, and DAOs
- **Colored Logging**: Console output with color-coded levels (ERROR/WARNING/INFO/DEBUG), plain text in log files
- **Activity Logging**: All actions logged to `./logs/server.log`
- **Thread Safety**: Mutex protection for online user tracking and logging
- **Online User Tracking**: Real-time tracking of connected users in `server.c`

### Client
- **Simple TCP Client**: Connects to server and sends commands
- **Interactive CLI**: Command-line interface for user interaction
- **Stream Buffering**: Handles partial receives correctly

### Database Schema
- **User**: User accounts with credentials
- **Friend**: Friend relationships and requests
- **GroupChat**: Group information with owner tracking
- **GroupMember**: Group membership with roles
- **GroupMessage**: Group chat message history
- **Message**: Direct messages between users


### Commands

#### Authentication
- `REGISTER <username> <password>` - Register a new user
- `LOGIN <username> <password>` - Login with credentials
- `LOGOUT` - Logout current user

#### Friend Management
- `SEND_FRIEND_REQUEST <username>` - Send friend request to user
- `ACCEPT_FRIEND <username>` - Accept friend request from user
- `DECLINE_FRIEND <username>` - Decline friend request from user
- `REMOVE_FRIEND <username>` - Remove existing friend
- `LIST_FRIENDS` - Get list of friends with online status
- `LIST_REQUESTS` - Get list of pending friend requests

#### Messaging
- `SEND <username> <message>` - Send message to user (offline delivery supported)
- `CHAT <username>` - Start chat session with user
- `STOP_CHAT` - End current chat session

#### Group Chat
- `CREATE_GROUP <group_name>` - Create a new group (creator becomes owner)
- `ADD_MEMBER <group_id> <username>` - Add friend to group (must be friends)
- `REMOVE_MEMBER <group_id> <username>` - Remove member from group (owner only)
- `LEAVE_GROUP <group_id>` - Leave group (members only, owners cannot leave)
- `SEND_GROUP <group_id> <message>` - Send message to group
- `GROUP_HISTORY <group_id>` - View last 50 messages in group
- `GROUP_MEMBERS <group_id>` - View all members in group
- `LIST_GROUPS` - List all groups you're a member of

#### Testing
- `PING` - Test server connectivity (responds with PONG)

### Responses
- `OK <message>` - Command successful
- `ERR <message>` - Command failed
- `NOTIFY <message>` - Asynchronous notification from server
- `GROUP_MSG [group_id] sender: message` - Group message broadcast


### Prerequisites
- GCC compiler
- POSIX-compliant system (Linux/Unix)
- pthread library
- PostgreSQL database server (version 12+)
- libpq-dev (PostgreSQL C library)

### Compile All
```bash
make all
```

### Compile Server Only
```bash
make server
```

### Compile Client Only
```bash
make client
```

### Clean Build Artifacts
```bash
make clean
```

## Usage

### Database Setup

1. Install PostgreSQL:
```bash
sudo apt-get install postgresql postgresql-contrib libpq-dev
```

2. Create database and tables:
```bash
sudo -u postgres psql
create database chat_db;
\c chat_db
\i server/database/chat.sql
\q
```

3. Configure database connection:
```bash
cd server/database
# Edit db.conf with your PostgreSQL credentials
cat db.conf
# host=localhost
# port=5432
# dbname=chat_db
# user=postgres
# password=your_password
```

### Start Server
```bash
cd server
./server localhost 8023
```

The server will:
1. Load database configuration from `database/db.conf`
2. Connect to PostgreSQL database
3. Reset all users to offline status
4. Create `logs/` directory if it doesn't exist
5. Start listening on the specified address and port
6. Accept multiple client connections (multithreaded)
7. Log all activities with colored output to console and `logs/server.log`

### Connect Client
```bash
cd client
./client localhost 8023
```

### Example Session

#### Basic Authentication
```
> REGISTER alice secret123
OK User registered

> LOGIN alice secret123
OK LOGIN successful

> PING
PONG

> LOGOUT
OK LOGOUT successful

> /quit
[CLIENT] Exiting.
```

#### Friend Management
```
> LOGIN alice secret123
OK LOGIN successful

> SEND_FRIEND_REQUEST bob
OK Friend request sent to bob

> LIST_REQUESTS
# (In Bob's session)
> ACCEPT_FRIEND alice
OK Friend request from alice accepted

> LIST_FRIENDS
FRIEND bob online
END_FRIENDS
```

#### Group Chat
```
> CREATE_GROUP ProjectTeam
OK Group created successfully (ID:1)

> ADD_MEMBER 1 bob
OK Added bob to group 1
# Bob receives: NOTIFY You have been added to group (ID: 1) by alice

> SEND_GROUP 1 Hello team!
OK Message sent to 1 online member(s)
# Bob receives: GROUP_MSG [1] alice: Hello team!

> GROUP_HISTORY 1
[2026-01-04 18:10:00] alice: Hello team!
END_HISTORY

> GROUP_MEMBERS 1
MEMBER alice owner
MEMBER bob member
END_MEMBERS

> LEAVE_GROUP 1
OK You have left group 1
```

## Development

### Implementation Status

**Completed:**
- System Foundation (6/6 points)
  - Stream handling and line-framed protocol
  - Multithreaded socket I/O
  - PostgreSQL user registration and management
  - Colored activity logging (ERROR/WARNING/INFO/DEBUG)
- User Session & Friends (6/6 points)
  - Login and session management
  - Send/accept/decline friend requests
  - Remove friends
  - List friends with online status
- Messaging (2/2 points)
  - Direct messaging between users
  - Offline message delivery
- Connection Management (1/1 point)
  - Disconnect handling with notifications
- Group Chat (5/5 points)
  - Create groups with owner tracking
  - Add members (friendship validation)
  - Remove members (owner-only)
  - Leave group (member-only)
  - Send/receive group messages with broadcast

### Architecture Highlights

**Service Layer Pattern:**
```
Controller → Service → DAO → Database
```
- **Controllers** (`auth_controller.c`, `chat_controller.c`, `group_controller.c`): Handle routing and input parsing
- **Services** (`user_service.c`, `group_service.c`, `log_service.c`): Business logic and validation
- **DAOs** (`user_dao.c`, `friend_dao.c`, `group_dao.c`, `chat_dao.c`): Database operations

**Key Components:**
- `server.c`: Main server, online user tracking (`get_user_index_by_fd`, `notify_user`)
- `log_service.c`: Colored logging with thread-safe file/console output
- `group_service.c`: Group chat business logic with notifications
- `globals.h`: Shared state declarations (online users, mutexes)

## Testing

Comprehensive test documentation available in [TEST_RESULTS.md](TEST_RESULTS.md).

### Quick Test Scenarios

#### 1. System Foundation Test
```bash
# Terminal 1: Start server
cd server && ./server localhost 8023

# Terminal 2: Connect client
cd client && ./client localhost 8023
> REGISTER alice secret123
> LOGIN alice secret123
> LOGOUT
```

#### 2. Friend Management Test
```bash
# Terminal 1: Alice
> LOGIN alice secret123
> SEND_FRIEND_REQUEST bob

# Terminal 2: Bob
> LOGIN bob secret123
> LIST_REQUESTS
> ACCEPT_FRIEND alice
> LIST_FRIENDS
```

#### 3. Group Chat Test
```bash
# Terminal 1: Alice (creator)
> LOGIN alice secret123
> CREATE_GROUP TestGroup
> ADD_MEMBER 1 bob
> SEND_GROUP 1 Hello!

# Terminal 2: Bob (member)
> LOGIN bob secret123
# Receives: NOTIFY You have been added to group (ID: 1) by alice
# Receives: GROUP_MSG [1] alice: Hello!
> SEND_GROUP 1 Hi Alice!
> GROUP_HISTORY 1
> LEAVE_GROUP 1
```

### Check Logs
```bash
cat server/logs/server.log
```

**Colored Output Example:**
- `[ERROR]` messages in **Red**
- `[WARNING]` messages in **Yellow**
- `[INFO]` messages in **Green**
- `[DEBUG]` messages in **Blue**

### Verify Database
```bash
sudo -u postgres psql -d chat_db
\dt  # List tables
select * from "User";
select * from "Friend";
select * from "GroupChat";
```

## Contributing

This is a university project for IT-4062E Network Programming course.

### Team Members
Ngo Anh Tu - 20226005
Dang Huu Tuan Minh - 20210609

## Troubleshooting

### Port Already in Use
```bash
# Kill process using port 8023
lsof -ti:8023 | xargs kill -9
```

### Database Connection Failed
```bash
# Check PostgreSQL is running
sudo systemctl status postgresql

# Start PostgreSQL if stopped
sudo systemctl start postgresql

# Verify database exists
sudo -u postgres psql -l | grep chat_db

# Check db.conf credentials
cat server/database/db.conf
```

### Permission Denied (logs directory)
```bash
# The server creates logs/ automatically
# If issues persist, manually create:
cd server && mkdir -p logs
chmod 755 logs
```

### Compilation Errors
```bash
# Install required dependencies
sudo apt-get install build-essential libpq-dev postgresql-server-dev-all

# Clean and rebuild
cd server && make clean && make
```

### Colored Logs Not Showing
```bash
# Colors only appear in console output
# Log files contain plain text (no ANSI codes)
# Ensure your terminal supports ANSI color codes
```

### Group Chat Issues
```bash
# Check friendship before adding to group
# Only friends can be added to groups

# Verify membership
sudo -u postgres psql -d chat_db
select * from "GroupMember" where "groupId" = 1;

# Check logs for errors
tail -50 server/logs/server.log | grep ERROR
```
