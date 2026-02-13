#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <string>
#include <cstddef>

class Channel {
	private :
		std::string		_name;
		std::set<int>	_members;
		std::set<int>	_moderators;
		std::set<int>	_invited; 		// invitations (fd)
		std::string		_topic;
		std::string		_key;			// +k
		size_t			_limit; 		// +l

		bool _inviteOnly; 		// +i
		bool _topicRestricted; 	// +t
		bool _hasKey;
		bool _hasLimit;


	public:

		Channel();
		Channel(std::string name);
		~Channel();


		bool	hasMember(int fd) const;
		bool	addMember(int fd);
		bool	removeMember(int fd);

		bool	isModerator(int fd) const;
		bool	addModerator(int fd);
		bool	removeModerator(int fd);

		//	Topic
		std::string const& topic() const;
		void	setTopic(const std::string&);

		// Invites (+i)
		bool	isInviteOnly() const;
		void	setInviteOnly(bool);
		bool	isInvited(int fd) const;
		void	invite(int fd);
		void	uninvite(int fd);

		//	Key (+k)
		bool	hasKey() const;
		bool	checkKey(const std::string& key) const;
		void	setKey(const std::string& key);
		void	clearKey();

		//	Limit (+l)
		bool	hasLimit() const;
		size_t	limit() const;
		void	setLimit(size_t n);
		void	clearLimit();
		bool	isFull() const;

		std::string const& name() const;
		std::set<int> const& members() const;

		size_t memberCount() const;
		bool empty() const;


};

#endif