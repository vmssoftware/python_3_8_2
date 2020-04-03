# Python test set -- pipeqio functions

import os
import unittest

class PipeQIOTestCase(unittest.TestCase):
    def test_simple_pipe(self):
        import _pipeqio
        import os
        input = b'12345\nabcde'
        r, w = os.pipe()
        # create selector
        with _pipeqio.selector() as selector:
            # register pipes
            selector.register(r)
            selector.register(w)
            num = selector.num
            self.assertEqual(num, 2)
            # query io operations
            selector.query_write(w, input)
            selector.query_read(r)
            # wait
            ready = selector.wait_io(30)
            # get results
            if w in ready:
                written = selector.bytes_count(w)
                self.assertEqual(written, len(input))
                selector.unregister(w)
                num = selector.num
                self.assertEqual(num, 1)
                os.close(w)
            if r in ready:
                data = selector.fetch(r)
                self.assertEqual(data, input)
                is_eof = selector.is_eof(r)
                self.assertEqual(is_eof, False)
            # read until eof
            selector.query_read(r)
            ready = selector.wait_io(30)
            if r in ready:
                data = selector.fetch(r)
                self.assertEqual(data, b'')
                is_eof = selector.is_eof(r)
                self.assertEqual(is_eof, True)
                selector.unregister(r)
                num = selector.num
                self.assertEqual(num, 0)
                os.close(r)

    def test_pipe_as_file(self):
        import _pipeqio
        import os
        import io
        input = b'12345\nabcde'
        r, w = os.pipe()
        read_stream = io.open(r, 'rb')
        write_stream = io.open(w, 'wb')
        # create selector
        with _pipeqio.selector() as selector:
            # register pipes
            selector.register(read_stream)
            selector.register(write_stream)
            num = selector.num
            self.assertEqual(num, 2)
            # query io operations
            selector.query_write(write_stream, input)
            selector.query_read(read_stream)
            # wait
            ready = selector.wait_io(30)
            # get results
            if write_stream in ready:
                written = selector.bytes_count(write_stream)
                self.assertEqual(written, len(input))
                selector.unregister(write_stream)
                num = selector.num
                self.assertEqual(num, 1)
                write_stream.close()
            if read_stream in ready:
                data = selector.fetch(read_stream)
                self.assertEqual(data, input)
                is_eof = selector.is_eof(read_stream)
                self.assertEqual(is_eof, False)
            # read until eof
            selector.query_read(read_stream)
            ready = selector.wait_io(30)
            if read_stream in ready:
                data = selector.fetch(read_stream)
                self.assertEqual(data, b'')
                is_eof = selector.is_eof(read_stream)
                self.assertEqual(is_eof, True)
                selector.unregister(read_stream)
                num = selector.num
                self.assertEqual(num, 0)
                read_stream.close()

    def test_pipe_memview(self):
        import _pipeqio
        import os
        import io
        input = b'12345\nabcde\n' * 1000
        input_view = memoryview(input)
        r, w = os.pipe()
        read_stream = io.open(r, 'rb', 512)
        write_stream = io.open(w, 'wb', 512)
        # create selector
        input_offset = 0
        fileobj2output = {}
        if read_stream:
            fileobj2output[read_stream] = []
        if write_stream:
            fileobj2output[write_stream] = []
        with _pipeqio.selector() as selector:
            if write_stream and input:
                selector.register(write_stream)
                selector.query_write(write_stream, input_view)
            if read_stream and not read_stream.closed:
                selector.register(read_stream)
                selector.query_read(read_stream)

            while selector.num > 0:
                ready = selector.wait_io(None)

                for stream in ready:
                    if stream is write_stream:
                        input_offset += selector.bytes_count(stream)
                        if input_offset >= len(input):
                            selector.unregister(stream)
                            stream.close()
                        else:
                            selector.query_write(write_stream, input_view[input_offset:])
                    elif stream in (read_stream, ):
                        data = selector.fetch(stream)
                        fileobj2output[stream].append(data)
                        if selector.is_eof(stream):
                            selector.unregister(stream)
                            stream.close()
                        else:
                            selector.query_read(stream)

        stdout = b''.join(fileobj2output[read_stream])
        self.assertEqual(stdout, input)
    
    def test_vfork_exec_communicate(self):
        import _pipeqio
        data = b'123\nABC'
        stdout, stderr = _pipeqio.vfork_exec_communicate(
            [b'/DSA20/VORFOLOMEEV/child/out/RELEASE/child.exe'],    #executable (list of bytes)
            ['/DSA20/VORFOLOMEEV/child/out/RELEASE/child.exe'],     #args (list of strings)
            None,           #cwd
            None,           #env (list of bytes)
            data)           #stdin override
        self.assertEqual(stdout, data)

    # def test_pipe_subprocess(self):
    #     import _pipeqio
    #     import os

    #     pid, p2cwrite, c2pread, errread = _pipeqio.vfork_exec(
    #         [b'/DSA20/VORFOLOMEEV/child/out/RELEASE/child.exe'],    #executable (list of bytes)
    #         ['/DSA20/VORFOLOMEEV/child/out/RELEASE/child.exe'],     #args (list of strings)
    #         None,       #cwd
    #         None,       #env (list of bytes)
    #         True,       #stdin override
    #         True,       #stdout override
    #         False)       #stderr override

    #     input = b'12345\nabcde\n' * 10
    #     input_view = memoryview(input)

    #     # create selector
    #     input_offset = 0
    #     fileobj2output = {}
    #     if c2pread == -1:
    #         c2pread = None
    #     else:
    #         fileobj2output[c2pread] = []
    #     if errread == -1:
    #         errread = None
    #     else:
    #         fileobj2output[errread] = []
    #     with _pipeqio.selector() as selector:
    #         if p2cwrite and input:
    #             selector.register(p2cwrite)
    #             selector.query_write(p2cwrite, input_view)
    #         if c2pread:
    #             selector.register(c2pread)
    #             selector.query_read(c2pread)
    #         if errread:
    #             selector.register(errread)
    #             selector.query_read(errread)

    #         while selector.num > 0:
    #             ready = selector.wait_io(None)
    #             for stream in ready:
    #                 if stream is p2cwrite:
    #                     input_offset += selector.bytes_count(stream)
    #                     if input_offset >= len(input):
    #                         selector.unregister(stream)
    #                         os.close(stream)
    #                     else:
    #                         selector.query_write(p2cwrite, input_view[input_offset:])
    #                 elif stream in (c2pread, errread):
    #                     data = selector.fetch(stream)
    #                     fileobj2output[stream].append(data)
    #                     if selector.is_eof(stream):
    #                         selector.unregister(stream)
    #                         os.close(stream)
    #                     else:
    #                         selector.query_read(stream)

    #     if c2pread:
    #         stdout = b''.join(fileobj2output[c2pread])
    #         self.assertEqual(stdout, input)
    #     if errread:
    #         stderr = b''.join(fileobj2output[errread])
    #         pass
    #     pass

if __name__ == "__main__":
    unittest.main()
