# Python test set -- pipeqio functions

import os
import unittest

class PipeQIOTestCase(unittest.TestCase):
    def test_one(self):
        import _pipeqio
        import os
        r, w = os.pipe()
        # create selector
        with _pipeqio.selector() as selector:
            # register pipes
            selector.register(r)
            selector.register(w)
            num = selector.num
            self.assertEqual(num, 2)
            # query io operations
            data_to_write = b'12345\nabcde'
            selector.query_write(w, data_to_write)
            selector.query_read(r)
            # wait
            ready = selector.wait_io(30)
            # get results
            if w in ready:
                written = selector.bytes_count(w)
                self.assertEqual(written, len(data_to_write))
                selector.unregister(w)
                num = selector.num
                self.assertEqual(num, 1)
                os.close(w)
            if r in ready:
                data = selector.fetch(r)
                self.assertEqual(data, data_to_write)
                is_eof = selector.is_eof(r)
                self.assertEqual(is_eof, False)
            # read until eof
            selector.query_read(r)
            ready = selector.wait_io(30)
            if r in ready:
                data = selector.fetch(r)
                self.assertEqual(data, None)
                is_eof = selector.is_eof(r)
                self.assertEqual(is_eof, True)
                selector.unregister(r)
                num = selector.num
                self.assertEqual(num, 0)
                os.close(r)

if __name__ == "__main__":
    unittest.main()
