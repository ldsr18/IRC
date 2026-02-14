#include "channel.hpp"

Channel::Channel()
: _name(""),
  _limit(0),
  _inviteOnly(false),
  _topicRestricted(false),
  _hasKey(false),
  _hasLimit(false) {}

Channel::Channel(std::string name)
: _name(name),
  _limit(0),
  _inviteOnly(false),
  _topicRestricted(false),
  _hasKey(false),
  _hasLimit(false) {}

Channel::~Channel() {}

bool	Channel::hasMember(int fd) const
{ 
	return _members.find(fd) != _members.end();
}

bool	Channel::addMember(int fd)
{ 
	return _members.insert(fd).second; // _members.insert(fd) retourne paire(iterator, bool) bool sur si il a ete bien insert ou pas
}

bool	Channel::removeMember(int fd)
{ 
	size_t erasd = _members.erase(fd);
	_moderators.erase(fd);
	_invited.erase(fd);
	return erasd != 0;
}


bool	Channel::isModerator(int fd) const
{ 
	return _moderators.find(fd) != _moderators.end();
}

bool	Channel::addModerator(int fd)
{ 
	if(!hasMember(fd))
		return false;
	return _moderators.insert(fd).second;
}

bool	Channel::removeModerator(int fd)
{ 
	return _moderators.erase(fd) != 0;
}

//	Topic
std::string const& Channel::topic() const
{ 
	return _topic;
}

void	Channel::setTopic(const std::string& t)
{
	_topic = t;
}

bool 	Channel::isTopicRestricted() const {
	return _topicRestricted;
}

void 	Channel::setTopicRestricted(bool tr) {
	_topicRestricted = tr;
}

// Invites (+i)
bool	Channel::isInviteOnly() const
{ 
	return _inviteOnly;
}

void	Channel::setInviteOnly(bool io)
{ 
	_inviteOnly = io;
}

bool	Channel::isInvited(int fd) const
{ 
	return _invited.find(fd) != _invited.end();
}

void	Channel::invite(int fd)
{ 
	_invited.insert(fd);
}

void	Channel::uninvite(int fd)
{ 
	_invited.erase(fd);
}

//	Key (+k)
std::string const& Channel::getKEy() const {
	return _key;
}

bool	Channel::hasKey() const
{ 
	return _hasKey;
}

bool	Channel::checkKey(const std::string& key) const
{ 
	if(!_hasKey)
		return true;
	return _key == key;
}

void	Channel::setKey(const std::string& key)
{ 
	_key = key;
	_hasKey = true;
}

void	Channel::clearKey()
{ 
	_key.clear();
	_hasKey = false;
}

//	Limit (+l)
bool	Channel::hasLimit() const
{ 
	return _hasLimit;
}

size_t	Channel::limit() const
{ 
	return _limit;
}

void	Channel::setLimit(size_t n)
{ 
	_limit = n;
	_hasLimit = true;
}

void	Channel::clearLimit()
{ 
	_limit = 0;
	_hasLimit = false;
}

bool	Channel::isFull() const
{ 
	if(! _hasLimit)
		return false;
	return _members.size() >= _limit;
}

std::string const& Channel::name() const
{ 
	return _name;
}

std::set<int> const& Channel::members() const
{ 
	return _members;
}


size_t Channel::memberCount() const
{ 
	return _members.size();
}

bool Channel::empty() const
{ 
	return _members.empty();
}
