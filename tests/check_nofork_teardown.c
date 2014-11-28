#include <stdio.h>
#include <stdlib.h>
#include "check.h"

/**
 * This test checks the result if in CK_NOFORK
 * mode a unit test fails but a checked teardown
 * runs after the failed test.
 *
 * Previously, the failure would be reported as:
 *
 * 0%: Checks: 1, Failures: 1, Errors: 0
 * (null):-1:S:tc:will_fail:0: Assertion '0' failed
 *
 * The reason why this happens is this: the end of the
 * message sequence coming down the pipe is CK_MSG_LOC
 * (location of failing test), CK_MSG_FAIL, CK_MSG_CTX
 * (TEARDOWN). It is this final message that confuses
 * things, because rcvmsg_update_ctx() updates
 * rmsg->lastctx (which likely is the right thing for it
 * to do), which is the ctx value used by the first 'if'
 * body in construct_test_result() in its call to
 * tr_set_loc_by_ctx().
 *
 * After the fix, the test failure should be reported
 * as:
 *
 * 0%: Checks: 1, Failures: 1, Errors: 0
 * check_nofork_teardown.c:33:F:tc:will_fail:0: Assertion '0' failed
 */

START_TEST( will_fail )
{
    ck_assert(0);
}
END_TEST

static void empty_checked_teardown( void )
{
}

int main( void )
{
    SRunner *sr = srunner_create( NULL );
    Suite *s = suite_create( "bug-99" );
    TCase *tc = tcase_create( "tc" );
    int result;

    srunner_add_suite( sr, s );
    srunner_set_fork_status( sr, CK_NOFORK );
    suite_add_tcase( s, tc );
    tcase_add_checked_fixture( tc, NULL, empty_checked_teardown );
    tcase_add_test( tc, will_fail );

    srunner_run_all( sr, CK_ENV );
    result = srunner_ntests_failed( sr ) ? EXIT_FAILURE : EXIT_SUCCESS;
    srunner_free( sr );

    return result;
}
