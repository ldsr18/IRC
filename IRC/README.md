# ft_irc — Numerics & messages (cheat sheet complet)

> Format (rappel) : `:ircserv <code> <nick> <args...> :<texte>`
> Les placeholders : `<nick> <target> <channel> <command> <modes> <params>`

## 001–005 (welcome / infos)
- **001** : `<nick> :Welcome to the Internet Relay Network <nick>`
- **002** : `<nick> :Your host is <servername>, running version <version>`
- **003** : `<nick> :This server was created <datetime>`
- **004** : `<nick> <servername> <version> <available_user_modes> <available_channel_modes>`
- **005** : `<nick> <tokens...> :are supported by this server`

## 200–299 (server replies divers)
- **221** : `<nick> <user_modes>`
- **251** : `<nick> :There are <integer> users and <integer> services on <integer> servers`
- **252** : `<nick> <integer> :operator(s) online`
- **253** : `<nick> <integer> :unknown connection(s)`
- **254** : `<nick> <integer> :channels formed`
- **255** : `<nick> :I have <integer> clients and <integer> servers`
- **265** : `<nick> :Current local users <integer>, max <integer>`
- **266** : `<nick> :Current global users <integer>, max <integer>`

## 300–399 (away / whois / etc.)
- **301** : `<nick> <target> :<away_message>`
- **311** : `<nick> <target> <user> <host> * :<realname>`
- **312** : `<nick> <target> <server> :<server_info>`
- **313** : `<nick> <target> :is an IRC operator`
- **317** : `<nick> <target> <seconds_idle> <signon_time> :seconds idle, signon time`
- **318** : `<nick> <target> :End of /WHOIS list`
- **319** : `<nick> <target> :<channels>`

## 320–399 (liste / stats — rarement nécessaire)
- **322** : `<nick> <channel> <#visible> :<topic>`
- **323** : `<nick> :End of /LIST`

## 330–399 (topic / names)
- **331** : `<nick> <channel> :No topic is set`
- **332** : `<nick> <channel> :<topic>`
- **333** : `<nick> <channel> <setter> <timestamp>`
- **341** : `<nick> <target> <channel>`
- **353** : `<nick> = <channel> :<names>`
- **366** : `<nick> <channel> :End of /NAMES list`

## 400–499 (errors)
- **401** : `<nick> <target> :No such nick`
- **402** : `<nick> <server> :No such server`
- **403** : `<nick> <channel> :No such channel`
- **404** : `<nick> <channel> :Cannot send to channel`
- **405** : `<nick> <channel> :You have joined too many channels`
- **407** : `<nick> <target> :Too many targets`
- **409** : `<nick> :No origin specified`
- **411** : `<nick> :No recipient given (<command>)`
- **412** : `<nick> :No text to send`
- **421** : `<nick> <command> :Unknown command`
- **431** : `<nick> :No nickname given`
- **432** : `<nick> <nick> :Erroneous nickname`
- **433** : `<nick> <nick> :Nickname is already in use`
- **436** : `<nick> <nick> :Nickname collision`
- **441** : `<nick> <target> <channel> :They aren't on that channel`
- **442** : `<nick> <channel> :You're not on that channel`
- **443** : `<nick> <target> <channel> :is already on channel`
- **451** : `<nick> :You have not registered`
- **461** : `<nick> <command> :Not enough parameters`
- **462** : `<nick> :You may not reregister`
- **464** : `<nick> :Password incorrect`
- **467** : `<nick> <channel> :Channel key already set`
- **471** : `<nick> <channel> :Cannot join channel (+l)`
- **472** : `<nick> <char> :is unknown mode char to me`
- **473** : `<nick> <channel> :Cannot join channel (+i)`
- **474** : `<nick> <channel> :Cannot join channel (+b)`
- **475** : `<nick> <channel> :Cannot join channel (+k)`
- **476** : `<nick> <channel> :Bad Channel Mask`
- **477** : `<nick> <channel> :Channel doesn't support modes`
- **478** : `<nick> <channel> <banmask> :Ban list is full`
- **481** : `<nick> :Permission Denied- You're not an IRC operator`
- **482** : `<nick> <channel> :You're not channel operator`
- **483** : `<nick> :You can't kill a server!`
- **484** : `<nick> :Your connection is restricted!`
- **485** : `<nick> :You're not the original channel operator`
- **491** : `<nick> :No O-lines for your host`

## 500–599 (errors divers)
- **501** : `<nick> :Unknown MODE flag`
- **502** : `<nick> :Cannot change mode for other users`

---
## Notes pratiques (placeholders)
- `<nick>` = nick du client qui reçoit la numeric
- `<target>` = nick visé (INVITE/KICK/WHOIS/etc.)
- `<channel>` = `#channel`
- `<command>` = `JOIN`, `KICK`, `INVITE`, `MODE`, `PRIVMSG`, etc.
