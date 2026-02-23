*This project has been created as part of the 42 curriculum by jdecarro and mbendidi.*

# ft_irc

Implementation of an IRC server in C++98 as part of the 42 curriculum.

## Description

The goal of this project is to implement an Internet Relay Chat (IRC) server following the IRC protocol specifications described in RFC 1459 and RFC 2812.

The server handles multiple clients simultaneously using non-blocking sockets and the poll() system call. It reproduces the core behavior of a real IRC server and can be tested using standard IRC clients such as irssi, as well as raw TCP clients such as netcat.

Constraints:
- C++98 standard
- No external libraries
- No threads
- poll() (or equivalent) for multiplexing
- Proper handling of partial reads/writes and CRLF terminated messages

# Instructions

## Compilation

```bash
make
```

This generates the executable:

```
ircserv
```

## Usage

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 42pass
```

Arguments:
- port: TCP port on which the server listens
- password: connection password required by clients

## Connecting and Testing

Two common ways to test the server are:
- Using irssi (recommended for evaluation)
- Using netcat (useful to send raw IRC lines)

## Testing with irssi

Start irssi:

```bash
irssi
```

Connect to the server:

```
/connect -password 42pass 127.0.0.1 6667
```

irssi provides user-friendly commands starting with `/`. These commands are translated into IRC protocol messages sent to the server.

Common tests with irssi:

- Join a channel:
  - `/join #channel`

- Send a message in a channel:
  - Type directly in the channel window:
    - `hello everyone`

- Send a private message:
  - `/msg nick hello`

- Kick a user:
  - `/kick #channel nick :reason`

- Invite a user:
  - `/invite nick #channel`

- Change or view the topic:
  - `/topic #channel new topic`
  - `/topic #channel`

- Change channel modes:
  - `/mode #channel +i`
  - `/mode #channel +t`
  - `/mode #channel +k key`
  - `/mode #channel -k`
  - `/mode #channel +l 10`
  - `/mode #channel -l`
  - `/mode #channel +o nick`
  - `/mode #channel -o nick`

Useful irssi help:
- `/help`
- `/help kick`
- `/help mode`
- `/help invite`
- `/help topic`
- `/help window`

Full irssi command list:
- https://irssi.org/documentation/help/
- or run `/help` inside irssi

Note:
- If you type `KICK #channel nick` without `/`, irssi will treat it as a normal message and send it as a PRIVMSG.
- To send a raw IRC command line from irssi, use:
  - `/quote KICK #channel nick :reason`

## Testing with netcat (nc)

Connect using:

```bash
nc -C 127.0.0.1 6667
```

Then register manually:

```
PASS 42pass
NICK simo
USER simo 0 * :simo
```

Join a channel:

```
JOIN #test
```

Send messages:

```
PRIVMSG #test :hello everyone
PRIVMSG nick :hello private
```

Kick / invite / topic / mode:

```
INVITE otherNick #test
KICK #test otherNick :reason
TOPIC #test :new topic
MODE #test +i
MODE #test +o otherNick
```

Important:
- IRC messages are terminated by CRLF. Using `nc -C` helps ensure correct line endings.
- If PASS/NICK/USER are not completed, the server should reply "You have not registered" for commands that require registration.

## Implemented Commands

Authentication:
- PASS
- NICK
- USER

Core:
- PING
- PONG
- PRIVMSG
- NOTICE

Channels:
- JOIN
- PART
- QUIT
- TOPIC
- INVITE
- KICK
- MODE

Supported channel modes:
- +i : Invite-only channel
- +t : Topic modification restricted to channel operators
- +k : Channel password
- +l : User limit
- +o : Operator privilege

## Bonus: IRC Bot

The repository also includes an IRC bot compiled with `make`.

### Usage

```bash
./mybot <addr_ip> <port> <password>
```

Example:

```bash
./mybot 127.0.0.1 6667 42pass
```

Once started, the bot connects to the server with the nickname `MyBot`.

### Talking to the bot

The bot reacts to commands sent as private messages.

Using netcat:

```
PRIVMSG MyBot :!help
PRIVMSG MyBot :!hello
PRIVMSG MyBot :!joke
```

Using irssi:

```
/msg MyBot !help
/msg MyBot !hello
/msg MyBot !joke
```

### Bot commands

- `!help`  : display available bot commands
- `!hello` : the bot replies to confirm it is connected
- `!joke`  : the bot sends a random joke

## Architecture Overview

Main components:

Server:
- Listening socket and poll() loop
- Accepts connections
- Reads client messages
- Dispatches parsed commands

Client:
- Stores client state and registration
- Stores nick/user
- Maintains input buffer

Channel:
- Stores members and operators
- Stores modes and parameters
- Handles broadcasting

Command handler:
- Parses incoming commands
- Validates parameters
- Applies logic
- Sends numeric replies

## Network Handling

The server:
- Uses non-blocking sockets
- Uses poll() for multiplexing
- Maintains per-client input buffers
- Handles partial reads
- Processes CRLF terminated messages ("\r\n")
- Properly removes disconnected clients

## Error Handling

The server returns appropriate IRC numeric replies when required, including:

- 401 : No such nick
- 403 : No such channel
- 421 : Unknown command
- 442 : You're not on that channel
- 443 : User already on channel
- 461 : Not enough parameters
- 473 : Invite-only channel
- 475 : Bad channel key
- 482 : You're not channel operator

# Resources

## Protocol documentation

- RFC 1459 — Internet Relay Chat Protocol
- RFC 2812 — Internet Relay Chat: Client Protocol

## IRC Client

- irssi - https://irssi.org
- netcat (nc) - TCP testing tool

## AI usage

AI tools were used for:

- Documentation
- Helping structuring files

All generated suggestions were manually reviewed, understood, and adapted before being integrated into the project.