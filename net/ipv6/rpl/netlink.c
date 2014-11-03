/*
 *  Netlink inteface
 *	RPL: IPv6 Routing Protocol for Low-Power and Lossy Networks
 *	Linux RPL implementation
 *
 *	Authors:
 *  Sergey Lapin <slapin@ossfans.org>
 *  Dmitry Eremin-Solenikov <dbaryshkov@gmail.com>
 *  Maxim Osipov <maxim.osipov@siemens.com>
 *	Joao Pedro Taveira	<joao.silva@inov.pt>
 *
 *	This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License.
 */

#include <linux/kernel.h>
#include <linux/gfp.h>
#include <net/genetlink.h>
#include <linux/rpl_nl.h>

#include "nlrpl.h"

#include <net/rpl/rpl_debug.h>

static unsigned int rpl_seq_num;
static DEFINE_SPINLOCK(rpl_seq_lock);

struct genl_family nlrpl_family = {
	.id		= GENL_ID_GENERATE,
	.hdrsize	= 0,
	.name		= RPL_NL_NAME,
	.version	= 1,
	.maxattr	= RPL_ATTR_MAX,
	.netnsok	= true
};

/* Requests to userspace */
struct sk_buff *rpl_nl_create(int flags, u8 req)
{
	void *hdr;
	struct sk_buff *msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_ATOMIC);
	unsigned long f;

	if (!msg)
		return NULL;

	spin_lock_irqsave(&rpl_seq_lock, f);
	hdr = genlmsg_put(msg, 0, rpl_seq_num++,
			&nlrpl_family, flags, req);
	spin_unlock_irqrestore(&rpl_seq_lock, f);
	if (!hdr) {
		nlmsg_free(msg);
		return NULL;
	}

	return msg;
}

int rpl_nl_mcast(struct sk_buff *msg, unsigned int group)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	void *hdr = genlmsg_data(nlmsg_data(nlh));

	if (genlmsg_end(msg, hdr) < 0)
		goto out;

	return genlmsg_multicast(msg, 0, group, GFP_ATOMIC);
out:
	nlmsg_free(msg);
	return -ENOBUFS;
}

struct sk_buff *rpl_nl_new_reply(struct genl_info *info,
		int flags, u8 req)
{
	void *hdr;
	struct sk_buff *msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_ATOMIC);

	if (!msg)
		return NULL;

	hdr = genlmsg_put_reply(msg, info,
			&nlrpl_family, flags, req);
	if (!hdr) {
		nlmsg_free(msg);
		return NULL;
	}

	return msg;
}

int rpl_nl_reply(struct sk_buff *msg, struct genl_info *info)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	void *hdr = genlmsg_data(nlmsg_data(nlh));

	if (genlmsg_end(msg, hdr) < 0)
		goto out;

	return genlmsg_reply(msg, info);
out:
	nlmsg_free(msg);
	return -ENOBUFS;
}

int __init rpl_nl_init(void)
{
	int rc;

	rc = genl_register_family(&nlrpl_family);
	if (rc)
		goto fail;

	rc = nlrpl_dag_conf_register();
	if (rc)
		goto fail;
	rc = nlrpl_dag_info_register();
	if (rc)
		goto fail;
	rc = nlrpl_dag_mng_register();
	if (rc)
		goto fail;

	return 0;

fail:
	genl_unregister_family(&nlrpl_family);
	return rc;
}

void __exit rpl_nl_exit(void)
{
	genl_unregister_family(&nlrpl_family);
}
