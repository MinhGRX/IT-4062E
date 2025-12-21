# Chat Application - IT-4062E Network Programming

A multithreaded TCP-based chat application with user authentication, friend management, and group chat capabilities.

## Project Overview

This chat application is built using C and provides a full-featured messaging platform with:
- User registration and authentication
- Real-time friend status tracking
- One-to-one messaging
- Group chat functionality
- Activity logging
- Offline message delivery

## Features

### 1. System Foundation (6 points)
- ✅ Stream handling and line-framed protocol
- ✅ Socket I/O mechanism on server (multithreaded)
- ✅ User registration and account management
- ✅ Activity logging

### 2. User Session & Friends (6 points)
- ✅ Login and session management
- 🔄 Send friend requests
- 🔄 Accept/decline friend requests
- 🔄 Remove friends
- 🔄 Get friends list and online status

### 3. Messaging (2 points)
- 🔄 Send/receive messages between two users
- 🔄 Offline message delivery

### 4. Connection Management (1 point)
- 🔄 Disconnect handling and notifications

### 5. Group Chat (5 points)
- 🔄 Create group chats
- 🔄 Add users to group chat
- 🔄 Remove users from group chat
- 🔄 Leave group chat
- 🔄 Send/receive group messages


## Architecture

### Server
- **Multithreaded TCP Server**: Each client connection is handled in a separate thread
- **Line-based Protocol**: Commands are newline-delimited for reliable parsing
- **File-based Storage**: User accounts stored in `./data/users.txt`
- **Activity Logging**: All actions logged to `./logs/server.log`
- **Thread Safety**: Mutex protection for file operations

### Client
- **Simple TCP Client**: Connects to server and sends commands
- **Interactive CLI**: Command-line interface for user interaction
- **Stream Buffering**: Handles partial receives correctly


#### Authentication
- `REGISTER <username> <password>` - Register a new user
- `LOGIN <username> <password>` - Login with credentials
- `LOGOUT` - Logout current user

#### Testing
- `PING` - Test server connectivity (responds with PONG)

### Responses
- `OK <message>` - Command successful
- `ERR <message>` - Command failed


### Prerequisites
- GCC compiler
- POSIX-compliant system (Linux/Unix)
- pthread library

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

### Start Server
```bash
cd server
./server localhost 8023
```

The server will:
1. Create `data/` and `logs/` directories if they don't exist
2. Start listening on the specified address and port
3. Accept multiple client connections (multithreaded)
4. Log all activities to `logs/server.log`

### Connect Client
```bash
cd client
./client localhost 8023
```

### Example Session
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

## Development

### Current Implementation Status

**Completed:**
- Basic TCP server/client framework
- Multithreaded connection handling
- User registration and login
- Password verification
- Activity logging
- Line-based command protocol

**In Progress:**
- Friend management system
- Online status tracking
- Direct messaging
- Offline messages
- Group chat functionality

## Testing

### Register and Login Test
```bash
# Terminal 1: Start server
cd server && ./server localhost 8023

# Terminal 2: Connect client
cd client && ./client localhost 8023
> REGISTER testuser password123
> LOGIN testuser password123
> LOGOUT
```

### Check Logs
```bash
cat logs/server.log
```

### Verify User Database
```bash
cat data/users.txt
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

### Permission Denied (data/logs directories)
```bash
# The server creates these automatically with proper permissions
# If issues persist, manually create:
mkdir -p data logs
chmod 755 data logs
```

### Compilation Errors
```bash
# Ensure you have gcc and pthread
sudo apt-get install build-essential
```
