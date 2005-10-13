/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* 
 * Test file created by Linas Vepstas <linas@linas.org>
 * Minimal test to see if a book can be split into two periods
 * without crashing.  Book is written to the database.
 * December 2001
 */

#include <ctype.h>
#include <glib.h>
#include <time.h>

#include "Account.h"
#include "Group.h"
#include "Period.h"
#include "qof.h"
#include "test-stuff.h"
#include "test-engine-stuff.h"
#include "Transaction.h"

#define PG_LIB_NAME "libgnc-backend-postgres.la"
#define PG_LIB_INIT "pgend_provider_init"

static void
run_test (void)
{
  QofBackendError io_err;
  QofSession *session;
  QofBook *openbook, *closedbook;
  AccountGroup *grp;
  AccountList *acclist, *anode;
  Account * acc = NULL;
  SplitList *splist;
  Split *sfirst, *slast;
  Transaction *tfirst, *tlast;
  Timespec tsfirst, tslast, tsmiddle;
  char * test_url;
  
  do_test( qof_load_backend_library (QOF_LIB_DIR, 
			PG_LIB_NAME, PG_LIB_INIT),
			" loading gnc-backend-postgres GModule failed");

  session = get_random_session ();

  test_url = "postgres://localhost/qqq?mode=single-update";
  qof_session_begin (session, test_url, FALSE, TRUE);

  io_err = qof_session_get_error (session);
  g_return_if_fail (io_err == ERR_BACKEND_NO_ERR);

  openbook = qof_session_get_book (session);
  if (!openbook)
  {
    failure("book not created");
    exit(get_rv());
  }

  add_random_transactions_to_book (openbook, 12);

  grp = gnc_book_get_group (openbook);

  acclist = xaccGroupGetSubAccounts (grp);
  for (anode=acclist; anode; anode=anode->next)
  {
    int ns;
    acc = anode->data;
    ns = g_list_length (xaccAccountGetSplitList (acc));
    if (2 <= ns) break;
    acc = NULL;
  }

  if(!acc)
  {
    failure("group didn't have accounts with enough splits");
    exit(get_rv());
  }

  splist = xaccAccountGetSplitList(acc);
  if(!splist)
  {
    failure("account has no transactions");
    exit(get_rv());
  }

  sfirst = splist->data;
  slast = g_list_last(splist) ->data;
  if (sfirst == slast)
  {
    failure("account doesn't have enough transactions");
    exit(get_rv());
  }

  tfirst = xaccSplitGetParent (sfirst);
  tlast = xaccSplitGetParent (slast);
  
  if (!tfirst || !tlast)
  {
    failure("malformed transactions in account");
    exit(get_rv());
  }

  tsfirst = xaccTransRetDatePostedTS (tfirst);
  tslast = xaccTransRetDatePostedTS (tlast);

  if (tsfirst.tv_sec == tslast.tv_sec)
  {
    failure("transactions not time separated");
    exit(get_rv());
  }

  tsmiddle = tsfirst;
  tsmiddle.tv_sec = (tsfirst.tv_sec + tslast.tv_sec)/2;

  qof_session_save (session, NULL);
  io_err = qof_session_get_error (session);
  g_return_if_fail (io_err == ERR_BACKEND_NO_ERR);

  gnc_set_logfile (stdout);
  gnc_set_log_level_global (GNC_LOG_INFO);
  closedbook = gnc_book_close_period (openbook, tsmiddle, 
                  NULL, "this is opening balance dude");

  if (!closedbook)
  {
    failure("closed book not created");
    exit(get_rv());
  }

  qof_session_save (session, NULL);
  io_err = qof_session_get_error (session);
  g_return_if_fail (io_err == ERR_BACKEND_NO_ERR);

  qof_session_end (session);
  io_err = qof_session_get_error (session);
  g_return_if_fail (io_err == ERR_BACKEND_NO_ERR);

  success ("periods lightly tested and seem to work");
}

int
main (int argc, char **argv)
{
  qof_init();
  run_test ();

  print_test_results();
  qof_close();
  return 0;
}
