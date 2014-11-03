/*
 *	RPL: IPv6 Routing Protocol for Low-Power and Lossy Networks
 *	Linux RPL implementation
 *
 *	Authors:
 *	Siemens AG (ieee802154)
+ *	Joao Pedro Taveira	<joao.silva@inov.pt>
 *
 *	This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License.
 */

#ifndef NL_RPL_LOCAL_H
#define NL_RPL_LOCAL_H

int __init rpl_nl_init(void);
void __exit rpl_nl_exit(void);

#define RPL_OP(_cmd, _func)			\
	{						\
		.cmd	= _cmd,				\
		.policy	= rpl_policy,		\
		.doit	= _func,			\
		.dumpit	= NULL,				\
		.flags	= GENL_ADMIN_PERM,		\
	}

#define RPL_DUMP(_cmd, _func, _dump)		\
	{						\
		.cmd	= _cmd,				\
		.policy	= rpl_policy,		\
		.doit	= _func,			\
		.dumpit	= _dump,			\
	}

struct genl_info;

struct sk_buff *rpl_nl_create(int flags, u8 req);
int rpl_nl_mcast(struct sk_buff *msg, unsigned int group);
struct sk_buff *rpl_nl_new_reply(struct genl_info *info,
		int flags, u8 req);
int rpl_nl_reply(struct sk_buff *msg, struct genl_info *info);

extern struct genl_family nlrpl_family;
int nlrpl_dag_conf_register(void);
int nlrpl_dag_info_register(void);
int nlrpl_dag_mng_register(void);

#endif /* NL_RPL_LOCAL_H */
