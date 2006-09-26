/*
 * Copyright (c) 2005 William Pitcock, et al.
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Lists object properties via their metadata table.
 *
 * $Id: taxonomy.c 6489 2006-09-26 15:47:06Z jilles $
 */

#include "atheme.h"

DECLARE_MODULE_V1
(
	"nickserv/taxonomy", FALSE, _modinit, _moddeinit,
	"$Id: taxonomy.c 6489 2006-09-26 15:47:06Z jilles $",
	"Atheme Development Group <http://www.atheme.org>"
);

static void ns_cmd_taxonomy(sourceinfo_t *si, int parc, char *parv[]);

command_t ns_taxonomy = { "TAXONOMY", "Displays a user's metadata.", AC_NONE, 1, ns_cmd_taxonomy };

list_t *ns_cmdtree, *ns_helptree;

void _modinit(module_t *m)
{
	MODULE_USE_SYMBOL(ns_cmdtree, "nickserv/main", "ns_cmdtree");
	MODULE_USE_SYMBOL(ns_helptree, "nickserv/main", "ns_helptree");

	command_add(&ns_taxonomy, ns_cmdtree);
	help_addentry(ns_helptree, "TAXONOMY", "help/nickserv/taxonomy", NULL);
}

void _moddeinit()
{
	command_delete(&ns_taxonomy, ns_cmdtree);
	help_delentry(ns_helptree, "TAXONOMY");
}

static void ns_cmd_taxonomy(sourceinfo_t *si, int parc, char *parv[])
{
	char *target = parv[0];
	myuser_t *mu;
	node_t *n;
	boolean_t isoper;

	if (!target)
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "TAXONOMY");
		command_fail(si, fault_needmoreparams, "Syntax: TAXONOMY <nick>");
		return;
	}

	if (!(mu = myuser_find_ext(target)))
	{
		command_fail(si, fault_badparams, "\2%s\2 is not registered.", target);
		return;
	}

	isoper = has_priv(si->su, PRIV_USER_AUSPEX);
	if (isoper)
		logcommand(nicksvs.me, si->su, CMDLOG_ADMIN, "TAXONOMY %s (oper)", target);
	else
		logcommand(nicksvs.me, si->su, CMDLOG_GET, "TAXONOMY %s", target);

	command_success_nodata(si, "Taxonomy for \2%s\2:", target);

	LIST_FOREACH(n, mu->metadata.head)
	{
		metadata_t *md = n->data;

		if (md->private == TRUE && !isoper)
			continue;

		command_success_nodata(si, "%-32s: %s", md->name, md->value);
	}

	command_success_nodata(si, "End of \2%s\2 taxonomy.", target);
}
