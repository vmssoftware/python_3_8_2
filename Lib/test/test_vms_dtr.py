import sys
import unittest

if sys.platform != 'OpenVMS':
    raise unittest.SkipTest('OpenVMS required')

import vms.rec as rec
import vms.dtr as dtr
import vms.ssdef as SS

class BaseTestCase(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_demo_01(self):
        """ tests demo 01 """
        r = rec.new()
        rec.addstr(r, None, 10, 0)
        rec.addstr(r, None, 10, 10)
        rec.addstr(r, None, 6, 20)
        rec.addstr(r, None, 3, 26)
        rec.addstr(r, None, 5, 29)
        rec.addstr(r, None, 2, 34)
        rec.addstr(r, None, 5, 36)

        sts, dab = dtr.init(100, 0)

        self.assertEqual(sts, SS.SS__NORMAL, "Initialization failed")

        sts, cond, state = dtr.command(dab, "set dictionary cdd$top.dtr$lib.demo;")

        self.assertEqual(sts, SS.SS__NORMAL, "Command 'set dictionary' failed")

        if state == dtr._K_STL_MSG:
            errorMsg = dtr.msg_buf(dab)
            dtr.cont(dab)

        sts, cond, state = dtr.command(dab, "declare port yport using yacht;")

        self.assertEqual(sts, SS.SS__NORMAL, "Command 'declare port' failed")

        while state == dtr._K_STL_MSG:
            errorMsg = dtr.msg_buf(dab)
            sts, cond, state = dtr.cont(dab)

        sts, cond, state = dtr.command(dab, "ready yachts; ready yport write;")

        self.assertEqual(sts, SS.SS__NORMAL, "Command 'ready' failed")

        while state == dtr._K_STL_MSG:
            errorMsg = dtr.msg_buf(dab)
            sts, cond, state = dtr.cont(dab)

        sts, cond, state = dtr.command(dab, "yport = yachts with loa > 30;")

        self.assertEqual(sts, SS.SS__NORMAL, "Command '=' failed")

        while state == dtr._K_STL_MSG:
            errorMsg = dtr.msg_buf(dab)
            sts, cond, state = dtr.cont(dab)

        while state == dtr._K_STL_PGET:
            sts = dtr.get_port(dab, r)

            self.assertEqual(sts, SS.SS__NORMAL, "Unable to retrieve data")

            dataStr = "%-10s\t%-10s\t%-6s\t%-3s\t%-5s\t%-2s\t%-5s" % (rec.getstr(r, 0), rec.getstr(r, 1), rec.getstr(r, 2), rec.getstr(r, 3), rec.getstr(r, 4), rec.getstr(r, 5), rec.getstr(r, 6))

            state = dtr.state(dab)

        sts = dtr.finish(dab)

        self.assertEqual(sts, SS.SS__NORMAL, "Problems tidying up")

        rec.delete(r)

    def test_demo_02(self):
        """ tests demo 02 """
        sts, dab = dtr.init(100, 0)

        self.assertEqual(sts, SS.SS__NORMAL, "initialization failed")

        sts, cond, state = dtr.command(dab, "set dictionary cdd$top.dtr$lib.demo.rdb;")

        self.assertEqual(sts, SS.SS__NORMAL, "Command 'set dictionary' failed")

        if state == dtr._K_STL_MSG:
            errorMsg = dtr.msg_buf(dab)
            dtr.cont(dab)

        sts, cond, state = dtr.command(dab, "ready jobs shared read;")

        self.assertEqual(sts, SS.SS__NORMAL, "Command 'ready' failed")

        while state == dtr._K_STL_MSG:
            errorMsg = dtr.msg_buf(dab)
            sts, cond, state = dtr.cont(dab)

        sts, cond, state = dtr.command(dab, "print jobs;")

        self.assertEqual(sts, SS.SS__NORMAL, "Command 'print' failed")

        sts = dtr.skip(dab, 4)

        fmt = "%4c   %1c   %20c %11c  %11c"

        while dtr.state(dab) == dtr._K_STL_LINE:
            row = dtr.row(dab, fmt)

        sts = dtr.finish(dab)

        self.assertEqual(sts, SS.SS__NORMAL, "Problems tidying up")

if __name__ == "__main__":
    unittest.main(verbosity=2)
